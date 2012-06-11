package net.damsy.soupeaucaillou.heriswap;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openfeint.api.OpenFeint;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
//NOLOGimport android.util.Log;
import android.util.Log;

public class HeriswapRenderer implements GLSurfaceView.Renderer {   
	AssetManager asset; 
	Thread gameThread;
	int frameCount = 0;
	long time;
	public HeriswapRenderer(AssetManager asset) {
		super();
		this.asset = asset;
		frameCount = 0;
		time = System.currentTimeMillis();
	}

    public void onDrawFrame(GL10 gl) {
    	synchronized (HeriswapActivity.mutex) {
    		if (HeriswapActivity.game == 0 || !initDone)
    	 		return;
    		
    		HeriswapJNILib.render(HeriswapActivity.game);

    		frameCount++;
    		long diff = System.currentTimeMillis() - time;
    		if (diff >= 10000) {
    			Log.w("TAG", "Render thread FPS: " + (float)1000*frameCount / diff);
    			frameCount = 0;
    			time = System.currentTimeMillis();
    		}
    	} 
        /*int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	Log.e(HeriswapActivity.Tag, "GL error : " + GLU.gluErrorString(err));
        } */
    }

    void startGameThread() {
    	gameThread = new Thread(new Runnable() {
			public void run() {
				OpenFeint.login();
				HeriswapActivity.runGameLoop = true;
				HeriswapJNILib.initFromGameThread(HeriswapActivity.game, HeriswapActivity.savedState);
				// force gc before starting game
				System.gc();
				
				HeriswapActivity.savedState = null;
				initDone = true;
				
				// TilematchActivity.cb.showInterstitial();
				
				while ( HeriswapActivity.isRunning || HeriswapActivity.requestPausedFromJava) {
					if (HeriswapActivity.runGameLoop) {
						HeriswapJNILib.step(HeriswapActivity.game);
						HeriswapActivity.mGLView.requestRender();
					} else {
						try {
							Thread.sleep(10);
						} catch (InterruptedException e) {}
					}
					
					if (HeriswapActivity.requestPausedFromJava) {
						HeriswapJNILib.pause(HeriswapActivity.game);
						HeriswapActivity.requestPausedFromJava = false;
						// force rendering
						HeriswapActivity.mGLView.requestRender();
					} else if (HeriswapActivity.backPressed) {
						HeriswapJNILib.back(HeriswapActivity.game);
						HeriswapActivity.backPressed = false;
					}
				}
				//NOLOGLog.i(HeriswapActivity.Tag, "Activity paused - exiting game thread");
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
    	//NOLOGLog.i(HeriswapActivity.Tag, "surface changed-> width: " + width + ", height: " + height + ", " + initDone);
    	if (!initDone) {
			HeriswapJNILib.initFromRenderThread(HeriswapActivity.game, width, height);
    		// TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
			//NOLOGLog.i(HeriswapActivity.Tag, "Start game thread");
    		// create game thread
    		initDone = true;
    		startGameThread();
    	} else {
    		if (gameThread == null)
    			startGameThread();
    	}
    	// rendering is ready, game loop can run
    	HeriswapActivity.runGameLoop = true;
/*
    	int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	//NOLOGLog.e(HeriswapActivity.Tag, "_GL error : " + GLU.gluErrorString(err));
        }*/
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	if (HeriswapActivity.game == 0) {
    		//NOLOGLog.i(HeriswapActivity.Tag, "Activity LifeCycle ##### Game instance creation (onSurfaceCreated)");
    		initDone = false;
    		HeriswapActivity.game = HeriswapJNILib.createGame(asset, HeriswapActivity.openGLESVersion);
    	} else {
    		//NOLOGLog.i(HeriswapActivity.Tag, "Activity LifeCycle ##### Game instance reused (onSurfaceCreated)");
    		HeriswapJNILib.invalidateTextures(HeriswapActivity.game);
    		HeriswapJNILib.initAndReloadTextures(HeriswapActivity.game);
    		initDone = true;
    	}
    }
}
