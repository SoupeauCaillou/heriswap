package net.damsy.soupeaucaillou.tilematch;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

import net.damsy.soupeaucaillou.tilematch.TilematchJNILib.DumbAndroid.Command;
import net.damsy.soupeaucaillou.tilematch.TilematchJNILib.DumbAndroid.Command.Type;
import android.content.ContentValues;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;
import android.view.View;

import com.openfeint.api.resource.Achievement;
import com.openfeint.api.resource.Leaderboard;
import com.openfeint.api.resource.Score;
import com.openfeint.api.ui.Dashboard;
 
public class TilematchJNILib {
    static {
        System.loadLibrary("tilematch");
    }

    static final String PREF_NAME = "HeriswapPref";
    
    /* Create native game instance */
    public static native long createGame(AssetManager mgr, int openGLESVersion);
    /* Initialize game, reset graphics assets, etc... */
    public static native void initFromRenderThread(long game, int width, int height);
    public static native void initFromGameThread(long game, byte[] state);
    public static native void step(long game);
    public static native void render(long game);
    public static native void pause(long game);
    public static native void handleInputEvent(long game, int event, float x, float y);
    public static native byte[] serialiazeState(long game);
    public static native void initAndReloadTextures(long game);
    
    //-------------------------------------------------------------------------
    // AssetAPI
    //-------------------------------------------------------------------------
    static public  byte[] assetToByteArray(AssetManager mgr, String assetName) {
    	try {
    		InputStream stream = mgr.open(assetName);
	    	byte[] data = new byte[stream.available()];
	    	stream.read(data);
	    	return data;	
    	} catch (Exception exc) { 
    		Log.e("plop", exc.toString());
    		return null;
    	} 
    }

    //-------------------------------------------------------------------------
    // SoundAPI
    //-------------------------------------------------------------------------
    static public int loadSound(AssetManager mgr, String assetPath) {
    	try {
		    return TilematchActivity.soundPool.load(mgr.openFd(assetPath), 1);
    	} catch (Exception exc) {
    		Log.e(TilematchActivity.Tag, "Unable to load sound: " + assetPath);
    		return -1;
    	}
    }
   
    static public void playSound(int soundID, float volume) {
    	if (soundID < 0)
    		return;
    	TilematchActivity.soundPool.play(soundID, 0.25f*volume, 0.25f*volume, 0, 0, 1.0f);
    }
     
    //-------------------------------------------------------------------------
    // StorageAPI
    //-------------------------------------------------------------------------
    static final String SoundEnabledPref = "SoundEnabled";
    static final String GameCountBeforeAds = "GameCountBeforeAds";
    
    static public boolean soundEnable(boolean switchIt) {
    	boolean enabled = TilematchActivity.preferences.getBoolean(SoundEnabledPref, true);
    	if (switchIt) {
    		Editor ed = TilematchActivity.preferences.edit();
    		ed.putBoolean(SoundEnabledPref, !enabled);
    		ed.commit();
    		return !enabled;
    	} else {
    		return enabled;
    	}
    }
    
    static public int getGameCountBeforeNextAd() {
    	return TilematchActivity.preferences.getInt(GameCountBeforeAds, 10);
    }
    
    static public void setGameCountBeforeNextAd(int value) {
    	Editor ed = TilematchActivity.preferences.edit();
    	ed.putInt(GameCountBeforeAds, value);
    	ed.commit();
    }
    
    static public int getSavedGamePointsSum() {
    	SQLiteDatabase db = TilematchActivity.scoreOpenHelper.getReadableDatabase();
    	Cursor cursor = db.rawQuery("select sum(points) from score", null);
    	if (cursor.getCount() > 0) {
    		cursor.moveToFirst();
    		return cursor.getInt(0);
    	} else {
    		return 0;
    	}
    }
	
	static final String[] boards = new String[] {
		"1141887",
		"1149477",
		"1149487",
	};
     
    static public void submitScore(int mode, int difficulty, int points, int level, float time, String name) {
    	SQLiteDatabase db = TilematchActivity.scoreOpenHelper.getWritableDatabase();
    	ContentValues v = new ContentValues();
    	v.put("name", name);
    	v.put("mode", mode);
    	v.put("difficulty", difficulty);
    	v.put("points", points);
    	v.put("time", time);
    	v.put("level", level);
    	db.insert("score", null, v);
    	db.close();

    	if (difficulty == 1) {
	    	Leaderboard l = new Leaderboard(boards[mode - 1]);
	    	Log.i(TilematchActivity.Tag, "leaderboard id: " + boards[mode - 1]);
			final Score s = new Score((long) ((mode != 2) ? points : (time * 1000)), null);
	
			s.submitTo(l, new Score.SubmitToCB() {			
				@Override public void onSuccess(boolean newHighScore) {
					Log.i(TilematchActivity.Tag, "score posting successfull");
				}
	
				@Override public void onFailure(String exceptionMessage) {
					Log.i(TilematchActivity.Tag, "score posting failure : " + exceptionMessage);
				}
		 		
				@Override public void onBlobUploadSuccess() {
				}
				
				@Override public void onBlobUploadFailure(String exceptionMessage) {
				}
			});
    	}
    }
         
