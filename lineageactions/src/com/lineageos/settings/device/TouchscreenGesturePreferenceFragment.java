/*
 * Copyright (C) 2015 The lineageos Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.lineageos.settings.device;

import android.app.ActionBar;
import android.os.Bundle;
import android.os.UserHandle;
import android.widget.Toast;
import android.provider.Settings;
import androidx.preference.Preference;
import androidx.preference.PreferenceCategory;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceFragment;
import androidx.preference.SwitchPreference;

import android.view.Menu;
import android.view.MenuItem;

public class TouchscreenGesturePreferenceFragment extends PreferenceFragment implements
Preference.OnPreferenceChangeListener {

    private static final String CATEGORY_AMBIENT_DISPLAY = "ambient_display_key";
    private static final String SWITCH_AMBIENT_DISPLAY = "ambient_display_switch";
    private SwitchPreference mSwitchAmbientDisplay;
    private SwitchPreference mUsbAutoBoot;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.gesture_panel);
        final ActionBar actionBar = getActivity().getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        mSwitchAmbientDisplay = (SwitchPreference) findPreference(SWITCH_AMBIENT_DISPLAY);
        mSwitchAmbientDisplay.setOnPreferenceChangeListener(this);

        mUsbAutoBoot = (SwitchPreference) findPreference("usb_auto_boot");
        mUsbAutoBoot.setOnPreferenceChangeListener(this);
        mUsbAutoBoot.setEnabled(UserHandle.myUserId() == UserHandle.USER_SYSTEM);

        updateState();
    }

    @Override
    public void onResume() {
        super.onResume();
        updateState();
    }

    private void updateState() {
        if (mUsbAutoBoot != null) {
            mUsbAutoBoot.setChecked(UsbAutoBootSettings.isEnabled());
        }
        if (mSwitchAmbientDisplay != null)
            mSwitchAmbientDisplay.setChecked(LineageActionsSettings.isDozeEnabled(getActivity().getContentResolver()));
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object objValue) {
        if (preference == mUsbAutoBoot) {
            if (UserHandle.myUserId() != UserHandle.USER_SYSTEM
                    || !UsbAutoBootSettings.setEnabled((Boolean) objValue)) {
                Toast.makeText(getActivity(), R.string.quark_usb_boot_save_error, Toast.LENGTH_LONG).show();
                return false;
            }
            return true;
        }
        if (preference == mSwitchAmbientDisplay) {
            boolean DozeValue = (Boolean) objValue;
            Settings.Secure.putInt(getActivity().getContentResolver(), Settings.Secure.DOZE_ENABLED, DozeValue ? 1 : 0);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            getActivity().onBackPressed();
            return true;
        }
        return false;
    }
}
