package net.damsy.soupeaucaillou.tilematch;

import java.io.InputStream;

import android.content.res.AssetManager;
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
}
