# Quark A12+ device tree bring-up

This branch is an experimental Android 12+ port, not a boot-tested release.
Use A11+ for LineageOS 18.1. No manifest was switched automatically.

Initial base: A11+ 06687bfe (including current audio experiments).
Shamu references: official LineageOS lineage-18.1 at 80fa688a;
rootfan los-19.1 at f97ae82e and los-20.0 at 62dc0728.
There is no official Shamu lineage-19.1 branch in the queried repository.

## Initial adaptations

- Move Quark Wi-Fi and tethering overlays to the Android 12 module paths,
  preserving their content (rootfan 349a224e).
- Install Quark ueventd.rc in vendor/etc (rootfan 647f0465).

## Required follow-up before an Android 12 build or boot claim

- Select a matching platform manifest, hardware trees, proprietary blobs and
  toolchain; audit all local signing configuration separately.
- Reconcile HIDL/VINTF packages, power HAL, keymaster, DRM and radio interfaces.
  Do not copy Shamu camera or audio HAL implementations over the Quark blobs.
- Review Android 12 SELinux changes against Quark denials while preserving
  enforcing. Do not import permissive or blanket BUILD_BROKEN settings.
- Keep partition sizes, boot offsets, fstab and encryption specific to Quark.
  Removing encryptable is not an acceptable generic compatibility fix.
- Audit kernel networking/BPF requirements. A property claiming BPF support
  does not provide a missing kernel implementation.
- Validate Android 12 requirements against kernel A12+, currently aligned
  with the A11+ baseline and with the problematic math64 change reverted.
- IMS/VoLTE depends on matching blobs, services, policy, modem and carrier
  support; do not enable it by copying property overrides alone.
- The perfd/audio_cutback move requires patched binaries as well as init and
  SELinux changes. Do not port only the directory names.

Android 13-specific Bluetooth, rmnet and power-profile changes remain deferred.
No runtime validation has been performed for this branch.

## Temporary SELinux diagnostics

A12+ userdebug/eng builds temporarily pass both enforcing=0 (kernel) and
androidboot.selinux=permissive (Android init). User builds remain enforcing.
SELinux is still enabled and policy is still loaded, so AVC denials can be
collected. The kernel retains CONFIG_SECURITY_SELINUX_DEVELOP=y.

This does not fix SELinux policy compilation errors or policy-load failures.
It only prevents enforcement of access denials after policy loads. Do not
disable SELinux or remove encryption to work around unrelated boot failures.

After boot, check `adb shell getenforce`, `adb shell cat /proc/cmdline` and
`adb shell getprop ro.boot.selinux`. A debug diagnostic boot should report
Permissive and the two parameters above. Capture logcat and, with adb root,
dmesg for AVCs. Do not blindly turn every denial into an allow rule.

For a temporary enforcing test use `adb shell setenforce 1` after `adb root`;
for the permanent fix restore enforcing=1 androidboot.selinux=enforcing in
BoardConfig.mk, rebuild boot.img and verify the mode again after reboot.

## A11+ synchronization checkpoint

Kernel A12+ was advanced without rewriting history to f9c074c2cab, matching
the current A11+ kernel tree. The problematic math64 change is reverted;
the keys and perf updates are retained. No Android 12-specific kernel
series has been added yet.

Device A12+ already contains A11+ 06687bfe, including the pending microphone
gain and headset volume experiments. Its own Android 12 migrations remain.
The A12+ local_manifests branch targets LineageOS 19.1 and explicitly includes
legacy APQ8084 sources that are absent from the upstream Android 12 manifest.
These legacy components still require build/API compatibility validation.

configs/apq8084-Android.mk is linked into hardware/qcom-caf/apq8084/Android.mk
by the A12+ local manifest. It uses the same namespace guard as the official
hardware/qcom-caf/common os_pickup.mk, preserving the A11 layout while the
Android 12 upstream manifest no longer creates that legacy guard.
