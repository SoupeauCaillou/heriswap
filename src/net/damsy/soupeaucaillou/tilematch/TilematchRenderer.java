package net.damsy.soupeaucaillou.tilematch;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.util.Log;

public class TilematchRenderer implements GLSurfaceView.Renderer {   
	AssetManager asset; 
	public TilematchRenderer(AssetManager asset) {
		super(); 
		this.asset = asset;
	}
	
    public void onDrawFrame(GL10 gl) {
    	synchronized (TilematchActivity.mutex) {
    		if (TilematchActivity.game == 0)
    	 		return;
    		TilematchJNILib.step(TilematchActivity.game);
    	}
        int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	Log.e("tilematchJava", "GL error : " + GLU.gluErrorString(err));
        } 
    }

    boolean initDone = false;
    public void onSurfaceChanged(GL10 gl, int width, int height) {
    	Log.i("tilematchJava", "surface changed-> width: " + width + ", height: " + height + ", " + initDone);
    	if (!initDone) {
    		TilematchJNILib.init(TilematchActivity.game, width, height, TilematchActivity.savedState);
    		TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
    		TilematchActivity.savedState = null;
    		initDone = true;
    	} else {
    		TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
    	}
    	
    	int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	Log.e("tilematchJava", "_GL error : " + GLU.gluErrorString(err));
        }
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	Log.i("tilematchJava", "Surface created (game: "  + TilematchActivity.game + ", " + initDone + ")");
    	if (TilematchActivity.game == 0) {
    		initDone = false;
    		TilematchActivity.game = TilematchJNILib.createGame(asset, TilematchActivity.openGLESVersion);
    	} else {
    		initDone = true;
    	}
    }
}