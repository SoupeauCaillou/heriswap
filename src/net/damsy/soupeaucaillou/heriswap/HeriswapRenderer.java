/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
package net.damsy.soupeaucaillou.heriswap;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.util.Log;

import com.openfeint.api.OpenFeint;

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
    		if (HeriswapActivity.game == 0 || !initDone) {
    			Log.i(HeriswapActivity.Tag, "No rendering: " + HeriswapActivity.game + ", " + initDone);
    	 		return;
    		}
    		
    		HeriswapJNILib.render(HeriswapActivity.game);

    		frameCount++;
    		long diff = System.currentTimeMillis() - time;
    		if (diff >= 10000) {
    			Log.w("TAG", "Render thread FPS: " + (float)1000*frameCount / diff);
    			frameCount = 0;
    			time = System.currentTimeMillis();
    		}
    	} 
        int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	Log.e(HeriswapActivity.Tag, "GL error : " + GLU.gluErrorString(err));
        }
    }

    void startGameThread() {
    	// GSSDK.initialize(HeriswapActivity.activity, HeriswapSecret.GS_appId);
    	
    	gameThread = new Thread(new Runnable() {
			public void run() {
				OpenFeint.login();
				HeriswapActivity.runGameLoop = true;
				HeriswapJNILib.initFromGameThread(asset, HeriswapActivity.game, HeriswapActivity.savedState);
				// force gc before starting game
				System.gc();
				 
				HeriswapActivity.savedState = null;
				initDone = true;
				 
				while ( HeriswapActivity.isRunning || HeriswapActivity.requestPausedFromJava) {
					if (HeriswapActivity.runGameLoop) {
						HeriswapJNILib.step(HeriswapActivity.game);
						HeriswapActivity.mGLView.requestRender();
					} else {
						try {
							Log.w(HeriswapActivity.Tag, "Game thread sleeping");
							synchronized (gameThread) {
								gameThread.wait();
							}
							if (HeriswapActivity.runGameLoop) {
								HeriswapJNILib.initFromGameThread(asset, HeriswapActivity.game, null);
							}
						} catch (InterruptedException e) {
							
						}
					}
					
					if (HeriswapActivity.requestPausedFromJava) {
						HeriswapJNILib.pause(HeriswapActivity.game);
						HeriswapJNILib.uninitFromGameThread(HeriswapActivity.game);
						HeriswapActivity.requestPausedFromJava = false;
					} else if (HeriswapActivity.backPressed) {
						HeriswapJNILib.back(HeriswapActivity.game);
						HeriswapActivity.backPressed = false;
					}
				}
				Log.i(HeriswapActivity.Tag, "Activity paused - exiting game thread");
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
    	Log.i(HeriswapActivity.Tag, "surface changed-> width: " + width + ", height: " + height + ", " + initDone);
    	if (!initDone) {
			HeriswapJNILib.initFromRenderThread(asset, HeriswapActivity.game, width, height);
    		// TilematchJNILib.initAndReloadTextures(TilematchActivity.game);
			Log.i(HeriswapActivity.Tag, "Start game thread");
    		// create game thread
    		initDone = true;
    		HeriswapActivity.runGameLoop = true;
    		startGameThread();
    	} else {
    		HeriswapActivity.runGameLoop = true;
    		if (gameThread == null) {
    			Log.i(HeriswapActivity.Tag, "Start game thread");
    			startGameThread();
    		} else {
    			Log.i(HeriswapActivity.Tag, "Wake up game thread");
    			synchronized (gameThread) {
    				gameThread.notify();
				}
    		}
    	}
/*
    	int err;
        while( (err = gl.glGetError()) != GL10.GL_NO_ERROR) {
        	//NOLOGLog.e(HeriswapActivity.Tag, "_GL error : " + GLU.gluErrorString(err));
        }*/
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	if (HeriswapActivity.game == 0) {
    		Log.i("HeriswapActivity.Tag", "Activity LifeCycle ##### Game instance creation (onSurfaceCreated)");
    		initDone = false;
    		HeriswapActivity.game = HeriswapJNILib.createGame(HeriswapActivity.openGLESVersion);
    	} else {
    		Log.i("HeriswapActivity.Tag", "Activity LifeCycle ##### Game instance reused (onSurfaceCreated)");
    		HeriswapJNILib.invalidateTextures(asset, HeriswapActivity.game);
    		HeriswapJNILib.initAndReloadTextures(HeriswapActivity.game);
    		initDone = true;
    	}
    }
}
