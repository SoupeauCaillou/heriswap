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
#include "StorageAPILinuxImpl.h"
#include <string>
#include <sstream>
#ifndef SAC_EMSCRIPTEN
#include <sqlite3.h>
#endif
#include "base/Log.h"
#include "Callback.h"
#include "../../modes/GameModeManager.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifndef SAC_EMSCRIPTEN
static bool request(const std::string& dbPath, std::string s, void* res, int (*callbackP)(void*,int,char**,char**)) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if( rc ){
        LOGI("Can't open database '" << dbPath.c_str() << "': '" << sqlite3_errmsg(db) << "'");
        sqlite3_close(db);
        return false;
    }
    //si on veut notre callback personnel(script component)
    if (callbackP && res)
        rc = sqlite3_exec(db, s.c_str(), callbackP, res, &zErrMsg);
    //sinon on prend celui par défaut
    else
        rc = sqlite3_exec(db, s.c_str(), callback, res, &zErrMsg);

    if( rc!=SQLITE_OK ){
        LOGI("SQL error: '" << zErrMsg << "' (asked = '" << s.c_str() << "')");
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    return true;
}
#endif

void StorageAPILinuxImpl::init() {
	#ifndef SAC_EMSCRIPTEN
	std::stringstream ss;
	char * pPath = getenv ("XDG_DATA_HOME");
	if (pPath) {
		ss << pPath;
	} else if ((pPath = getenv ("HOME")) != 0) {
		ss << pPath << "/.local/share/";
	} else {
		ss << "/tmp/";
	}
	ss << "heriswap/";
	dbPath = ss.str();
	// create folder if needed
	struct stat statFolder;
	int st = stat(dbPath.c_str(), &statFolder);
	if (st || (statFolder.st_mode & S_IFMT) != S_IFDIR) {
		if (mkdir(dbPath.c_str(), S_IRWXU | S_IWGRP | S_IROTH)) {
            LOGE("Failed to create : '" <<  dbPath <<  "'");
			return;
		}
	}

	ss << "heriswap.db";
	dbPath = ss.str();
    bool r = request(dbPath, "", 0, 0);
    if (r) {
        LOGI("initializing database...");
        request(dbPath, "create table score(name varchar2(11) default 'Anonymous', mode number(1) default '0', difficulty number(1) default '1', points number(7) default '0', time number(5) default '0', level number(3) default '1')", 0, 0);
        request(dbPath, "create table info(opt varchar2(8), value varchar2(11), constraint f1 primary key(opt,value))", 0, 0);
        std::string s;
        request(dbPath, "select value from info where opt like 'sound'", &s, 0);
        if (s.length()==0) {
            request(dbPath, "insert into info values('sound', 'on')", 0, 0);
        }
        s = "";
        request(dbPath, "select value from info where opt like 'helpActive'", &s, 0);
        if (s.length()==0) {
            request(dbPath, "insert into info values('helpActive', '1')", 0, 0);
        }
        s = "";
        request(dbPath, "select value from info where opt like 'gameb4Ads'", &s, 0);
        if (s.length()==0) {
            request(dbPath, "insert into info values('gameb4Ads', '2')", 0, 0);
        } else {
            request(dbPath, "UPDATE info SET value='2' where opt='gameb4Ads'",0, 0);
        }
    }
    #endif
}
void StorageAPILinuxImpl::submitScore(Score scr, GameMode mode, Difficulty diff) {
	#ifndef SAC_EMSCRIPTEN
    std::stringstream tmp;
    tmp << "INSERT INTO score VALUES ('" << scr.name <<"'," << (int)mode <<","<< (int)diff<<","<<scr.points<<","<<scr.time<<","<<scr.level<<")";
    request(dbPath, tmp.str().c_str(), 0, 0);
    #else
	for (int i=0; i<5; i++) {
		bool replace = false;
		if (mode==Normal || mode==Go100Seconds) {
			replace = (scores[mode][diff][i].points == 0 || scr.points > scores[mode][diff][i].points);
		} else {
			replace = (scores[mode][diff][i].time <= 0 || scr.time < scores[mode][diff][i].time);
		}
		if (replace) {
			for (int j=4; j>i; j--) {
				scores[mode][diff][j] = scores[mode][diff][j-1];
			}
			scores[mode][diff][i] = scr;
			break;
		}
	}
	#endif
}

std::vector<StorageAPI::Score> StorageAPILinuxImpl::savedScores(GameMode mode, Difficulty difficulty, float& avg) {
    std::vector<StorageAPI::Score> result;
	#ifndef SAC_EMSCRIPTEN
    std::stringstream tmp;
		tmp << "select * from score where mode= "<< (int)mode;
	if  (difficulty != SelectAllDifficulty)
		tmp << " and difficulty=" << (int)difficulty;

    if (mode==Normal || mode==Go100Seconds)
        tmp << " order by points desc limit 5";
    else
        tmp << " order by time asc limit 5";
    request(dbPath, tmp.str().c_str(), &result, callbackScore);
   #else
	for (int i=0; i<5; i++) {
		if (mode==Normal || mode==Go100Seconds) {
			if (scores[mode][difficulty][i].points == 0) {
				break;
			} else {
				result.push_back(scores[mode][difficulty][i]);
			}
		} else {
			if (scores[mode][difficulty][i].time <= 0) {
				break;
			} else {
				result.push_back(scores[mode][difficulty][i]);
			}
		}
	}
	#endif
	avg = -1;
    return result;
}

bool StorageAPILinuxImpl::soundEnable(bool switchIt) {
	#ifndef SAC_EMSCRIPTEN
    std::string s;
    request(dbPath, "select value from info where opt like 'sound'", &s, 0);
    if (switchIt) {
        if (s=="on") request(dbPath, "UPDATE info SET value='off' where opt='sound'",0, 0);
        else request(dbPath, "UPDATE info SET value='on' where opt='sound'",0, 0);
        LOGI("switched to !'" << s.c_str() << "'");
    }
    return (s == "on");
    #else
    return true;
    #endif
}

int StorageAPILinuxImpl::getGameCountBeforeNextAd() {
	#ifndef SAC_EMSCRIPTEN
    std::string s;
    request(dbPath, "select value from info where opt='gameb4Ads'", &s, 0);
    return std::atoi(s.c_str());
    #else
    return 0;
   	#endif
}

void StorageAPILinuxImpl::setGameCountBeforeNextAd(int c) {
	#ifndef SAC_EMSCRIPTEN
    std::stringstream s;
    s << "update info set value='" << c << "' where opt='gameb4Ads'";
    request(dbPath, s.str(),0, 0);
    #endif
}

int StorageAPILinuxImpl::getSavedGamePointsSum() {
	#ifndef SAC_EMSCRIPTEN
    std::string s = "";
    request(dbPath, "select sum(points) from score", &s, 0);
    return atoi(s.c_str());
    #else
    return 0;
    #endif
}

bool StorageAPILinuxImpl::everyModesPlayed() {
	#ifndef SAC_EMSCRIPTEN
    int s = 0;
    request(dbPath, "select distinct difficulty,mode from score", &s, callbackResultSize);
    return (s==6);
    #else
    return false;
    #endif
}
