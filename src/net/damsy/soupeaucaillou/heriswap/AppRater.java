package net.damsy.soupeaucaillou.heriswap;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class AppRater {
    private final static String APP_TITLE = "Heriswap";
    private final static String APP_PNAME = "net.damsy.soupeaucaillou.heriswap";
    private final static int LAUNCHES_UNTIL_PROMPT = 7;
    
    public static void app_launched(Context mContext, int width) {
    	SharedPreferences prefs = mContext.getSharedPreferences("apprater", 0);
        if (prefs.getBoolean("dontshowagain", false)) { return ; }
        
        boolean pleaseShow = false;
        
		SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
				.getReadableDatabase();
		Cursor cursor = db.rawQuery("select count (*) from score", null);
		try {
			if (cursor.getCount() > 0) {
				cursor.moveToFirst();
				pleaseShow = (cursor.getInt(0) >= 10);
			}
		} catch (Exception exc) {

		} finally {
			cursor.close();
		} 
		
		
        SharedPreferences.Editor editor = prefs.edit();
        
        // Increment launch counter
        long launch_count = prefs.getLong("launch_count", 0) + 1;
        editor.putLong("launch_count", launch_count);

        // Wait at least n days before opening
        if (launch_count >= LAUNCHES_UNTIL_PROMPT && pleaseShow) {
        	showRateDialog(mContext, editor, width); 
        }
        
        editor.commit();
    }   
    
    public static void showRateDialog(final Context mContext, final SharedPreferences.Editor editor, int width) {
        final Dialog dialog = new Dialog(mContext);
        dialog.setTitle(APP_TITLE);

        LinearLayout ll = new LinearLayout(mContext);
        ll.setOrientation(LinearLayout.VERTICAL);
        
        TextView tv = new TextView(mContext);
        tv.setText(R.string.rate_it);
        //tv.setWidth(width);
        tv.setPadding(4, 0, 4, 10);
        ll.addView(tv);
        
        Button b1 = new Button(mContext);
        b1.setText("Rate " + APP_TITLE);
        b1.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mContext.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=" + APP_PNAME)));
                dialog.dismiss();
            }
        });        
        ll.addView(b1);

        Button b2 = new Button(mContext);
        b2.setText("Remind me later");
        b2.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	editor.putLong("launch_count", 0);
                dialog.dismiss();
            }
        });
        ll.addView(b2);

        Button b3 = new Button(mContext);
        b3.setText("No, thanks");
        b3.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                if (editor != null) {
                    editor.putBoolean("dontshowagain", true);
                    editor.commit();
                }
                dialog.dismiss();
            }
        });
        ll.addView(b3);

        dialog.setContentView(ll);        
        dialog.show();        
    }
}
// see http://androidsnippets.com/prompt-engaged-users-to-rate-your-app-in-the-android-market-appirater