/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

    Heriswap is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    Heriswap is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/


package net.damsy.soupeaucaillou.heriswap;

import android.preference.PreferenceActivity;
import android.os.Bundle;
import im.delight.android.languages.Language;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.Context;
import android.app.PendingIntent;
import android.app.AlarmManager;
import android.widget.Toast;
import android.preference.PreferenceManager;

public class HeriswapSettingsActivity extends PreferenceActivity {
    private String previousValue;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.preferences);

        previousValue = getLangValue();
    }
    private String getLangValue() {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
        return prefs.getString("myPreferenceKey", "");
    }

    @Override
    protected void onPause() {
        Language.setFromPreference(this, "myPreferenceKey", true);
        if (!previousValue.equals(getLangValue())) {
            // restart game to handle new language
            Intent mStartActivity = new Intent(getBaseContext(), HeriswapActivity.class);
            int mPendingIntentId = 123456;
            PendingIntent mPendingIntent = PendingIntent.getActivity(getBaseContext(), mPendingIntentId, mStartActivity, PendingIntent.FLAG_CANCEL_CURRENT);
            AlarmManager mgr = (AlarmManager)getBaseContext().getSystemService(Context.ALARM_SERVICE);
            mgr.set(AlarmManager.RTC, System.currentTimeMillis() + 100, mPendingIntent);
            System.exit(0);
        }
        super.onPause();
    }
}
