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

import java.io.InputStream;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;

import net.damsy.soupeaucaillou.heriswap.HeriswapJNILib.DumbAndroid.Command;
import net.damsy.soupeaucaillou.heriswap.HeriswapJNILib.DumbAndroid.Command.Type;
import android.content.ContentValues;
import android.content.Intent;
import android.content.SharedPreferences;
//import android.content.Intent;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.net.Uri;
//import android.util.Log;
import android.util.Log;
import android.view.View;

import com.chartboost.sdk.ChartBoost;

import com.swarmconnect.Swarm;
import com.swarmconnect.SwarmAchievement;
import com.swarmconnect.SwarmLeaderboard;

public class HeriswapJNILib {
	static {
		System.loadLibrary("heriswap");
	}

	static final String PREF_NAME = "HeriswapPref";

	/* Create native game instance */
	public static native long createGame(int openGLESVersion);

	public static native void destroyGame(long game);

	/* Initialize game, reset graphics assets, etc... */
	public static native void initFromRenderThread(AssetManager mgr, long game, int width,
			int height);

	public static native void initFromGameThread(AssetManager mgr, long game, byte[] state); 

	public static native void uninitFromRenderThread(long game);

	public static native void uninitFromGameThread(long game);

	public static native void step(long game);
 
	public static native void resetTimestep(long game); 

	public static native void render(long game);
 
	public static native void pause(long game);

	public static native void back(long game);

	public static native void invalidateTextures(AssetManager mgr, long game);

	public static native void handleInputEvent(long game, int event, float x,
			float y);

	public static native byte[] serialiazeState(long game);

	public static native void initAndReloadTextures(long game);

	// --- ----------------------------------------------------------------------
	// AdsAPI
	// -------------------------------------------------------------------------
	static public boolean showAd() {
		ChartBoost _cb = ChartBoost.getSharedChartBoost(HeriswapActivity.activity);

		int adProviderSelection = -1;

		boolean gsReady = false; //GSSDK.getSharedInstance().isAdReady();
		boolean cbReady = _cb.hasCachedInterstitial();

		if (gsReady && cbReady) {
			adProviderSelection = (Math.random() > 0.5) ? 0 : 1;
		} else if (gsReady) {
			adProviderSelection = 0;
		} else if (cbReady) {
			adProviderSelection = 1;
		}

		if (adProviderSelection == 0) {
			HeriswapActivity.adHasBeenShown = true;
			/* HeriswapActivity.activity.runOnUiThread(new Runnable() {
				public void run() {
					if (!GSSDK.getSharedInstance().displayAd(
							HeriswapActivity.activity)) {
	    					HeriswapActivity.adHasBeenShown = true;
					}
				}
			});*/
			return false;
		} else if (adProviderSelection == 1) {
			HeriswapActivity.adHasBeenShown = false;
			HeriswapActivity.adWaitingAdDisplay = true;
			_cb.showInterstitial();
			_cb.cacheInterstitial();
			return true;
		} else {
			_cb.cacheInterstitial();
			// Log.w("AD", "No ad ready");
			HeriswapActivity.adHasBeenShown = true;
			return false;
		}
	}

	static public boolean done() {
		return HeriswapActivity.adHasBeenShown;
	}

	// -------------------------------------------------------------------------
	// AssetAPI
	// -------------------------------------------------------------------------
	static public byte[] assetToByteArray(AssetManager mgr, String assetName) {
		try {
			InputStream stream = mgr.open(assetName);
			byte[] data = new byte[stream.available()];
			stream.read(data);
			return data;
		} catch (Exception exc) {
			//NOLOGLog.e(HeriswapActivity.Tag, exc.toString());
			return null;
		}
	}

