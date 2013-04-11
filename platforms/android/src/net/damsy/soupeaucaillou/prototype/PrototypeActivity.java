/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

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
package net.damsy.soupeaucaillou.prototype;

import net.damsy.soupeaucaillou.SacActivity;
import net.damsy.soupeaucaillou.api.AssetAPI;
import net.damsy.soupeaucaillou.api.LocalizeAPI;
import android.os.Bundle;

public class PrototypeActivity extends SacActivity {
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
        
        // Vibrator vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
    }
	
	@Override
	public void initRequiredAPI() {
		AssetAPI.Instance().init(getAssets());
		LocalizeAPI.Instance().init(this.getResources(), this.getPackageName());
	}
}