    static public int getScores(int mode, int difficulty, int[] points, int[] levels, float[] times, String[] names) {    	 
    	SQLiteDatabase db = TilematchActivity.scoreOpenHelper.getWritableDatabase();
    	Cursor cursor = null;
    	if (mode == 1 || mode == 3) { 
    		cursor = db.query("score", new String[] {"name", "points", "time", "level"}, "mode='" + mode + "' and difficulty='" + difficulty + "'", null, null, null, "points desc");
    	} else {
    		cursor = db.query("score", new String[] {"name", "points", "time", "level"}, "mode='" + mode + "' and difficulty='" + difficulty + "'", null, null, null, "time asc");
    	}
    	int maxResult = Math.min(5, cursor.getCount());
    	cursor.moveToFirst();
    	Log.d(TilematchActivity.Tag, "Found " + maxResult + " result");
    	for (int i=0; i<maxResult; i++) {
    		points[i] = cursor.getInt(cursor.getColumnIndex("points"));
    		levels[i] = cursor.getInt(cursor.getColumnIndex("level"));
    		times[i] = cursor.getFloat(cursor.getColumnIndex("time"));
    		names[i] = cursor.getString(cursor.getColumnIndex("name"));
    		
    		//Log.i("tilematchJ", points[i] + ", " + levels[i] + ", " + times[i] + ", " + names[i] + ".");
    		cursor.moveToNext();
    	}   
    	db.close();
    	return maxResult;
    }
      
    //-------------------------------------------------------------------------
    // SuccessAPI
    //-------------------------------------------------------------------------
    static public void unlockAchievement(int id) {
    	Achievement achv = new Achievement(Integer.toString(id));
    	achv.unlock(new Achievement.UnlockCB() {
			
			@Override
			public void onSuccess(boolean newUnlock) {
				Log.i(TilematchActivity.Tag, "Achievement unlock successful");
			}
		});
    } 
    
    static public void openfeintLeaderboard(int mode) {
    	if (mode >= 1 && mode <= 3) {
    		Dashboard.openLeaderboard(boards[mode - 1]);
    	}
    }
    
    //-------------------------------------------------------------------------
    // NameInputAPI
    //------------------------------------------------------------------------- 
    static public void showPlayerNameUi() { 
    	TilematchActivity.nameReady = false;
    	// show input view
    	TilematchActivity.playerNameInputView.post(new Runnable() {
			public void run() {
				Log.i(TilematchActivity.Tag, "requesting user input visibility");
				TilematchActivity.playerNameInputView.setVisibility(View.VISIBLE);
				TilematchActivity.playerNameInputView.requestFocus();
				TilematchActivity.playerNameInputView.invalidate();
				TilematchActivity.playerNameInputView.forceLayout();
				TilematchActivity.playerNameInputView.bringToFront();
		    	TilematchActivity.nameEdit.setText(TilematchActivity.playerName);	
			}
		});
    	Log.i(TilematchActivity.Tag, "showPlayerNameUI");
    }
    
    static public String queryPlayerName() {
    	if (TilematchActivity.nameReady) {
    		Log.i(TilematchActivity.Tag, "queryPlayerName done");
    		return TilematchActivity.playerName;
    	} else {
    		return null;
    	}
    }

    //-------------------------------------------------------------------------
    // MusicAPI
    //-------------------------------------------------------------------------
    static class DumbAndroid {
    	AudioTrack track;
    	int bufferSize;
    	Thread writeThread;
    	int initialCount;
    	
    	static class Command {
    		static enum Type {
    			Buffer, 
    			Play,
    			Stop
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
    		initialCount = 1 + (int)AudioTrack.getMinBufferSize(rate, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT) / pcmBufferSize(rate);
    		bufferSize = initialCount * pcmBufferSize(rate);
        	writePendings = new LinkedList<Command>();
        	track = new AudioTrack(AudioManager.STREAM_MUSIC, rate, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT, bufferSize, AudioTrack.MODE_STREAM);
    		running = true;
    		writeThread = new Thread(new Runnable() {
				public void run() {
					while (running) {
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
						if (cmd != null) { 
							switch (cmd.type) {
								case Buffer: {
									int result = track.write(cmd.buffer, 0, cmd.bufferSize);
									if (result != cmd.bufferSize) {
										Log.e("tilematchJ", "Error writing data to AudioTrack: " + result);
										checkReturnCode("write,", result);
									} else {
										// Log.e("tilematchJ", "Successful write of " + data.length);
									}
									synchronized (bufferPool) {
										bufferPool.add(cmd.buffer);
									}
									break;
								} 
								case Play: {
				 					int offset = cmd.offset;
									if (cmd.master != null) {
										offset += cmd.master.getPlaybackHeadPosition();
									}
									if (offset != 0) {
										Log.i(TilematchActivity.Tag, "Setting offset: " + offset);
										checkReturnCode("setPosition", track.setPlaybackHeadPosition(offset));
									}
									Log.i(TilematchActivity.Tag, "start track (" + initialCount + ")");
									// track.setStereoVolume(1, 1);
									track.play();
									break;
								}
								case Stop: {
									track.flush();
									track.stop();
									break;
								}
							}
		 				}
					}
				}
    		}, "DumbAndroid");   
    	} 
    	// buffer pool shared accross all instances
    	static List<byte[]> bufferPool = new ArrayList<byte[]>();
    }   
      