	// -------------------------------------------------------------------------
	// SoundAPI
	// -------------------------------------------------------------------------
	static public int loadSound(AssetManager mgr, String assetPath) {
		try {
			return HeriswapActivity.soundPool.load(mgr.openFd(assetPath), 1);
		} catch (Exception exc) {
			//NOLOGLog.e(HeriswapActivity.Tag, "Unable to load sound: " + assetPath);
			return -1;
		}
	}

	static public boolean playSound(int soundID, float volume) {
		if (soundID < 0)
			return false;
		return HeriswapActivity.soundPool.play(soundID, 0.5f * volume,
				0.5f * volume, 0, 0, 1.0f) != 0;
	}
	// -------------------------------------------------------------------------
	// StorageAPI
	// -------------------------------------------------------------------------
	static final String SoundEnabledPref = "SoundEnabled";
	static final String GameCountBeforeAds = "GameCountBeforeAds";

	static public boolean soundEnable(boolean switchIt) {
		boolean enabled = HeriswapActivity.preferences.getBoolean(
				SoundEnabledPref, true);
		if (switchIt) {
			Editor ed = HeriswapActivity.preferences.edit();
			ed.putBoolean(SoundEnabledPref, !enabled);
			ed.commit();
			return !enabled;
		} else {
			return enabled;
		}
	}

	static public int getGameCountBeforeNextAd() {
		return HeriswapActivity.preferences.getInt(GameCountBeforeAds, 10);
	}

	static public void setGameCountBeforeNextAd(int value) {
		Editor ed = HeriswapActivity.preferences.edit();
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
		v.put("mode", mode);
		v.put("difficulty", difficulty);
		v.put("points", points);
		v.put("time", time);
		v.put("level", level);
		db.insert("score", null, v);

		db.close();

		// retrieve Leaderboard
		//NOLOGLog.i(HeriswapActivity.Tag, "leaderboard id: " + boards[2 * (mode - 1) + difficulty]);


		SwarmLeaderboard.GotLeaderboardCB callback = new SwarmLeaderboard.GotLeaderboardCB() {
		    public void gotLeaderboard(SwarmLeaderboard leaderboard) {

		    		if (leaderboard != null) {
		    			//que si meilleur score

		    			//leaderboard.getScoreForUser(Swarm.user, truc);
		    			if (true) {
		    				if (mode == 1)
		    					leaderboard.submitScore(points);
		    				else
		    					leaderboard.submitScore(time);
		    			}
		        	}
		        }
		    };
		if (!Swarm.isLoggedIn())
			Swarm.init(HeriswapActivity.activity, HeriswapSecret.Swarm_gameID, HeriswapSecret.Swarm_gameKey);

		SwarmLeaderboard.getLeaderboardById(HeriswapSecret.boardsSwarm[2 * (mode - 1) + difficulty], callback);

		/*
		// Build score object
		final Score s;
		if (mode == 1)
			s = new Score((long) points, null);
		else
			s = new Score((long) (time * 1000),
					(float) ((int) (time * 100) / 100.f) + "s");

//		final Leaderboard l = new Leaderboard(boards[2 * (mode - 1)	+ difficulty]);

		// Callback called by OF on score querying
		final Score.SubmitToCB scCB = new Score.SubmitToCB() {
			@Override
			public void onSuccess(boolean newHighScore) {
				//NOLOGLog.i(HeriswapActivity.Tag, "score posting successfull");

			}

			@Override
			public void onFailure(String exceptionMessage) {
				//NOLOGLog.i(HeriswapActivity.Tag, "score posting failure : "+ exceptionMessage);
			}

			@Override
			public void onBlobUploadSuccess() {
			}

			@Override
			public void onBlobUploadFailure(String exceptionMessage) {
			}
		};

		User user = OpenFeint.getCurrentUser();

		if (user == null) {
			return;
		}

		// Retrieve user best score
		l.getUserScore(OpenFeint.getCurrentUser(),
				new Leaderboard.GetUserScoreCB() {
					@Override
					public void onSuccess(Score best) {
						boolean send = false;
						if (best == null) {
							send = true;
						} else if (mode == 1) {
							send = (best.score < s.score);
						} else {
							send = (best.score > s.score);
						}
						if (send) {
							s.submitTo(l, scCB);
						} else {
							//NOLOGLog.w(HeriswapActivity.Tag,"Inferior score not submitted to OF");
						}
					}

					@Override
					public void onFailure(String exceptionMessage) {
						super.onFailure(exceptionMessage);
						s.submitTo(l, scCB);
					}
				});*/
	}

