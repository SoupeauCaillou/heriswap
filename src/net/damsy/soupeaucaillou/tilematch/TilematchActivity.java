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
	static public int openGLESVersion = 2;
	static public byte[] savedState;
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
        
        // renderingSystemState = TilematchJNILib.saveRenderingSystemState(TilematchActivity.game);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLView.onResume();
        
        // TilematchJNILib.restoreRenderingSystemState(TilematchActivity.game, renderingSystemState);
        // renderingSystemState = null;
    }
    
    @Override
    protected void onSaveInstanceState(Bundle outState) {
    	/*synchronized (TilematchActivity.mutex) {
	    	Log.i("tilematch", "Save state!");
	    	byte[] state = TilematchJNILib.serialiazeState(TilematchActivity.game);
	    	outState.putByteArray(TILEMATCH_BUNDLE_KEY, state);
	    	Log.i("tilematch", "State saved");
    	}*/
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
