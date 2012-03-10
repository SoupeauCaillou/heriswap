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
	    		for(MediaPlayer p : TilematchActivity.players) {
	    			if (!p.isPlaying()) {
	    				p.reset();
	    				p.setDataSource(fd.getFileDescriptor(), fd.getStartOffset(), fd.getLength());
	    				p.setVolume(1.0f, 1.0f);
	    				p.prepare();
	    				return TilematchActivity.players.indexOf(p);
	    			}
	    		}
	    		Log.w("tilematch", "No available MediaPlayer for playing: " + assetPath);
	    		return -1;
	    	} else {
		    	return TilematchActivity.soundPool.load(mgr.openFd(assetPath), 1);
	    	}
    	} catch (Exception exc) {
    		Log.e("tilematch", "Unable to load sound: " + assetPath);
    		return -1;
    	}
    }
    
    static public int playSound(int soundID, boolean music) {
    	if (soundID < 0)
    		return soundID;
    	if (music) {
    		Log.i("tilematch", "play: " + soundID);
    		TilematchActivity.players.get(soundID).start();
    		return soundID;
    	} else { 
    		return TilematchActivity.soundPool.play(soundID, 1.0f, 1.0f, 0, 0, 1.0f);
    	}
    }
     
    static public float musicPosition(int soundID) {
    	if (soundID >= 0) {
    		// Log.i("tilematch", soundID + " position: " + TilematchActivity.players.get(soundID).getCurrentPosition() + ", duration: "+ TilematchActivity.players.get(soundID).getDuration());
    		return TilematchActivity.players.get(soundID).getCurrentPosition() /
    				(float) TilematchActivity.players.get(soundID).getDuration();
    	} else {
    		return 0;
    	}
    }
    
    static public void pauseAllSounds() {
    	TilematchActivity.soundPool.autoPause();
    	for(MediaPlayer p : TilematchActivity.players) {
    		p.pause();
    	}    	
    }
    
    static public void resumeAllSounds() {
    	TilematchActivity.soundPool.autoResume();
    	for(MediaPlayer p : TilematchActivity.players) {
    		if (p.isPlaying()) // ?
    			p.start(); 
    	}
    }
}
