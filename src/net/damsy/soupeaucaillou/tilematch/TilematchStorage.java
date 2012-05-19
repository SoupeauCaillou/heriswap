package net.damsy.soupeaucaillou.tilematch;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class TilematchStorage {
	static public class ScoreOpenHelper extends SQLiteOpenHelper {

        private static final int DATABASE_VERSION = 2;
        private static final String SCORE_TABLE_CREATE = "create table score(name char2(25) default 'Anonymous', mode number(1) default '0', difficulty number(1) default '1', points number(7) default '0', time number(5) default '0', level number(3) default '1')";

        ScoreOpenHelper(Context context) {
            super(context, "score", null, DATABASE_VERSION);
        }
        
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        	
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            db.execSQL(SCORE_TABLE_CREATE);
        }
    }
    
    static public class OptionsOpenHelper extends SQLiteOpenHelper {

        private static final int DATABASE_VERSION = 2;
        private static final String INFO_TABLE_CREATE = "create table info(opt char2(8), value char2(25))";

        OptionsOpenHelper(Context context) {
            super(context, "info", null, DATABASE_VERSION);
        }
        
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        	
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            db.execSQL(INFO_TABLE_CREATE);
        }
    }
}
