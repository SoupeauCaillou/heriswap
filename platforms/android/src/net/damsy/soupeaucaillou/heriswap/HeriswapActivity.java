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

import net.damsy.soupeaucaillou.SacActivity;
import net.damsy.soupeaucaillou.api.AssetAPI;
import net.damsy.soupeaucaillou.api.CommunicationAPI;
import net.damsy.soupeaucaillou.api.LocalizeAPI;
import net.damsy.soupeaucaillou.api.MusicAPI;
import net.damsy.soupeaucaillou.api.OpenURLAPI;
import net.damsy.soupeaucaillou.api.SoundAPI;
import net.damsy.soupeaucaillou.api.StorageAPI;
import net.damsy.soupeaucaillou.api.StringInputAPI;
import net.damsy.soupeaucaillou.api.VibrateAPI;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;

import im.delight.android.languages.LanguageList;
import java.util.Arrays;
import android.content.res.Configuration;
import im.delight.android.languages.Language;

public class HeriswapActivity extends SacActivity {
	static {
        System.loadLibrary("sac");
    }

    public int getLayoutId() {
        return R.layout.main;
    }

	public int getParentViewId() {
        return R.id.parent_frame;
    }

	@Override
    protected void onCreate(Bundle savedInstanceState) {
		SacActivity.LogI("-> onCreate [" + savedInstanceState);
        super.onCreate(savedInstanceState);
        Language.setFromPreference(this, "myPreferenceKey");

        SacActivity.LogE("LanguageList.getHumanReadable() = "+Arrays.toString(LanguageList.getHumanReadable()));
        SacActivity.LogE("LanguageList.getMachineReadable() = "+Arrays.toString(LanguageList.getMachineReadable()));
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        Language.setFromPreference(this, "myPreferenceKey");
    }

	@Override
	public void initRequiredAPI() {
		AssetAPI.Instance().init(this, getAssets());
		LocalizeAPI.Instance().init(this.getResources(), this.getPackageName());
		StorageAPI.Instance().init(this);
		MusicAPI.Instance();
        OpenURLAPI.Instance().init(this);
		SoundAPI.Instance().init(getAssets());
		CommunicationAPI.Instance().init(this, getPreferences(MODE_PRIVATE));
		StringInputAPI.Instance().init(
			(Button)findViewById(R.id.name_save),
			(EditText)findViewById(R.id.player_name_input),
			(ListView)findViewById(R.id.names_list),
			(View)findViewById(R.id.enter_name),
			R.layout.namelist,
			(InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)
				);
		VibrateAPI.Instance().init((Vibrator) getSystemService(Context.VIBRATOR_SERVICE));
	}
}
