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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.media.AudioManager;
import android.media.SoundPool;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager.LayoutParams;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;

import com.chartboost.sdk.ChartBoost;
import com.chartboost.sdk.ChartBoostDelegate;
import com.openfeint.api.OpenFeint;
 
public class HeriswapActivity extends Activity {
static public final String Tag = "HeriswapJ";
	// public static List<Achievement> achievements = null;
	// public static List<Leaderboard> leaderboards = null;

	static final String TILEMATCH_BUNDLE_KEY = "plop";
	static public long game = 0 ;
	static public Object mutex;
	static public byte[] savedState; 
	static public int openGLESVersion = 2;
	byte[] renderingSystemState;
	static public SoundPool soundPool;
	static public boolean isRunning;
	static public boolean requestPausedFromJava, backPressed;
	static public HeriswapStorage.OptionsOpenHelper optionsOpenHelper;
	static public HeriswapStorage.ScoreOpenHelper scoreOpenHelper;
	static public View playerNameInputView;
	static public EditText nameEdit;
	static public String playerName;
	static public boolean nameReady;
	static public Resources res;
	static public SharedPreferences preferences;
	static public Button[] oldName;
	static public boolean adHasBeenShown, ofHasBeenShown, adWaitingAdDisplay;
	static public boolean runGameLoop;
	
