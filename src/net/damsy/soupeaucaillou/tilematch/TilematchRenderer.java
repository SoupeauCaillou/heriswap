package net.damsy.soupeaucaillou.tilematch;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.util.Log;

public class TilematchRenderer implements GLSurfaceView.Renderer {   
	AssetManager asset; 
	Thread gameThread;
	int frameCount = 0;
	long time;
	public TilematchRenderer(AssetManager asset) {
		super();
		this.asset = asset;
		frameCount = 0;
		time = System.currentTimeMillis();
	}
	
    public void onDrawFrame(GL10 gl) {
    	synchronized (TilematchActivity.mutex) {
    		if (TilematchActivity.game == 0 || !initDone)
    	 		return;
    		
    		TilematchJNILib.render(TilematchActivity.game);
    		
    		frameCount++;
    		long diff = System.currentTimeMillis() - time;
    		if (diff >= 10000) {
    			Log.w("tilematchJava", "Render thread FPS: " + (float)1000*frameCount / diff);
    			frameCount = 0;
    			time = System.currentTimeMillis();
    		}
    	}
        int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	Log.e("tilematchJava", "GL error : " + GLU.gluErrorString(err));
        } 
    }
    
    void startGameThread() {
    	gameThread = new Thread(new Runnable() {
			public void run() {
				TilematchJNILib.initFromGameThread(TilematchActivity.game);
				initDone = true;
				while ( TilematchActivity.isRunning) {
					TilematchJNILib.step(TilematchActivity.game);
					// TilematchActivity.mGLView.requestRender();
				}
				Log.i("tilematchJava", "Activity paused - exiting game thread");
				gameThread = null;
			}
		});
		gameThread.start();
    }

    boolean initDone = false;
    public void onSurfaceChanged(GL10 gl, final int width, final int height) {
    	Log.i("tilematchJava", "surface changed-> width: " + width + ", height: " + height + ", " + initDone);
    	if (!initDone) {
			TilematchJNILib.initFromRenderThread(TilematchActivity.game, width, height, TilematchActivity.savedState);
    		// TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
    		TilematchActivity.savedState = null;
        
    		Log.i("tilematchJava", "Start game thread");
    		// create game thread
    		startGameThread();
    	} else {
    		TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
    		if (gameThread == null)
    			startGameThread();
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
