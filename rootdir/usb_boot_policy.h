#ifndef QUARK_USB_BOOT_POLICY_H
#define QUARK_USB_BOOT_POLICY_H

#include <stdbool.h>
#include <string.h>

/* Percent and microvolts. These are boot gates, not charging limits. */
static inline bool quark_usb_boot_ready(int capacity, long voltage, int present,
                                      int usb_present, int usb_online,
                                      const char *health, const char *status) {
    return capacity >= 5 && capacity <= 100 &&
           voltage >= 3600000 && voltage <= 4500000 &&
           present == 1 && usb_present == 1 && usb_online == 1 &&
           strcmp(health, "Good") == 0 &&
           (strcmp(status, "Charging") == 0 || strcmp(status, "Full") == 0);
}

#endif
