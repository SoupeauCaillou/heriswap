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
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnSeekCompleteListener;
import android.util.Log;
import android.view.View;

import com.openfeint.api.resource.Leaderboard;
import com.openfeint.api.resource.Score;
import com.openfeint.api.ui.Dashboard;
 
public class TilematchJNILib {
    static {
        System.loadLibrary("tilematch");
    }

    /* Create native game instance */
    public static native long createGame(AssetManager mgr, int openGLESVersion);
    /* Initialize game, reset graphics assets, etc... */
    public static native void initFromRenderThread(long game, int width, int height, byte[] state);
    public static native void initFromGameThread(long game);
    public static native void step(long game);
    public static native void render(long game);
    public static native void pause(long game);
    public static native void handleInputEvent(long game, int event, float x, float y);
    public static native byte[] serialiazeState(long game);
    public static native void initAndReloadTextures(long game);
   
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
     
    static public int loadSound(AssetManager mgr, String assetPath, boolean music) {
    	try {
	    	if (music) {
	    		AssetFileDescriptor fd = mgr.openFd(assetPath);
	    		// find available MediaPlayer
	    		for(MediaPlayer p : TilematchActivity.availablePlayers) {
	    			p.reset();
	    			p.setAudioStreamType(AudioManager.STREAM_MUSIC);
	    			p.setDataSource(fd.getFileDescriptor(), fd.getStartOffset(), fd.getLength());
	    			p.setVolume(1.0f, 1.0f);
	    			p.prepare();
	    			TilematchActivity.availablePlayers.remove(p);
	    			for (int i=0; i<8; i++) {
	    				if (TilematchActivity.activePlayers[i] == null) {
	    					TilematchActivity.activePlayers[i] = p;
	    					return i;
	    				}
	    			}
	    			throw new RuntimeException("Mouarf buggy code");
	    		}
	    		Log.w("tilematchJava", "No available MediaPlayer for playing: " + assetPath);
	    		return -1;
	    	} else {
		    	return TilematchActivity.soundPool.load(mgr.openFd(assetPath), 1);
	    	}
    	} catch (Exception exc) {
    		Log.e("tilematchJava", "Unable to load sound: " + assetPath);
    		return -1;
    	}
    }
 
    static public int playSound(int soundID, boolean music, int soundRef, int offsetms) {
    	if (soundID < 0)
    		return soundID;
    	if (music) {
    		Log.i("tilematchJava", "play: " + soundID + "(ref: " + soundRef + ", offset: " + offsetms);
    		TilematchActivity.activePlayers[soundID].start();
    		
    		if (soundRef >= 0) {
    			int pos = TilematchActivity.activePlayers[soundRef].getCurrentPosition() + offsetms;
    			MediaPlayer m = TilematchActivity.activePlayers[soundID];
    			final Object o = new Object();
    			m.setOnSeekCompleteListener(new OnSeekCompleteListener() {
					public void onSeekComplete(MediaPlayer mp) {
						synchronized (o) {
							o.notifyAll();
						}
					}
				});
    			TilematchActivity.activePlayers[soundID].seekTo(pos);
    			synchronized (o) {
    				try {
    					o.wait();
    				} catch (InterruptedException exc) {
    					// oh.. well
    				}
				}
    			Log.i("tilematchJava", "seekpos: " + pos);
    		}
    		
    		return soundID;
    	} else { 
    		return TilematchActivity.soundPool.play(soundID, 1.0f, 1.0f, 0, 0, 1.0f);
    	}
    }
    
    static public void stopSound(int soundID, boolean music) {
    	if (soundID < 0)
    		return ;
    	if (music) {
    		Log.i("tilematchJava", "stop: " + soundID);
    		TilematchActivity.activePlayers[soundID].stop();
    		TilematchActivity.activePlayers[soundID].reset();
			TilematchActivity.availablePlayers.add(TilematchActivity.activePlayers[soundID]);
			TilematchActivity.activePlayers[soundID] = null;
		} else { 
    		TilematchActivity.soundPool.stop(soundID);
    	}
    }
     
    static public float musicPosition(int soundID) {
    	if (soundID >= 0) {
    		MediaPlayer p = TilematchActivity.activePlayers[soundID];
    		float result = p.getCurrentPosition() / (float) p.getDuration();
    		if (result >= 0.99) {
    			Log.i("tilematchJava", "done: " + soundID);
    			p.stop();
    			p.reset();
    			TilematchActivity.activePlayers[soundID] = null;
    			TilematchActivity.availablePlayers.add(p);
    			return 1;
    		}
    		return result;
    	} else {
    		return 0;
    	}
    }
    
