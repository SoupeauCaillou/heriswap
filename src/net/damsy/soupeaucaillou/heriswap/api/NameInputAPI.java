package net.damsy.soupeaucaillou.heriswap.api;

import net.damsy.soupeaucaillou.heriswap.HeriswapActivity;
import net.damsy.soupeaucaillou.heriswap.R;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.view.View;

public class NameInputAPI {
	public static boolean nameReady;
	
	// -------------------------------------------------------------------------
		// NameInputAPI
		// -------------------------------------------------------------------------
		static public void showPlayerNameUi() {
			NameInputAPI.nameReady = false;
			// show input view
			HeriswapActivity.playerNameInputView.post(new Runnable() {
				public void run() {
					//NOLOGLog.i(HeriswapActivity.Tag, "requesting user input visibility");

					// ici
					SQLiteDatabase db = HeriswapActivity.scoreOpenHelper
							.getReadableDatabase();
					Cursor cursor = db
							.rawQuery(
									"select distinct name from score order by rowid desc limit 4",
									null);
					try {
						int count = cursor.getCount();
						if (count > 0) {
							HeriswapActivity.playerNameInputView.findViewById(
									R.id.reuse).setVisibility(View.VISIBLE);
						} else {
							HeriswapActivity.playerNameInputView.findViewById(
									R.id.reuse).setVisibility(View.GONE);
						}
						int i = 0;
						if (cursor.moveToFirst()) {
							do {
								String n = cursor.getString(0);
								//NOLOGLog.i(HeriswapActivity.Tag, "nsssssssssom : " + n);

								if (!n.equals("rzehtrtyBg")) {
									//NOLOGLog.i(HeriswapActivity.Tag, "nom : " + n);
									HeriswapActivity.oldName[i].setText(n);
									HeriswapActivity.oldName[i]
											.setVisibility(View.VISIBLE);
									i++;
								}
							} while (i < 3 && cursor.moveToNext());
						}
						for (; i < 3; i++) {
							HeriswapActivity.oldName[i].setVisibility(View.GONE);
						}
					} finally {
						cursor.close();
					}

					HeriswapActivity.playerNameInputView
							.setVisibility(View.VISIBLE);
					HeriswapActivity.playerNameInputView.requestFocus();
					HeriswapActivity.playerNameInputView.invalidate();
					HeriswapActivity.playerNameInputView.forceLayout();
					HeriswapActivity.playerNameInputView.bringToFront();
					HeriswapActivity.nameEdit.setText("");
				}
			});
			//NOLOGLog.i(HeriswapActivity.Tag, "showPlayerNameUI");
		}

		static public String queryPlayerName() {
			if (NameInputAPI.nameReady) {
				//NOLOGLog.i(HeriswapActivity.Tag, "queryPlayerName done");
				return HeriswapActivity.playerName;
			} else {
				return null;
			}
		}
}
