# Quark SELinux consolidation through stage 35

Branch: A12+

Android 12L/API 32 port for bring-up. Full Android 12 build and hardware validation remain pending. Preserve the existing userdebug/eng permissive boot configuration.

This branch consolidates the source and camera-blob changes used by the incremental SELinux validation. Use the matching local_manifests branch to select all policy repositories together. No SELINUX_IGNORE_NEVERALLOWS bypass is required for the validated Android 11 policy. The custom legacy HAL1 camera ownership contract is not a claim of stock AOSP policy/CTS conformance.

Pending investigations: thermal-engine DAC audit records, camera_prop reads and third-party diagnostic apps such as AIDA64. Normal observed temperature does not establish that every control path is validated. The integrated overlay has read-only metric access; no broad DAC capabilities were restored.
