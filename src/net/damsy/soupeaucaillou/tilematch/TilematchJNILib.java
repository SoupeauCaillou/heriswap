package net.damsy.soupeaucaillou.tilematch;

import java.io.InputStream;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaPlayer;
import android.util.Log;
 
public class TilematchJNILib {
    static {
        System.loadLibrary("tilematch");
    }
      
    /* Create native game instance */
    public static native long createGame(AssetManager mgr, int openGLESVersion);
    /* Initialize game, reset graphics assets, etc... */
    public static native void init(long game, int width, int height, byte[] state);
    public static native void step(long game);
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
}