    static public void pauseAllSounds() {
    	// TilematchActivity.soundPool. .autoPause();
    	for(MediaPlayer p : TilematchActivity.activePlayers) {
    		if (p!=null)
    			p.pause();
    	}    	
    } 
    
    static public void resumeAllSounds() {
    	for(MediaPlayer p : TilematchActivity.activePlayers) {
    		if (p!=null)
    			p.start(); 
    	}
    }
    
    static public boolean soundEnable(boolean switchIt) {
    	Log.i("TilematchJava", "soundEnable : " + switchIt);
    	SQLiteDatabase db = TilematchActivity.optionsOpenHelper.getWritableDatabase();
    	Cursor cursor = db.query("info", new String[] {"value"}, "opt LIKE \"sound\"", null, null, null, null);
    	String value = null;
    	if (!cursor.moveToFirst()) {
    		Log.w("TilematchJAva", "soundEnable select: no result");
    		ContentValues v = new ContentValues(1);
    		v.put("value", "on");
    		v.put("opt", "sound");
    		value = "on";
    		long rowsAffected = db.insert("info", null , v);
    		Log.i("tilemacthJvave", "plop, insertt: 3"+rowsAffected);
    	} else {
    		int idx = cursor.getColumnIndex("value");
    		value = cursor.getString(idx);
    	}
    	
    	if (switchIt) {
    		ContentValues v = new ContentValues(1);
    		if ("on".equals(value)) {
    			v.put("value", "off");
    		} else {
    			v.put("value", "on");
    		}
    		long rowsAffected  = db.update("info", v, "opt='sound'", null);
    		Log.i("tilemacthJvave", "plop, update: 3"+rowsAffected);
    	}
    	db.close();
    	return "on".equals(value);
    } 
    
	
	static final String[] boards = new String[] {
		"1141887",
		"1149477",
		"1149487",
	};
     
    static public void submitScore(int mode, int points, int level, float time, String name) {
    	SQLiteDatabase db = TilematchActivity.scoreOpenHelper.getWritableDatabase();
    	ContentValues v = new ContentValues();
    	v.put("name", name);
    	v.put("mode", mode);
    	v.put("points", points);
    	v.put("time", time);
    	v.put("level", level);
    	db.insert("score", null, v);
    	db.close();

    	Leaderboard l = new Leaderboard(boards[mode - 1]);
    	Log.i("tilematchJ", "leaderboard id: " + boards[mode - 1]);
		final Score s = new Score((long) ((mode != 2) ? points : (time * 1000)), null);

		s.submitTo(l, new Score.SubmitToCB() {			
			@Override public void onSuccess(boolean newHighScore) {
				Log.i("tilematchJ", "score posting successfull");
			}

			@Override public void onFailure(String exceptionMessage) {
				Log.i("tilematchJ", "score posting failure : " + exceptionMessage);
			}
			
			@Override public void onBlobUploadSuccess() {
			}
			
			@Override public void onBlobUploadFailure(String exceptionMessage) {
			}
		});
    }
    
