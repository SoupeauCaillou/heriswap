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
 
import java.util.Arrays;

import net.damsy.soupeaucaillou.SacActivity;
import net.damsy.soupeaucaillou.api.AssetAPI;
import net.damsy.soupeaucaillou.api.CommunicationAPI;
import net.damsy.soupeaucaillou.api.ExitAPI;
import net.damsy.soupeaucaillou.api.GameCenterAPI;
import net.damsy.soupeaucaillou.api.LocalizeAPI;
import net.damsy.soupeaucaillou.api.MusicAPI;
import net.damsy.soupeaucaillou.api.SoundAPI;
import net.damsy.soupeaucaillou.api.StorageAPI;
import net.damsy.soupeaucaillou.api.StringInputAPI;
import net.damsy.soupeaucaillou.api.VibrateAPI;
import net.damsy.soupeaucaillou.googleplaygameservices.SacGooglePlayGameServicesPlugin;
import android.content.Context;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;

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
    }

	@Override
	public void initRequiredAPI() {
		AssetAPI.Instance().init(this, getAssets());
		LocalizeAPI.Instance().init(this.getResources(), this.getPackageName());
		StorageAPI.Instance().init(this);
		ExitAPI.Instance();
		MusicAPI.Instance();
		SoundAPI.Instance();
		CommunicationAPI.Instance().init(this, getPreferences(MODE_PRIVATE));
		StringInputAPI.Instance().init(
			(Button)findViewById(R.id.name_save),
			(EditText)findViewById(R.id.player_name_input),
			(View)findViewById(R.id.enter_name),
			(InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE)
				);
		VibrateAPI.Instance().init((Vibrator) getSystemService(Context.VIBRATOR_SERVICE));
        
        SacGooglePlayGameServicesPlugin sgpgsp = new SacGooglePlayGameServicesPlugin();
        sgpgsp.init(this, sgpgsp.new GooglePlayGameServicesParams(false, 
                Arrays.asList(new String[] { 
            		"CgkIwJzk2eQSEAIQDA",
            		"CgkIwJzk2eQSEAIQDQ",
            		"CgkIwJzk2eQSEAIQAw",
            		"CgkIwJzk2eQSEAIQCg",
            		"CgkIwJzk2eQSEAIQCw",
            		"CgkIwJzk2eQSEAIQAQ",
            		"CgkIwJzk2eQSEAIQDg",
            		"CgkIwJzk2eQSEAIQBA",
            		"CgkIwJzk2eQSEAIQAg",
            		"CgkIwJzk2eQSEAIQEA",
            		"CgkIwJzk2eQSEAIQCA",
            		"CgkIwJzk2eQSEAIQDw",
            		"CgkIwJzk2eQSEAIQBw",
            		"CgkIwJzk2eQSEAIQBg",
            		"CgkIwJzk2eQSEAIQBQ",
            		"CgkIwJzk2eQSEAIQCQ",
            		"CgkIwJzk2eQSEAIQFA",
            		"CgkIwJzk2eQSEAIQEQ",
            		"CgkIwJzk2eQSEAIQEg",
            		"CgkIwJzk2eQSEAIQEw",
                }), 
                Arrays.asList(new String[] {
                	"CgkIwJzk2eQSEAIQFg",
                	"CgkIwJzk2eQSEAIQFQ",
                    "CgkIwJzk2eQSEAIQGA",
                    
            		"CgkIwJzk2eQSEAIQHQ",
            		"CgkIwJzk2eQSEAIQHg",
            		"CgkIwJzk2eQSEAIQHA",

                    "CgkIwJzk2eQSEAIQGw",
                    "CgkIwJzk2eQSEAIQGQ",
                    "CgkIwJzk2eQSEAIQGg",
                })
            )
        );
        GameCenterAPI.Instance().init(this, sgpgsp);
	}
}
