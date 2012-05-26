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
    			// Log.w(TilematchActivity.Tag, "Render thread FPS: " + (float)1000*frameCount / diff);
    			frameCount = 0;
    			time = System.currentTimeMillis();
    		}
    	}
        /*int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	Log.e("tilematchJava", "GL error : " + GLU.gluErrorString(err));
        } */
    }
    
    void startGameThread() {
    	gameThread = new Thread(new Runnable() {
			public void run() {
				TilematchJNILib.initFromGameThread(TilematchActivity.game, TilematchActivity.savedState);
				// force gc before starting game
				System.gc();
				
				TilematchActivity.savedState = null;
				initDone = true;
				while ( TilematchActivity.isRunning || TilematchActivity.requestPausedFromJava) {
					TilematchJNILib.step(TilematchActivity.game);
					TilematchActivity.mGLView.requestRender();
					
					if (TilematchActivity.requestPausedFromJava) {
						TilematchJNILib.pause(TilematchActivity.game);
						TilematchActivity.requestPausedFromJava = false;
					}
				}  
				Log.i(TilematchActivity.Tag, "Activity paused - exiting game thread");
				gameThread = null;
				/*
				synchronized (TilematchActivity.mutex) {
					TilematchJNILib.destroyGame(TilematchActivity.game);
					Log.i(TilematchActivity.Tag, "Clearing game ref");
					TilematchActivity.game = 0;
				}
				*/
			}
		}); 
		gameThread.start(); 
    }
 
    boolean initDone = false;
    public void onSurfaceChanged(GL10 gl, final int width, final int height) {
    	Log.i(TilematchActivity.Tag, "surface changed-> width: " + width + ", height: " + height + ", " + initDone);
    	if (!initDone) {
			TilematchJNILib.initFromRenderThread(TilematchActivity.game, width, height);
    		// TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
    		Log.i(TilematchActivity.Tag, "Start game thread");
    		// create game thread
    		initDone = true;
    		startGameThread();
    	} else {
    		
    		if (gameThread == null)
    			startGameThread();
    	}

    	int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	Log.e(TilematchActivity.Tag, "_GL error : " + GLU.gluErrorString(err));
        }
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	if (TilematchActivity.game == 0) {
    		Log.i(TilematchActivity.Tag, "Activity LifeCycle ##### Game instance creation (onSurfaceCreated)");
    		initDone = false;
    		TilematchActivity.game = TilematchJNILib.createGame(asset, TilematchActivity.openGLESVersion);
    	} else {
    		Log.i(TilematchActivity.Tag, "Activity LifeCycle ##### Game instance reused (onSurfaceCreated)");
    		TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
    		initDone = true;
    	}
    }
}
