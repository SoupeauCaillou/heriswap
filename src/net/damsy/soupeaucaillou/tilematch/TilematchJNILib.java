package net.damsy.soupeaucaillou.tilematch;

import java.io.InputStream;

import com.openfeint.api.resource.Leaderboard;
import com.openfeint.api.resource.Score;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.media.MediaPlayer;
import android.util.Log;
import android.widget.Toast;
 
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
 
    static public byte[] assetToByteArray(AssetManager mgr, String assetName) {
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
	    			p.setDataSource(fd.getFileDescriptor(), fd.getStartOffset(), fd.getLength());
	    			p.setVolume(0.8f, 0.8f);
	    			p.prepare();
	    			TilematchActivity.availablePlayers.remove(p);
	    			for (int i=0; i<4; i++) {
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
 
    static public int playSound(int soundID, boolean music) {
    	if (soundID < 0)
    		return soundID;
    	if (music) {
    		Log.i("tilematchJava", "play: " + soundID);
    		TilematchActivity.activePlayers[soundID].start();
    		return soundID;
    	} else { 
    		return TilematchActivity.soundPool.play(soundID, 1.0f, 1.0f, 0, 0, 1.0f);
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
    	TilematchActivity.soundPool.autoPause();
    	for(MediaPlayer p : TilematchActivity.activePlayers) {
    		if (p!=null)
    			p.pause();
    	}    	
    }
    
    static public void resumeAllSounds() {
    	TilematchActivity.soundPool.autoResume();
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
    	
    	String[] boards = new String[] {
    		"1141887",
    		"1149477",
    		"1149487",
    	};
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
   
}
