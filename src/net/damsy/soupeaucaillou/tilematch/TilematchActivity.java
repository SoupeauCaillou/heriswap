package net.damsy.soupeaucaillou.tilematch;

import android.app.Activity;
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
	
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mutex = new Object();
         
        getWindow().setFlags(LayoutParams.FLAG_FULLSCREEN,
    			LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.main);
        
        mGLView = (GLSurfaceView) findViewById(R.id.glview);
        
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
