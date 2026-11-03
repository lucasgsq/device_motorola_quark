#include <assert.h>
#include <stdio.h>
#include "../rootdir/usb_boot_policy.h"

int main(void) {
    assert(quark_usb_boot_ready(5, 3600000, 1, 1, 1, "Good", "Charging"));
    assert(quark_usb_boot_ready(100, 4293437, 1, 1, 1, "Good", "Full"));
    assert(!quark_usb_boot_ready(4, 3600000, 1, 1, 1, "Good", "Charging"));
    assert(!quark_usb_boot_ready(-1, 4000000, 1, 1, 1, "Good", "Charging"));
    assert(!quark_usb_boot_ready(101, 4000000, 1, 1, 1, "Good", "Charging"));
    assert(!quark_usb_boot_ready(50, 3599999, 1, 1, 1, "Good", "Charging"));
    assert(!quark_usb_boot_ready(50, 4500001, 1, 1, 1, "Good", "Charging"));
    assert(!quark_usb_boot_ready(50, 4000000, 0, 1, 1, "Good", "Charging"));
    assert(!quark_usb_boot_ready(50, 4000000, 1, 0, 1, "Good", "Charging"));
    assert(!quark_usb_boot_ready(50, 4000000, 1, 1, 0, "Good", "Charging"));
    assert(!quark_usb_boot_ready(50, 4000000, 1, 1, 1, "Overheat", "Charging"));
    assert(!quark_usb_boot_ready(50, 4000000, 1, 1, 1, "Unknown", "Charging"));
    assert(!quark_usb_boot_ready(50, 4000000, 1, 1, 1, "Good", "Discharging"));
    assert(!quark_usb_boot_ready(50, 4000000, 1, 1, 1, "Good", "Not charging"));
    puts("14 battery/USB policy cases passed");
    return 0;
}
