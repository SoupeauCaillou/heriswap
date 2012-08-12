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
package net.damsy.soupeaucaillou.heriswap;

import java.util.HashMap;
import java.util.Map;

import com.openfeint.api.OpenFeint;
import com.openfeint.api.OpenFeintDelegate;
import com.openfeint.api.OpenFeintSettings;

import com.swarmconnect.Swarm;

import android.app.Application;


public class HeriswapApplication extends Application {
	@Override
	public void onCreate() {
		super.onCreate();

        Map<String, Object> options = new HashMap<String, Object>();
        options.put(OpenFeintSettings.SettingCloudStorageCompressionStrategy, OpenFeintSettings.CloudStorageCompressionStrategyDefault);
        // use the below line to set orientation
        // options.put(OpenFeintSettings.RequestedOrientation, ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        OpenFeintSettings settings = new OpenFeintSettings(
        		HeriswapSecret.OF_gameName, HeriswapSecret.OF_gameKey, HeriswapSecret.OF_gameSecret, HeriswapSecret.OF_gameID, options);
        OpenFeint.initializeWithoutLoggingIn(this, settings, new OpenFeintDelegate() { });

        /*
        Achievement.list(new Achievement.ListCB() {
			@Override public void onSuccess(List<Achievement> _achievements) {
				achievements = _achievements;
			}
		});

        Leaderboard.list(new Leaderboard.ListCB() {
			@Override public void onSuccess(List<Leaderboard> _leaderboards) {
				leaderboards = _leaderboards;
			}
		});*/
	}
}
