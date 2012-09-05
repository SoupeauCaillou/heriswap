package net.damsy.soupeaucaillou.heriswap.api;

import net.damsy.soupeaucaillou.SacJNILib;
import net.damsy.soupeaucaillou.heriswap.HeriswapActivity;
import net.damsy.soupeaucaillou.heriswap.HeriswapSecret;
import android.content.ContentValues;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.swarmconnect.Swarm;
import com.swarmconnect.SwarmLeaderboard;
import com.swarmconnect.SwarmLeaderboardScore;

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
		
		static private float bestLocalTimeForModeAndDifficulty(SQLiteDatabase db, final int mode, final int difficulty) {
			Cursor cursor = null;
			try {
				cursor = db.query("score", new String[] { "time" }, "mode='" + (mode+1) + "' and difficulty='"
				+ difficulty + "'", null, null, null, "time asc");
				if (cursor.getCount() > 0) {
					cursor.moveToFirst();
					return cursor.getFloat(0);
				}
			} catch (Exception exc) {
				Log.e("Sac", "Exception accessing db: ", exc);
			
			} finally {
				cursor.close();
			}
			return 0;
		}
		
		static private int bestLocalScoreForModeAndDifficulty(SQLiteDatabase db, final int mode, final int difficulty) {
			Cursor cursor = null;
			try {
				cursor = db.query("score", new String[] { "points" }, "mode='" + (mode+1) + "' and difficulty='"
						+ difficulty + "'", null, null, null, "points desc");
				if (cursor.getCount() > 0) {
					cursor.moveToFirst();
					return cursor.getInt(0);
				}
			} catch (Exception exc) {
				Log.e("Sac", "Exception accessing db: ", exc);
			
			} finally {
				cursor.close();
			}
			return 0;
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
			
			Log.i("Sac", "Submit score: " + mode + ", diff: " + difficulty + ", pts: " + points + ", time : " + time);

			
			final float bestTime = Math.min(time, bestLocalTimeForModeAndDifficulty(db, mode, difficulty));
			final int bestScore = Math.max(points, bestLocalScoreForModeAndDifficulty(db, mode, difficulty));
			Log.i("Sac", "Best time: "+ bestTime + ", bestScore: " + bestScore);

			db.close();

			if (!Swarm.isInitialized() || !Swarm.isEnabled())
				return;
			if (!Swarm.isLoggedIn())
				return;
			
			Log.i("Sac", "Swarn is initialized go!");
			

			SwarmLeaderboard.GotLeaderboardCB callback = new SwarmLeaderboard.GotLeaderboardCB() {
				@Override
			    public void gotLeaderboard(final SwarmLeaderboard leaderboard) {
			    	if (leaderboard != null) {
			    		Log.i("Sac", "Got leaderboard");
			    		leaderboard.getScoreForUser(Swarm.user, new SwarmLeaderboard.GotScoreCB() {
									
			    			@Override
			    			public void gotScore(SwarmLeaderboardScore arg0) {
			    				Log.i("Sac", "Got bestscore: " + arg0);
			    				boolean submit = true;
			    				if (arg0 != null) {
			    					if ((mode == 0 || mode == 2) && arg0.score >= bestScore) {
			    						submit = false;
			    					} else if (mode == 1 && arg0.score <= bestTime) {
			    						submit = false;
			    					}
			    				}
			    				if (submit) {
			    					Log.i("Sac", "Submit: " + (mode == 1 ? bestTime : bestScore) + " to ldb " + leaderboard.name);
			    					leaderboard.submitScore(mode == 1 ? bestTime : bestScore, null, new SwarmLeaderboard.SubmitScoreCB() {
										@Override
										public void scoreSubmitted(int arg0) {
											Log.i("Sac", "Score submitted result : " + arg0);
										}
										
									});
			    				}
			    			}
			    			});
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
