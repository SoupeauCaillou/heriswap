package net.damsy.soupeaucaillou.tilematch;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.WindowManager.LayoutParams;

public class TilematchActivity extends Activity {
	static final String TILEMATCH_BUNDLE_KEY = "plop";
	static public long game = 0 	;
	static public Object mutex;
	static public byte[] savedState;
	static public int openGLESVersion = 2;
	byte[] renderingSystemState;
	static public SoundPool soundPool;
	static public List<MediaPlayer> players;
	
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mutex = new Object();
         
        getWindow().setFlags(LayoutParams.FLAG_FULLSCREEN,
    			LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.main);
        
        mGLView = (GLSurfaceView) findViewById(R.id.glview);
        
        TilematchActivity.soundPool = new SoundPool(4, AudioManager.STREAM_MUSIC, 0);
        TilematchActivity.players = new ArrayList<MediaPlayer>(4);
        for(int i=0; i<4; i++) {
        	TilematchActivity.players.add(new MediaPlayer());
        } 
        
        if (savedInstanceState != null) {
	        TilematchActivity.savedState = savedInstanceState.getByteArray(TILEMATCH_BUNDLE_KEY);
	        if (TilematchActivity.savedState != null) {
	        	Log.i("tilematch", "State restored from app bundle");
	        } else {
	        	Log.i("tilematch", "WTF?");
	        }
        } else {
        	Log.i("tilematch", "savedInstanceState is null");
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLView.onPause();
        TilematchJNILib.pause(TilematchActivity.game);
        TilematchActivity.soundPool.autoPause();
        for(MediaPlayer p : TilematchActivity.players) {
        	p.pause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLView.onResume();
    }
    
    @Override
    protected void onSaveInstanceState(Bundle outState) {
    	/* save current state; we'll be used only if app get killed */
    	synchronized (TilematchActivity.mutex) {
	    	Log.i("tilematch", "Save state!");
	    	byte[] savedState = TilematchJNILib.serialiazeState(TilematchActivity.game);
	    	if (savedState != null) {
	    		outState.putByteArray(TILEMATCH_BUNDLE_KEY, savedState);
	    	}
	    	Log.i("tilematch", "State saved");
    	}
    	super.onSaveInstanceState(outState);
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
    	int action = event.getAction();
    	
    	if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_MOVE) {
    		TilematchJNILib.handleInputEvent(game, event.getAction(), event.getX(), event.getY());
    		return true;
    	}
    	
    	return super.onTouchEvent(event);
    }
    
    @Override
    public void onBackPressed() {
    	
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
    	if (keyCode == KeyEvent.KEYCODE_MENU) {
    		TilematchJNILib.pause(game);
    	}
    	return super.onKeyUp(keyCode, event);
    }
    
    private GLSurfaceView mGLView;

    static {
        System.loadLibrary("tilematch");
    }
}
