package net.damsy.soupeaucaillou.heriswap.api;

import net.damsy.soupeaucaillou.SacJNILib;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;

import com.swarmconnect.Swarm;

public class CommunicationAPI {
	// -------------------------------------------------------------------------
		// CommunicationAPI
		// -------------------------------------------------------------------------
		static public boolean swarmEnabled() {
			return Swarm.isLoggedIn();
		}

		static public void swarmRegistering(int mode, int difficulty) {
			SuccessAPI.openLeaderboard(-1, -1);
		}

		static public void shareFacebook() {
			//Intent sharingIntent = new Intent(Intent.ACTION_SEND);
			//sharingIntent.setType("plain/text");
			//sharingIntent.putExtra(android.content.Intent.EXTRA_TEXT, "This is the text that will be shared.");
			//startActivity(Intent.createChooser(sharingIntent,"Share using"));

		}

		static public void shareTwitter() {
		//	String message = "Text I wan't to share.";
			/*Intent share = new Intent(Intent.ACTION_SEND);
			share.setType("text/plain");
			share.putExtra(Intent.EXTRA_TEXT, message);

			startActivity(Intent.createChooser(share, "Title of the dialog the system will open"));*/
		}

		static public boolean mustShowRateDialog() {
			SharedPreferences prefs = SacJNILib.activity.getSharedPreferences("apprater", 0);
			if (prefs.getBoolean("dontshowagain", false))
				return false;
			if (prefs.getLong("launch_count", 0) < 10)
				return false;

			return SacJNILib.activity.canShowAppRater();
		}

		private final static String APP_PNAME = "net.damsy.soupeaucaillou.heriswap";

		static public void rateItNow() {
			SacJNILib.activity.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=" + APP_PNAME)));
			rateItNever();
		}

		static public void rateItLater() {
			SharedPreferences prefs = SacJNILib.activity.getSharedPreferences("apprater", 0);
			SharedPreferences.Editor editor = prefs.edit();
			editor.putLong("launch_count", 0);
			editor.commit();
		}

		static public void rateItNever() {
			SharedPreferences prefs = SacJNILib.activity.getSharedPreferences("apprater", 0);
			SharedPreferences.Editor editor = prefs.edit();
			editor.putBoolean("dontshowagain", true);
			editor.commit();
		}
}
