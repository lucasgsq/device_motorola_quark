/* Copyright 2026 The LineageOS Project. SPDX-License-Identifier: Apache-2.0 */
package com.lineageos.settings.device;

import android.system.ErrnoException;
import android.system.Os;
import android.util.AtomicFile;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/** Device-wide preference; shared preferences are unavailable in charger mode. */
public final class UsbAutoBootSettings {
    private static final String TAG = "QuarkUsbAutoBoot";
    private static final AtomicFile CONFIG =
            new AtomicFile(new File("/persist/quark_usb_boot/enabled"));

    private UsbAutoBootSettings() {}

    private static boolean readFlag() throws IOException {
        try (FileInputStream in = CONFIG.openRead()) {
            int value = in.read();
            if ((value != '0' && value != '1') || in.read() != '\n' || in.read() != -1) {
                throw new IOException("Invalid USB boot preference");
            }
            return value == '1';
        }
    }

    public static synchronized boolean isEnabled() {
        try {
            return readFlag();
        } catch (IOException e) {
            return false;
        }
    }

    public static synchronized boolean setEnabled(boolean enabled) {
        FileOutputStream output = null;
        try {
            output = CONFIG.startWrite();
            Os.fchmod(output.getFD(), 0600);
            output.write(enabled ? new byte[] {'1', '\n'} : new byte[] {'0', '\n'});
            CONFIG.finishWrite(output);
            output = null;
            return readFlag() == enabled;
        } catch (IOException | ErrnoException e) {
            if (output != null) CONFIG.failWrite(output);
            Log.e(TAG, "Could not persist USB boot preference", e);
            return false;
        }
    }
}
