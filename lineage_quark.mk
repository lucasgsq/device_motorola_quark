# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2018 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Boot animation
TARGET_SCREEN_HEIGHT := 2560
TARGET_SCREEN_WIDTH := 1440
TARGET_BOOTANIMATION_HALF_RES := true

# Inherit some common stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit from hardware-specific part of the product configuration
$(call inherit-product, $(LOCAL_PATH)/device.mk)

#This will make TWRP backups name same as ROM zip name
#QUARK_LOS_ROM_VERSION=lineage-18.1-$(shell date -u +%Y%m%d)-MOD-quark

#PRODUCT_BUILD_PROP_OVERRIDES += \
    #BUILD_DISPLAY_ID=$(QUARK_LOS_ROM_VERSION)

BUILD_FINGERPRINT := motorola/quark_retbr/quark_umts:6.0.1/MPGS24.107-70.2-7/7:user/release-keys
PRODUCT_DEVICE := quark
PRODUCT_NAME := lineage_quark
PRODUCT_BRAND := motorola
PRODUCT_MODEL := quark
PRODUCT_MANUFACTURER := motorola

PRODUCT_GMS_CLIENTID_BASE := android-motorola

COMMON_LUNCH_CHOICES := \
    lineage_quark-user \
    lineage_quark-userdebug \
    lineage_quark-eng
