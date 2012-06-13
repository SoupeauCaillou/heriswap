package net.damsy.soupeaucaillou.heriswap;

import java.util.HashMap;
import java.util.Map;

import com.openfeint.api.OpenFeint;
import com.openfeint.api.OpenFeintDelegate;
import com.openfeint.api.OpenFeintSettings;

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
