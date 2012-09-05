package net.damsy.soupeaucaillou.heriswap.api;

import net.damsy.soupeaucaillou.SacJNILib;
import net.damsy.soupeaucaillou.heriswap.HeriswapActivity;
import net.damsy.soupeaucaillou.heriswap.HeriswapSecret;
import android.content.ContentValues;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import com.swarmconnect.Swarm;
import com.swarmconnect.SwarmLeaderboard;

public class StorageAPI {
	// -------------------------------------------------------------------------
		// StorageAPI
		// -------------------------------------------------------------------------
		static final String SoundEnabledPref = "SoundEnabled";
		static final String GameCountBeforeAds = "GameCountBeforeAds";

		static public boolean soundEnable(boolean switchIt) {
		 	SharedPreferences preferences = SacJNILib.activity.getSharedPreferences(HeriswapActivity.HERISWAP_SHARED_PREF, 0); 
			boolean enabled = preferences.getBoolean(
					SoundEnabledPref, true);
			if (switchIt) {
				Editor ed = preferences.edit();
				ed.putBoolean(SoundEnabledPref, !enabled);
				ed.commit();
				return !enabled;
			} else {
				return enabled;
			}
		}
  
		static public int getGameCountBeforeNextAd() {
			SharedPreferences preferences = SacJNILib.activity.getSharedPreferences(HeriswapActivity.HERISWAP_SHARED_PREF, 0);
			return preferences.getInt(GameCountBeforeAds, 10);
		}

		static public void setGameCountBeforeNextAd(int value) {
			SharedPreferences preferences = SacJNILib.activity.getSharedPreferences(HeriswapActivity.HERISWAP_SHARED_PREF, 0);
			Editor ed = preferences.edit();
			ed.putInt(GameCountBeforeAds, value);
			ed.commit();
		}

		static public int getSavedGamePointsSum() {
			SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
					.getReadableDatabase();
			Cursor cursor = db.rawQuery("select sum(points) from score", null);
			try {
				if (cursor.getCount() > 0) {
					cursor.moveToFirst();
					return cursor.getInt(0);
				} else {
					return 0;
				}
			} finally {
				cursor.close();
			}
		}

		static public int getModePlayedCount() {
			SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
					.getReadableDatabase();
			Cursor cursor = db.rawQuery(
					"select distinct difficulty,mode from score", null);
			int count = cursor.getCount();
			cursor.close();
			return count;
		}

		static public void submitScore(final int mode, final int difficulty, final int points,
				final int level, final float time, final String name) {
			SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
					.getWritableDatabase();
			ContentValues v = new ContentValues();
			v.put("name", name);
			v.put("mode", mode + 1);
			v.put("difficulty", difficulty);
			v.put("points", points);
			v.put("time", time);
			v.put("level", level);
			db.insert("score", null, v);

			db.close();

			// retrieve Leaderboard
			//NOLOGLog.i(HeriswapActivity.Tag, "leaderboard id: " + boards[2 * mode + difficulty]);

			if (!Swarm.isInitialized() || !Swarm.isEnabled())
				return;

			SwarmLeaderboard.GotLeaderboardCB callback = new SwarmLeaderboard.GotLeaderboardCB() {
			    public void gotLeaderboard(SwarmLeaderboard leaderboard) {

			    		if (leaderboard != null) {
			    			//que si meilleur score

			    			//leaderboard.getScoreForUser(Swarm.user, truc);
			    			if (true) {
			    				if (mode == 0 || mode == 2)
			    					leaderboard.submitScore(points);
			    				else
			    					leaderboard.submitScore(time);
			    			}
			        	}
			        }
			    };
			//if (!Swarm.isLoggedIn())
			//Swarm.init(HeriswapActivity.activity, HeriswapSecret.Swarm_gameID, HeriswapSecret.Swarm_gameKey);

			SwarmLeaderboard.getLeaderboardById(HeriswapSecret.boardsSwarm[3 * mode + difficulty], callback);
		}

		static public int getScores(int mode, int difficulty, int[] points,
				int[] levels, float[] times, String[] names) {
			SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
					.getWritableDatabase();
			Cursor cursor = null;
			if (mode == 0 || mode == 2) {
				cursor = db.query("score", new String[] { "name", "points", "time",
						"level" }, "mode='" + (mode+1) + "' and difficulty='"
						+ difficulty + "'", null, null, null, "points desc");
			} else {
				cursor = db.query("score", new String[] { "name", "points", "time",
						"level" }, "mode='" + (mode+1) + "' and difficulty='"
						+ difficulty + "'", null, null, null, "time asc");
			}
			int maxResult = Math.min(5, cursor.getCount());
			cursor.moveToFirst();
			//NOLOGLog.d(HeriswapActivity.Tag, "Found " + maxResult + " result");
			for (int i = 0; i < maxResult; i++) {
				points[i] = cursor.getInt(cursor.getColumnIndex("points"));
				levels[i] = cursor.getInt(cursor.getColumnIndex("level"));
				times[i] = cursor.getFloat(cursor.getColumnIndex("time"));
				names[i] = cursor.getString(cursor.getColumnIndex("name"));

				//NOLOGLog.i(HeriswapActivity.Tag, points[i] + ", " + levels[i] + ", "+ times[i] + ", " + names[i] + ".");
				cursor.moveToNext();
			}
			cursor.close();
			db.close();
			return maxResult;
		}
}