	static public HeriswapActivity activity;
	PowerManager.WakeLock wl;
	HeriswapRenderer renderer;
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		/*if (resultCode == GSSDK.GS_ACTIVITY_RESULT) {
			// notify blocked 
			//NOLOGLog.w(HeriswapActivity.Tag, "Ad hidden");
			adHasBeenShown = true;
		}*/
		ofHasBeenShown = false;
	}
	
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
      Log.i(HeriswapActivity.Tag, "Activity LifeCycle ##### ON CREATE");
        activity = this;
        ofHasBeenShown = adHasBeenShown = adWaitingAdDisplay = false;
        
        ChartBoost _cb = ChartBoost.getSharedChartBoost(this);
        _cb.setAppId(HeriswapSecret.CB_appId);
        _cb.setAppSignature(HeriswapSecret.CB_AppSignature);
        _cb.install();
        _cb.setDelegate(new ChartBoostDelegate() {
        	@Override
        	public void didCloseInterstitial(View interstitialView) {
        		super.didCloseInterstitial(interstitialView);
        		adWaitingAdDisplay = false;
        		adHasBeenShown = true;
        	}
        	
        	@Override
        	public void didFailToLoadInterstitial() {
        		super.didFailToLoadInterstitial();
        		adHasBeenShown = true;
        	}
        	
        	@Override
        	public boolean shouldDisplayInterstitial(View interstitialView) {
        		return adWaitingAdDisplay;
        	}
		});
        _cb.cacheInterstitial();
        
        preferences = getSharedPreferences("HeriswapPref", 0);
        
        mutex = new Object();

        getWindow().setFlags(LayoutParams.FLAG_FULLSCREEN,
    			LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.main);
        res = getResources();
       
        RelativeLayout rl = (RelativeLayout) findViewById(R.id.parent_frame);
        mGLView = (GLSurfaceView) findViewById(R.id.surfaceviewclass);
        
        synchronized (mGLView) {
        	mGLView.setEGLContextClientVersion(2);
        	HeriswapActivity.openGLESVersion = 2;
        	renderer = new HeriswapRenderer(getAssets());
            mGLView.setRenderer(renderer);	
		}
         
        mGLView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        // rl.addView(mGLView);
        playerNameInputView = findViewById(R.id.enter_name);
        nameEdit = (EditText) findViewById(R.id.player_name_input);
        rl.bringChildToFront(playerNameInputView);
        playerNameInputView.setVisibility(View.GONE);

        Button b = (Button) findViewById(R.id.name_save);
        b.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				playerName = filterPlayerName(nameEdit.getText().toString());
 
				//NOLOGLog.i(HeriswapActivity.Tag, "Player name: '" + playerName + "'");
				if (playerName != null && playerName.length() > 0) {
					playerNameInputView.setVisibility(View.GONE);
					HeriswapActivity.nameReady = true;
					// hide keyboard
					InputMethodManager mgr = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
					mgr.hideSoftInputFromWindow(nameEdit.getWindowToken(), 0);
				}
			}
		});
        oldName = new Button[3];
        oldName[0] = (Button)findViewById(R.id.reuse_name_1);
        oldName[1] = (Button)findViewById(R.id.reuse_name_2);
        oldName[2] = (Button)findViewById(R.id.reuse_name_3);
        for (int i=0 ;i<3; i++) {
        	oldName[i].setOnClickListener( new View.OnClickListener() {	
				public void onClick(View v) {
	        		playerName = ((Button)v).getText().toString();
	        		playerNameInputView.setVisibility(View.GONE);
					HeriswapActivity.nameReady = true;
					// hide keyboard
					InputMethodManager mgr = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
					mgr.hideSoftInputFromWindow(nameEdit.getWindowToken(), 0);
	        	}
        	});
        }
        
        HeriswapActivity.scoreOpenHelper = new HeriswapStorage.ScoreOpenHelper(this);
        HeriswapActivity.optionsOpenHelper = new HeriswapStorage.OptionsOpenHelper(this);
        HeriswapActivity.soundPool = new SoundPool(8, AudioManager.STREAM_MUSIC, 0);
        
        if (savedInstanceState != null) {
	        HeriswapActivity.savedState = savedInstanceState.getByteArray(TILEMATCH_BUNDLE_KEY);
	        if (HeriswapActivity.savedState != null) {
	        	//NOLOGLog.i(HeriswapActivity.Tag, "State restored from app bundle");
	        } else {
	        	//NOLOGLog.i(HeriswapActivity.Tag, "WTF?");
	        }
        } else {
        	//NOLOGLog.i(HeriswapActivity.Tag, "savedInstanceState is null");
        }
        
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wl = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
    } 

    @Override
    protected void onPause() {
    	Log.i(HeriswapActivity.Tag, "Activity LifeCycle ##### ON PAUSE");
    	synchronized (mGLView) {
	       	if (renderer != null) {
	       		// must be done before super.pause()
	       		mGLView.onPause();
	       	}
	    }
        if (wl != null)
        	wl.release();
        HeriswapActivity.requestPausedFromJava = true;
        
        if (HeriswapActivity.game != 0) {
	        // TilematchActivity.isRunning = false;
        	HeriswapActivity.runGameLoop = false; // prevent step being called again
	        synchronized (HeriswapActivity.mutex) {
	        	// HeriswapJNILib.invalidateTextures(HeriswapActivity.game);
			}
        }

        OpenFeint.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
    	ChartBoost.getSharedChartBoost(this);
Log.i(HeriswapActivity.Tag, "Activity LifeCycle ##### ON RESUME");
    	ofHasBeenShown  = false;
        super.onResume();
        if (wl != null)
        	wl.acquire();
        HeriswapJNILib.resetTimestep(HeriswapActivity.game);
        HeriswapActivity.requestPausedFromJava = false;
        isRunning = true;
        
        synchronized (mGLView) {
        	if (renderer != null) {
        		mGLView.onResume();
        	}
        }

        OpenFeint.onResume();
    } 
      
    @Override
    protected void onSaveInstanceState(Bundle outState) {
    	//NOLOGLog.i(HeriswapActivity.Tag, "Activity LifeCycle ##### ON SAVE INSTANCE");
    	if (HeriswapActivity.game == 0)
    		return;
    	/* save current state; we'll be used only if app get killed */
    	synchronized (HeriswapActivity.mutex) {
    		//NOLOGLog.i(HeriswapActivity.Tag, "Save state!");
	    	byte[] savedState = HeriswapJNILib.serialiazeState(HeriswapActivity.game);
	    	if (savedState != null) {
	    		outState.putByteArray(TILEMATCH_BUNDLE_KEY, savedState);
	    	}
	    	//NOLOGLog.i(HeriswapActivity.Tag, "State saved");
    	}
    	super.onSaveInstanceState(outState);
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
    	int action = event.getAction();
    	
    	if (game != 0) {
	    	if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_MOVE) {
	    		HeriswapJNILib.handleInputEvent(game, event.getAction(), event.getX(), event.getY());
	    		return true;
	    	}
    	}
    	
    	return super.onTouchEvent(event);
    }
    
    @Override
    public void onBackPressed() {
    	HeriswapActivity.backPressed = true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
    	if (keyCode == KeyEvent.KEYCODE_MENU) {
    		HeriswapActivity.backPressed = true;
    		// TilematchJNILib.pause(game);
    	}
    	return super.onKeyUp(keyCode, event);
    }
    
    public static GLSurfaceView mGLView;

    private String filterPlayerName(String name) {
    	String n = name.trim();
    	return n.replaceAll("[^a-zA-Z0-9 ]"," ").substring(0, Math.min(11, name.length()));
    }
    
    static {
        System.loadLibrary("heriswap");
    }
    
    @Override
    protected void onDestroy() {
    	super.onDestroy();
    	OpenFeint.onExit();
    	Log.i(HeriswapActivity.Tag, "Activity destroyed");
    }
}