    static void checkReturnCode(String ctx, int result) {
    	switch (result) {
    	case AudioTrack.SUCCESS: /*Log.i(TilematchActivity.Tag, ctx + " : success");*/ break;
    	case AudioTrack.ERROR_BAD_VALUE: Log.i(TilematchActivity.Tag, ctx + " : bad value"); break;
    	case AudioTrack.ERROR_INVALID_OPERATION: Log.i(TilematchActivity.Tag, ctx + " : invalid op"); break;
    	}
    }
    static public Object createPlayer(int rate) {
    	return new DumbAndroid(rate);
    }
    
    static public int pcmBufferSize(int sampleRate) {
    	int r = (int) (0.05 * sampleRate * 2); // 100ms 
    	Log.i(TilematchActivity.Tag, "size : " + r);
    	return r;
    }
    static public byte[] allocate(int size) {
    	synchronized (DumbAndroid.bufferPool) {
    		int s = DumbAndroid.bufferPool.size();
			if (s > 0) {
				// Log.i("tilematchJ", "Reuse old buffer (count: " + s + ")");
				return DumbAndroid.bufferPool.remove(s - 1);
			} else {
				// Log.i("tilematchJ", "Create new buffer: " + size);
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
       
    static public byte[] queueMusicData(Object o, byte[] audioData, int size, int sampleRate) {
    	DumbAndroid dumb = (DumbAndroid) o;
//Log.i(TilematchActivity.Tag, "queue data");
    	synchronized (dumb.track) {
    		if (size > dumb.bufferSize) {
	    		// split buffer 
	    		int start = 0;
	    		do {
	    			int end = Math.min(start + dumb.bufferSize, size);
	    			byte[] data = Arrays.copyOfRange(audioData, start, end);
	    			Command cmd = new Command();
	    			cmd.type = Type.Buffer;
	    			cmd.buffer = data;
	    			dumb.writePendings.add(cmd); 
	    			start += (end - start + 1);
	    		} while (start < size);
	    		DumbAndroid.bufferPool.add(audioData);
    		} else {
    			Command cmd = new Command();
    			cmd.type = Type.Buffer;
    			cmd.buffer = audioData;
    			cmd.bufferSize = size;
    			dumb.writePendings.add(cmd); 
    		}
			dumb.track.notify();
		}
    	// Log.i(TilematchActivity.Tag, "b4 allocate");
    	return allocate(size);
    } 
    
    static public void startPlaying(Object o, Object master, int offset) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	dumb.writeThread.start();
    	synchronized (dumb.track) { 
    		Command cmd = new Command();
    		cmd.type = Type.Play;
     		if (master != null) {
    			cmd.master = ((DumbAndroid)master).track;
    		}
    		cmd.offset = offset;
    		dumb.writePendings.add(cmd);
			dumb.track.notify();
    	}
    }  
      
    static public void stopPlayer(Object o) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	synchronized (dumb.track) {
    		dumb.track.stop();
    		// flush queue
    		for (Command cmd: dumb.writePendings) {
    			if (cmd.type == Type.Buffer)
    				DumbAndroid.bufferPool.add(cmd.buffer);
			}
    		dumb.writePendings.clear();
    	}
    }
    
    static public int getPosition(Object o) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	return dumb.track.getPlaybackHeadPosition();
    }
    
    static public void setPosition(Object o, int pos) {

    }
    
    static public void setVolume(Object o, float v) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	checkReturnCode("setVolume", dumb.track.setStereoVolume(v * 0.25f, v *0.25f));
    }
      
    static public boolean isPlaying(Object o) { 
    	DumbAndroid dumb = (DumbAndroid) o; 
    	synchronized (dumb.track) {
    		return !dumb.writePendings.isEmpty() || dumb.track.getPlayState() == AudioTrack.PLAYSTATE_PLAYING;
    	} 
    }
    
    static public void deletePlayer(Object o) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	synchronized (dumb.track) {
			dumb.writePendings.clear();
			dumb.running = false;
			dumb.track.stop();
			dumb.track.flush();
			dumb.track.release();
    	}
    }
    
    //-------------------------------------------------------------------------
    // LocalizeAPI
    //-------------------------------------------------------------------------
    static public String localize(String name) {
    	int id = TilematchActivity.res.getIdentifier(name, "string", "net.damsy.soupeaucaillou.tilematch");
    	if (id == 0) {
    		Log.e(TilematchActivity.Tag, "Cannot find text entry : '" + name + "' for localization");
    		return "LOC" + name + "LOC";
    	}
    	return TilematchActivity.res.getString(id);
    }
}
