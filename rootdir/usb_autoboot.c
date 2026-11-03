/* Copyright 2026 The LineageOS Project. SPDX-License-Identifier: Apache-2.0 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <android/log.h>
#include <cutils/properties.h>
#include "usb_boot_policy.h"

#define TAG "quark-usb-autoboot"
#define CONFIG_DIR "/dev/quark-usb-pref/quark_usb_boot"
#define ENABLED_PROP "sys.quark.usb_autoboot_enabled"
#define ACTIVE_PROP "sys.quark.usb_autoboot_active"
#define POWER "/sys/class/power_supply/"
#define TMPFS_MAGIC 0x01021994

static bool charger_mode(void) {
    char value[PROPERTY_VALUE_MAX];
    property_get("ro.bootmode", value, "");
    return strcmp(value, "charger") == 0;
}

static bool preference_enabled(void) {
    bool enabled = false;
    int directory = open(CONFIG_DIR, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (directory < 0) return false;
    int fd = openat(directory, "enabled", O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
    close(directory);
    if (fd >= 0) {
        struct stat st;
        char value[3];
        if (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_uid == 1000 &&
            st.st_nlink == 1 && st.st_size == 2 && !(st.st_mode & 0022) &&
            read(fd, value, sizeof(value)) == 2 && value[0] == '1' && value[1] == '\n') {
            enabled = true;
        }
        close(fd);
    }
    return enabled;
}

static bool read_text(const char *relative, char *value, size_t size) {
    char path[PATH_MAX];
    if (snprintf(path, sizeof(path), POWER "%s", relative) >= (int)sizeof(path)) return false;
    FILE *file = fopen(path, "re");
    if (!file) return false;
    bool ok = fgets(value, (int)size, file) != NULL;
    fclose(file);
    if (ok) value[strcspn(value, "\r\n")] = '\0';
    return ok;
}

static long read_number(const char *relative) {
    char value[64], *end;
    if (!read_text(relative, value, sizeof(value))) return -1;
    errno = 0;
    long number = strtol(value, &end, 10);
    return errno || end == value || *end ? -1 : number;
}

static bool private_mount_released(void) {
    struct statfs st;
    return statfs("/dev/quark-usb-pref", &st) == 0 &&
           (unsigned long)st.f_type == TMPFS_MAGIC;
}

int main(int argc, char **argv) {
    if (!charger_mode()) return 0;
    if (argc == 2 && strcmp(argv[1], "--read-preference") == 0) {
        return property_set(ENABLED_PROP, preference_enabled() ? "1" : "0") == 0 ? 0 : 1;
    }
    if (argc != 1) return 1;
    char value[PROPERTY_VALUE_MAX];
    property_get(ENABLED_PROP, value, "0");
    if (strcmp(value, "1") != 0) return 0;
    if (!private_mount_released()) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Preference mount not released; keeping charger mode");
        return 1;
    }
    __android_log_print(ANDROID_LOG_INFO, TAG, "USB autoboot enabled; waiting for stable battery >=5%% and >=3.6V");
    unsigned int stable = 0;
    for (;;) {
        char health[64] = "", status[64] = "";
        read_text("battery/health", health, sizeof(health));
        read_text("battery/status", status, sizeof(status));
        long capacity = read_number("battery/capacity");
        bool ready = capacity >= 0 && capacity <= 100 && quark_usb_boot_ready(
            (int)capacity, read_number("battery/voltage_now"),
            (int)read_number("battery/present"), (int)read_number("usb/present"),
            (int)read_number("usb/online"), health, status);
        stable = ready ? stable + 1 : 0;
        if (stable >= 3) {
            if (!charger_mode() || !private_mount_released()) return 1;
            /* init stops the charger class and continues late-init; no reboot. */
            if (property_set(ACTIVE_PROP, "1") != 0) return 1;
            if (property_set("sys.boot_from_charger_mode", "1") != 0) {
                property_set(ACTIVE_PROP, "0");
                return 1;
            }
            return 0;
        }
        sleep(2);
    }
}
