package net.damsy.soupeaucaillou.tilematch;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;

public class TilematchActivity extends Activity {
	static public long game = 0 	;
	 
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
         
        setContentView(R.layout.main);
        
        mGLView = (GLSurfaceView) findViewById(R.id.glview);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLView.onResume();
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
    
    private GLSurfaceView mGLView;

    static {
        System.loadLibrary("tilematch");
    }
}