    static public int getScores(int mode,  int[] points, int[] levels, float[] times, String[] names) {
    	for (int i=0; i<5; i++) {
    		Log.i("tilematchJ", points[i] + ", " + levels[i] + ", " + times[i] + ", " + names[i] + ".");
    	}
    	 
    	SQLiteDatabase db = TilematchActivity.scoreOpenHelper.getWritableDatabase();
    	Cursor cursor = null;
    	if (mode == 1 || mode == 3) {
    		cursor = db.query("score", new String[] {"name", "points", "time", "level"}, "mode='" + mode + "'", null, null, null, "points desc");
    	} else {
    		cursor = db.query("score", new String[] {"name", "points", "time", "level"}, "mode='" + mode + "'", null, null, null, "time asc");
    	}
    	int maxResult = Math.min(5, cursor.getCount());
    	cursor.moveToFirst();
    	Log.i("tilematchJ", "Found " + maxResult + " result");
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
      
    static public void showPlayerNameUi() { 
    	TilematchActivity.nameReady = false;
    	TilematchActivity.playerNameInputView.post(new Runnable() {
			public void run() {
				Log.i("tilematchJ", "requesting user input visibility");
				TilematchActivity.playerNameInputView.setVisibility(View.VISIBLE);
				TilematchActivity.playerNameInputView.requestFocus();
				TilematchActivity.playerNameInputView.invalidate();
				TilematchActivity.playerNameInputView.forceLayout();
				TilematchActivity.playerNameInputView.bringToFront();
		    	TilematchActivity.nameEdit.setText(TilematchActivity.playerName);	
			}
		});
    	Log.i("tilematchJ", "showPlayerNameUI");
    }
    
    static public String queryPlayerName() {
    	if (TilematchActivity.nameReady) {
    		Log.i("tilematchJ", "queryPlayerName done");
    		return TilematchActivity.playerName;
    	} else {
    		return null;
    	}
    }
    
    static public void openfeintLeaderboard(int mode) {
    	if (mode >= 1 && mode <= 3) {
    		Dashboard.openLeaderboard(boards[mode - 1]);
    	}
    }
    
    static class DumbAndroid {
    	AudioTrack track;
    	int bufferSize;
    	Thread writeThread;
    	
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
    		// 1 sec
    		bufferSize = 2 * pcmBufferSize(rate);//10 * AudioTrack.getMinBufferSize(rate, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT);
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
									track.wait(100);
								} catch (InterruptedException exc) {
									   
								}
								cmd = writePendings.poll();
							}
					 	} 
						if (cmd != null) { 
							switch (cmd.type) {
								case Buffer: {
									// Log.i("tilematchJ", "queue " + cmd.buffer.length + "'" + bufferSize);
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
										Log.i("tilematchJ", "Setting offset: " + offset);
										checkReturnCode("setPosition", track.setPlaybackHeadPosition(offset));
									}
									Log.i("tilematchJ", "start track");
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
    		});   
    	} 
    	// buffer pool shared accross all instances
    	static List<byte[]> bufferPool = new ArrayList<byte[]>();
    }   
      
    static void checkReturnCode(String ctx, int result) {
    	switch (result) {
    	case AudioTrack.SUCCESS: Log.i("tilematchJ", ctx + " : success"); break;
    	case AudioTrack.ERROR_BAD_VALUE: Log.i("tilematchJ", ctx + " : bad value"); break;
    	case AudioTrack.ERROR_INVALID_OPERATION: Log.i("tilematchJ", ctx + " : invalid op"); break;
    	}
    }
    static public Object createPlayer(int rate) {
    	return new DumbAndroid(rate);
    }
    
    static public int pcmBufferSize(int sampleRate) {
    	return sampleRate / 2; // * 2;
    }
    static public byte[] allocate(int size) {
    	synchronized (DumbAndroid.bufferPool) {
    		int s = DumbAndroid.bufferPool.size();
			if (s > 0) {
				// Log.i("tilematchJ", "Reuse old buffer (count: " + s + ")");
				return DumbAndroid.bufferPool.remove(s - 1);
			} else {
				// Log.i("tilematchJ", "Create new buffer: " + size + ", rate:" + dumb.track.getSampleRate());
				// assert(size <= dumb.track.getSampleRate() * 2);  
				return new byte[size];
			}
		}
    } 
       
    static public void queueMusicData(Object o, byte[] audioData, int size, int sampleRate) {
    	DumbAndroid dumb = (DumbAndroid) o;

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
	    		dumb.bufferPool.add(audioData);
    		} else {
    			Command cmd = new Command();
    			cmd.type = Type.Buffer;
    			cmd.buffer = audioData;
    			cmd.bufferSize = size;
    			dumb.writePendings.add(cmd); 
    		}
			dumb.track.notify();
		} 
    } 
    
    static public void startPlaying(Object o, Object master, int offset) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	dumb.writeThread.start();
    	Log.i("tilematchJ", "startPlaying! " + o + ", master:" + master + ", " + offset);
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
    	Log.i("tilematchJ", "stop not implemented");
    	dumb.track.stop();
    }
    
    static public int getPosition(Object o) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	return dumb.track.getPlaybackHeadPosition();
    }
    
    static public void setPosition(Object o, int pos) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	Log.i("tilematchJ", "pure setposition not implemented");
    }
    
    static public void setVolume(Object o, float v) {
    	DumbAndroid dumb = (DumbAndroid) o;
    	checkReturnCode("setVolume", dumb.track.setStereoVolume(v, v));
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
}
