/* Preserve the legacy camera album before starting the vendor daemon.
 * No shell, capabilities, ownership changes, or overwrite of an existing album.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifndef QUARK_MIGRATE_TEST
#include <android/log.h>
#define REPORT(...) __android_log_print(ANDROID_LOG_ERROR, "quark-camera-start", __VA_ARGS__)
#else
#define REPORT(...) fprintf(stderr, __VA_ARGS__)
#endif

static int valid_file(const struct stat *st, uid_t owner) {
    return S_ISREG(st->st_mode) && st->st_uid == owner &&
           st->st_nlink == 1 && !(st->st_mode & 0007) &&
           st->st_size >= 0 && st->st_size <= 64 * 1024 * 1024;
}

static int migrate(const char *old_path, const char *directory, uid_t owner) {
    const char *temporary = ".fdb.migrating";
    struct stat st, before, after;
    int dir = -1, source = -1, output = -1, existing = -1;
    int result = -1, created = 0, saved_errno;
    char buffer[16384];
    ssize_t count;
    off_t copied = 0;
    dir = open(directory, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (dir < 0 || fstat(dir, &st) < 0) goto done;
    if (st.st_uid != owner || (st.st_mode & 0007)) { errno = EPERM; goto done; }
    existing = openat(dir, "fdb", O_RDONLY | O_NONBLOCK | O_NOFOLLOW | O_CLOEXEC);
    if (existing >= 0) {
        if (fstat(existing, &st) < 0) goto done;
        if (S_ISREG(st.st_mode) && st.st_uid == owner && st.st_nlink == 2) {
            struct stat staged;
            if (fstatat(dir, temporary, &staged, AT_SYMLINK_NOFOLLOW) < 0) goto done;
            if (!S_ISREG(staged.st_mode) || st.st_dev != staged.st_dev ||
                st.st_ino != staged.st_ino) { errno = EPERM; goto done; }
            if (unlinkat(dir, temporary, 0) < 0 || fsync(dir) < 0 ||
                fstat(existing, &st) < 0) goto done;
        }
        if (!valid_file(&st, owner)) { errno = EPERM; goto done; }
        result = 0; goto done; /* Never overwrite a newer destination. */
    }
    if (errno != ENOENT) goto done;
    source = open(old_path, O_RDONLY | O_NONBLOCK | O_NOFOLLOW | O_CLOEXEC);
    if (source < 0) {
        if (errno == ENOENT) result = 0; /* Fresh install: vendor creates its database. */
        goto done;
    }
    if (fstat(source, &before) < 0) goto done;
    if (!valid_file(&before, owner)) { errno = EPERM; goto done; }
    /* Clean only our own regular staging file left by an interrupted copy. */
    if (fstatat(dir, temporary, &st, AT_SYMLINK_NOFOLLOW) == 0) {
        if (!valid_file(&st, owner)) { errno = EPERM; goto done; }
        if (unlinkat(dir, temporary, 0) < 0) goto done;
    } else if (errno != ENOENT) goto done;
    output = openat(dir, temporary, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (output < 0) goto done;
    created = 1;
    for (;;) {
        count = read(source, buffer, sizeof(buffer));
        if (count < 0) { if (errno == EINTR) continue; goto done; }
        if (!count) break;
        copied += count;
        if (copied > before.st_size) { errno = EBUSY; goto done; }
        ssize_t offset = 0;
        while (offset < count) {
            ssize_t written = write(output, buffer + offset, (size_t)(count - offset));
            if (written < 0 && errno == EINTR) continue;
            if (written <= 0) { if (!written) errno = EIO; goto done; }
            offset += written;
        }
    }
    if (fstat(source, &after) < 0) goto done;
    if (copied != before.st_size || after.st_size != before.st_size ||
        after.st_mtim.tv_sec != before.st_mtim.tv_sec ||
        after.st_mtim.tv_nsec != before.st_mtim.tv_nsec ||
        after.st_ctim.tv_sec != before.st_ctim.tv_sec ||
        after.st_ctim.tv_nsec != before.st_ctim.tv_nsec) { errno = EBUSY; goto done; }
    if (fchmod(output, 0660) < 0 || fsync(output) < 0) goto done;
    if (close(output) < 0) { output = -1; goto done; }
    output = -1;
    /* Atomic publication without replacement; compatible with Linux 3.10. */
    if (linkat(dir, temporary, dir, "fdb", 0) < 0) goto done;
    if (unlinkat(dir, temporary, 0) < 0) goto done;
    created = 0;
    if (fsync(dir) < 0) goto done;
    result = 0;
done:
    saved_errno = errno;
    if (output >= 0) close(output);
    if (source >= 0) close(source);
    if (existing >= 0) close(existing);
    if (created) unlinkat(dir, temporary, 0);
    if (dir >= 0) close(dir);
    errno = saved_errno;
    return result;
}

#ifdef QUARK_MIGRATE_TEST
int main(int argc, char **argv) {
    if (argc != 3) return 2;
    if (migrate(argv[1], argv[2], getuid()) < 0) {
        REPORT("migration failed: %s\n", strerror(errno)); return 1;
    }
    return 0;
}
#else
int main(void) {
    if (getuid() != 1006 || getgid() != 1006) {
        REPORT("Unexpected camera credentials; refusing startup"); return 1;
    }
    if (migrate("/data/fdAlbum", "/data/cam", 1006) < 0) {
        REPORT("Album migration failed; daemon not started: %s", strerror(errno)); return 1;
    }
    execl("/vendor/bin/mm-qcamera-daemon", "mm-qcamera-daemon", (char *)NULL);
    REPORT("Cannot execute camera daemon: %s", strerror(errno));
    return 1;
}
#endif
