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
- Audit the existing kernel A12+ branch before use: it was created from an
  older checkpoint containing the math64 change associated with microphone
  regressions. Its name does not imply a validated matching kernel.
- IMS/VoLTE depends on matching blobs, services, policy, modem and carrier
  support; do not enable it by copying property overrides alone.
- The perfd/audio_cutback move requires patched binaries as well as init and
  SELinux changes. Do not port only the directory names.

Android 13-specific Bluetooth, rmnet and power-profile changes remain deferred.
No runtime validation has been performed for this branch.