	static public int getScores(int mode, int difficulty, int[] points,
			int[] levels, float[] times, String[] names) {
		SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
				.getWritableDatabase();
		Cursor cursor = null;
		if (mode == 1) {
			cursor = db.query("score", new String[] { "name", "points", "time",
					"level" }, "mode='" + mode + "' and difficulty='"
					+ difficulty + "'", null, null, null, "points desc");
		} else {
			cursor = db.query("score", new String[] { "name", "points", "time",
					"level" }, "mode='" + mode + "' and difficulty='"
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

	// -------------------------------------------------------------------------
	// SuccessAPI
	// -------------------------------------------------------------------------
	static public void unlockAchievement(int idS) {
		//if (!OpenFeint.isNetworkConnected()) {
		if (!Swarm.isLoggedIn()) {
			return;
		}
		final int id = idS;

		SwarmAchievement.GotAchievementsMapCB callback = new SwarmAchievement.GotAchievementsMapCB() {
			@Override
			public void gotMap(Map<Integer, SwarmAchievement> achievements) {
		        SwarmAchievement achievement = achievements.get(id);
		        // No need to unlock more than once...
		        if (achievement != null && achievement.unlocked == false) {
		            achievement.unlock();
		        }
			}
		};
		SwarmAchievement.getAchievementsMap(callback);

		//final SwarmAchievement ach = new SwarmAchievement();
		//SwarmAchievement.GotAchievementsListCB loadC = new SwarmAchievement.GotAchievementsListCB() {
		//}
		/*
		final Achievement achv = new Achievement(Integer.toString(id));
		Achievement.LoadCB loadCb = new Achievement.LoadCB() {
			@Override
			public void onSuccess() {
				if (!achv.isUnlocked) {
					achv.unlock(new Achievement.UnlockCB() {

						@Override
						public void onSuccess(boolean newUnlock) {
							//NOLOGLog.i(HeriswapActivity.Tag,"Achievement unlock successful");
						}
					});
				}
			}

			@Override
			public void onFailure(String exceptionMessage) {
				super.onFailure(exceptionMessage);
				achv.unlock(new Achievement.UnlockCB() {

					@Override
					public void onSuccess(boolean newUnlock) {
						//NOLOGLog.i(HeriswapActivity.Tag,"Achievement unlock successful");
					}
				});
			}

		};

		achv.load(loadCb);*/
	}


	static public void openLeaderboard(int mode, int difficulty) {
		Log.i(HeriswapActivity.Tag, "openleaderbord: " + mode + "," + difficulty);
		if (mode >= 1 && mode <= 2 && difficulty >= 0 && difficulty <= 2) {
			//Dashboard.openLeaderboard();

			SwarmLeaderboard.GotLeaderboardCB callback = new SwarmLeaderboard.GotLeaderboardCB() {
			    public void gotLeaderboard(SwarmLeaderboard leaderboard) {
 
			    	if (leaderboard != null) {
			    		leaderboard.showLeaderboard();
			        }
			    }
			};

			SwarmLeaderboard.getLeaderboardById(HeriswapSecret.boardsSwarm[3 * (mode - 1) + difficulty], callback);
		}
	}

	static public void openDashboard() {
		//Dashboard.open();
		Swarm.showDashboard();
	}

	// -------------------------------------------------------------------------
	// NameInputAPI
	// -------------------------------------------------------------------------
	static public void showPlayerNameUi() {
		HeriswapActivity.nameReady = false;
		// show input view
		HeriswapActivity.playerNameInputView.post(new Runnable() {
			public void run() {
				//NOLOGLog.i(HeriswapActivity.Tag, "requesting user input visibility");

				// ici
				SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
						.getReadableDatabase();
				Cursor cursor = db
						.rawQuery(
								"select distinct name from score order by rowid desc limit 4",
								null);
				try {
					int count = cursor.getCount();
					if (count > 0) {
						HeriswapActivity.playerNameInputView.findViewById(
								R.id.reuse).setVisibility(View.VISIBLE);
					} else {
						HeriswapActivity.playerNameInputView.findViewById(
								R.id.reuse).setVisibility(View.GONE);
					}
					int i = 0;
					if (cursor.moveToFirst()) {
						do {
							String n = cursor.getString(0);
							//NOLOGLog.i(HeriswapActivity.Tag, "nsssssssssom : " + n);

							if (!n.equals("rzehtrtyBg")) {
								//NOLOGLog.i(HeriswapActivity.Tag, "nom : " + n);
								HeriswapActivity.oldName[i].setText(n);
								HeriswapActivity.oldName[i]
										.setVisibility(View.VISIBLE);
								i++;
							}
						} while (i < 3 && cursor.moveToNext());
					}
					for (; i < 3; i++) {
						HeriswapActivity.oldName[i].setVisibility(View.GONE);
					}
				} finally {
					cursor.close();
				}

				HeriswapActivity.playerNameInputView
						.setVisibility(View.VISIBLE);
				HeriswapActivity.playerNameInputView.requestFocus();
				HeriswapActivity.playerNameInputView.invalidate();
				HeriswapActivity.playerNameInputView.forceLayout();
				HeriswapActivity.playerNameInputView.bringToFront();
				HeriswapActivity.nameEdit.setText("");
			}
		});
		//NOLOGLog.i(HeriswapActivity.Tag, "showPlayerNameUI");
	}

	static public String queryPlayerName() {
		if (HeriswapActivity.nameReady) {
			//NOLOGLog.i(HeriswapActivity.Tag, "queryPlayerName done");
			return HeriswapActivity.playerName;
		} else {
			return null;
		}
	}

	// -------------------------------------------------------------------------
	// CommunicationAPI
	// -------------------------------------------------------------------------
	static public boolean swarmEnabled() {
		return Swarm.isLoggedIn();
	}

	static public void swarmRegistering() {
		Swarm.init(HeriswapActivity.activity, HeriswapSecret.Swarm_gameID, HeriswapSecret.Swarm_gameKey);
	}

	static public void shareFacebook() {
		//Intent sharingIntent = new Intent(Intent.ACTION_SEND);
		//sharingIntent.setType("plain/text");
		//sharingIntent.putExtra(android.content.Intent.EXTRA_TEXT, "This is the text that will be shared.");
		//startActivity(Intent.createChooser(sharingIntent,"Share using"));

	}

	static public void shareTwitter() {
	//	String message = "Text I wan't to share.";
		/*Intent share = new Intent(Intent.ACTION_SEND);
		share.setType("text/plain");
		share.putExtra(Intent.EXTRA_TEXT, message);

		startActivity(Intent.createChooser(share, "Title of the dialog the system will open"));*/
	}
	
	static public boolean mustShowRateDialog() {
		SharedPreferences prefs = HeriswapActivity.activity.getSharedPreferences("apprater", 0);
		if (prefs.getBoolean("dontshowagain", false))
			return false;
		if (prefs.getLong("launch_count", 0) < 10)
			return false;
		
		SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
				.getReadableDatabase();
		Cursor cursor = db.rawQuery("select count (*) from score", null);
		try {
			if (cursor.getCount() > 0) {
				cursor.moveToFirst();
				if (cursor.getInt(0) >= 10)
					return true;
			}
		} catch (Exception exc) {

		} finally {
			cursor.close();
		}
		return false;
	}
	
	private final static String APP_PNAME = "net.damsy.soupeaucaillou.heriswap";
	static public void rateItNow() {
		HeriswapActivity.activity.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=" + APP_PNAME)));
		rateItNever();
	}
	
	static public void rateItLater() {
		SharedPreferences prefs = HeriswapActivity.activity.getSharedPreferences("apprater", 0);
		SharedPreferences.Editor editor = prefs.edit();
		editor.putLong("launch_count", 0);
		editor.commit();
	}

	static public void rateItNever() {
		SharedPreferences prefs = HeriswapActivity.activity.getSharedPreferences("apprater", 0);
		SharedPreferences.Editor editor = prefs.edit();
		editor.putBoolean("dontshowagain", true);
		editor.commit();
	}

	// -------------------------------------------------------------------------
	// MusicAPI
	// -------------------------------------------------------------------------
	static class DumbAndroid {
		AudioTrack track;
		int bufferSize;
		Thread writeThread;
		int initialCount;
		boolean playing;
		Object destroyMutex = new Object();

		static class Command {
			static enum Type {
				Buffer, Play, Stop
			};

			Type type;
			byte[] buffer;
			int bufferSize;
			AudioTrack master;
			int offset;
		}

		Queue<Command> writePendings;
		boolean running;

		DumbAndroid(int rate) {
			playing = false;
			// at least 0.5 (1 sec => too much memory pressure on AudioFlinger
			// 1Mo mem pool)
			initialCount = Math.max(
					(int) (2 * 0.5 * rate) / pcmBufferSize(rate),
					1
							+ (int) AudioTrack.getMinBufferSize(rate,
									AudioFormat.CHANNEL_OUT_MONO,
									AudioFormat.ENCODING_PCM_16BIT)
							/ pcmBufferSize(rate));
			bufferSize = initialCount * pcmBufferSize(rate);
			writePendings = new LinkedList<Command>();

			synchronized (DumbAndroid.audioTrackPool) {
				if (DumbAndroid.audioTrackPool.size() > 0) {
					track = DumbAndroid.audioTrackPool.remove(0);
					//NOLOGLog.i(HeriswapActivity.Tag, "Reuse audiotrack");
				} else {
					//NOLOGLog.i(HeriswapActivity.Tag, "Create audiotrack");
					track = new AudioTrack(AudioManager.STREAM_MUSIC, rate,
							AudioFormat.CHANNEL_OUT_MONO,
							AudioFormat.ENCODING_PCM_16BIT, bufferSize,
							AudioTrack.MODE_STREAM);
				}
			}

			int state = track.getState();
			if (state != AudioTrack.STATE_INITIALIZED) {
				//NOLOGLog.e(HeriswapActivity.Tag, "Failed to create AudioTrack");
				track.release();
				track = null;
				return;
			}

			running = true;
			writeThread = new Thread(new Runnable() {
				public void run() {
					while (running) {
						// Log.w(TilematchActivity.Tag, "queue size: " +
						// writePendings.size());
						Command cmd = null;
						synchronized (track) {
							if (!writePendings.isEmpty()) {
								cmd = writePendings.poll();
							} else {
								try {
									track.wait(10000);
								} catch (InterruptedException exc) {

								}
								cmd = writePendings.poll();
							}
						}

							if (cmd != null && running) {
								switch (cmd.type) {
								case Buffer: {
									if (playing && running) {
										int result = track.write(cmd.buffer, 0,
												cmd.bufferSize);
										if (result != cmd.bufferSize) {
											//NOLOGLog.e(HeriswapActivity.Tag,"Error writing data to AudioTrack("+ track.toString()+ "): "+ result+ ". Is track playing ? "+ track.getPlayState());
											checkReturnCode("write,", result);
											if (result == 0)
												track.stop();
										} else {
											// Log.e(HeriswapActivity.Tag,
											// "Successful write of " +
											// data.length);
										}
									}
									synchronized (bufferPool) {
										bufferPool.add(cmd.buffer);
									}
									break;
								}
								case Play: {
									int offset = cmd.offset;
									if (cmd.master != null) {
										offset += cmd.master
												.getPlaybackHeadPosition();
									}
									if (offset != 0) {
										//NOLOGLog.i(HeriswapActivity.Tag,"Setting offset: " + offset);
										checkReturnCode(
												"setPosition",
												track.setPlaybackHeadPosition(offset));
									}
									//NOLOGLog.i(HeriswapActivity.Tag, "start track ("+ initialCount + ")");
									// track.setStereoVolume(1, 1);
									track.play();
									break;
								}
								case Stop: {
									//track.flush();
									track.stop();
									playing = false;
									break;
								}
							}
						}
					}
					//NOLOGLog.i(HeriswapActivity.Tag, "Effective delete of track: "+ track.toString());
					synchronized (DumbAndroid.bufferPool) {
						for (Command c : writePendings) {
							if (c.type == Type.Buffer) {
								DumbAndroid.bufferPool.add(c.buffer);
							}
						}
					}
					writePendings.clear();
					track.stop();

					track.flush();
					synchronized (DumbAndroid.audioTrackPool) {
						DumbAndroid.audioTrackPool.add(track);
					}
					writePendings.clear();
					// track.release();
				}
			}, "DumbAndroid");
		}

		// buffer pool shared accross all instances
		static List<byte[]> bufferPool = new ArrayList<byte[]>();
		static List<AudioTrack> audioTrackPool = new ArrayList<AudioTrack>(10);
	}

	static void checkReturnCode(String ctx, int result) {
		switch (result) {
		case AudioTrack.SUCCESS: /*
								 * Log.i(TilematchActivity.Tag, ctx +
								 * " : success");
								 */
			break;
		case AudioTrack.ERROR_BAD_VALUE:
			//NOLOGLog.i(HeriswapActivity.Tag, ctx + " : bad value");
			break;
		case AudioTrack.ERROR_INVALID_OPERATION:
			//NOLOGLog.i(HeriswapActivity.Tag, ctx + " : invalid op");
			break;
		}
	}

	static public Object createPlayer(int rate) {
		DumbAndroid result = new DumbAndroid(rate);
		if (result.track == null)
			return null;
		else
			return result;
	}

	static public int pcmBufferSize(int sampleRate) {
		int r = (int) (0.1 * sampleRate * 2); // 100ms
		// Log.i(TilematchActivity.Tag, "size : " + r);
		return r;
	}

	static public byte[] allocate(int size) {
		synchronized (DumbAndroid.bufferPool) {
			int s = DumbAndroid.bufferPool.size();
			if (s > 0) {
				// Log.i(HeriswapActivity.Tag, "Reuse old buffer (count: " + s +
				// ")");
				return DumbAndroid.bufferPool.remove(s - 1);
			} else {
				// Log.i(HeriswapActivity.Tag, "Create new buffer: " + size);
				// assert(size <= dumb.track.getSampleRate() * 2);
				return new byte[size];
			}
		}
	}

	static public void deallocate(byte[] b) {
		synchronized (DumbAndroid.bufferPool) {
			DumbAndroid.bufferPool.add(b);
		}
	}

	static public int initialPacketCount(Object o) {
		DumbAndroid dumb = (DumbAndroid) o;
		return dumb.initialCount;
	}

	static public byte[] queueMusicData(Object o, byte[] audioData, int size,
			int sampleRate) {
		DumbAndroid dumb = (DumbAndroid) o;
		// Log.i(TilematchActivity.Tag, "queue data");
		synchronized (dumb.track) {
			/*
			 * if (size > dumb.bufferSize) { // split buffer int start = 0; do {
			 * int end = Math.min(start + dumb.bufferSize, size); byte[] data =
			 * Arrays.copyOfRange(audioData, start, end); Command cmd = new
			 * Command(); cmd.type = Type.Buffer; cmd.buffer = data;
			 * dumb.writePendings.add(cmd); start += (end - start + 1); } while
			 * (start < size); synchronized (DumbAndroid.bufferPool) {
			 * DumbAndroid.bufferPool.add(audioData); } } else
			 */{
				Command cmd = new Command();
				cmd.type = Type.Buffer;
				cmd.buffer = audioData;
				cmd.bufferSize = size;
				dumb.writePendings.add(cmd);
			}
			dumb.track.notify();
		}
		return null;
	}

	static public void startPlaying(Object o, Object master, int offset) {
		DumbAndroid dumb = (DumbAndroid) o;
		dumb.writeThread.start();
		synchronized (dumb.track) {
			dumb.playing = true;
			Command cmd = new Command();
			cmd.type = Type.Play;
			if (master != null) {
				cmd.master = ((DumbAndroid) master).track;
			}
			cmd.offset = offset;
			dumb.writePendings.add(cmd);
			dumb.track.notify();

			//NOLOGLog.i(HeriswapActivity.Tag, "BUFFER POOL size: "+ DumbAndroid.bufferPool.size());
		}
	}

	static public void stopPlayer(Object o) {
		DumbAndroid dumb = (DumbAndroid) o;
		synchronized (dumb.track) {
			//NOLOGLog.i(HeriswapActivity.Tag, "Stop track: " + dumb.track.toString());
			synchronized (dumb.destroyMutex) {
				dumb.track.stop();
				// flush queue
				for (Command cmd : dumb.writePendings) {
					if (cmd.type == Type.Buffer)
						DumbAndroid.bufferPool.add(cmd.buffer);
				}
				dumb.writePendings.clear();
			}
			dumb.track.notify();
		}
	}

	static public int getPosition(Object o) {
		DumbAndroid dumb = (DumbAndroid) o;
		if (dumb.track.getState() != AudioTrack.STATE_INITIALIZED
				|| dumb.track.getPlayState() != AudioTrack.PLAYSTATE_PLAYING)
			return 0;
		return dumb.track.getPlaybackHeadPosition();
	}

	static public void setPosition(Object o, int pos) {

	}

	static public void setVolume(Object o, float v) {
		DumbAndroid dumb = (DumbAndroid) o;
		//Log.w(HeriswapActivity.Tag, " set volume : " + dumb.toString() + " => " + v);
		checkReturnCode("setVolume",
				dumb.track.setStereoVolume(v, v));
	}

	static public boolean isPlaying(Object o) {
		DumbAndroid dumb = (DumbAndroid) o;
		synchronized (dumb.track) {
			return !dumb.writePendings.isEmpty()
					|| dumb.track.getPlayState() == AudioTrack.PLAYSTATE_PLAYING
					|| dumb.playing;
		}
	}

	static public void deletePlayer(Object o) {
		DumbAndroid dumb = (DumbAndroid) o;
		synchronized (dumb.track) {
			dumb.running = false;
			dumb.track.stop();
			dumb.track.notify();
		}

		//NOLOGLog.i(HeriswapActivity.Tag,"Delete (delayed) track: " + dumb.track.toString());
	}

	// -------------------------------------------------------------------------
	// LocalizeAPI
	// -------------------------------------------------------------------------
	static public String localize(String name) {
		int id = HeriswapActivity.res.getIdentifier(name, "string",
				"net.damsy.soupeaucaillou.heriswap");
		if (id == 0) {
			//NOLOGLog.e(HeriswapActivity.Tag, "Cannot find text entry : '" + name+ "' for localization");
			return "LOC" + name + "LOC";
		}
		return HeriswapActivity.res.getString(id);
	}

	// -------------------------------------------------------------------------
	// ExitAPI
	// -------------------------------------------------------------------------
	static public void exitGame() {
		System.exit(0);
	}
}
