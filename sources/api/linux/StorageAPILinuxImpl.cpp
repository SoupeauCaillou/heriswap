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
#ifndef EMSCRIPTEN
#include <sqlite3.h>
#endif
#include "base/Log.h"
#include "Callback.h"
#include "../../modes/GameModeManager.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifndef EMSCRIPTEN
static bool request(const std::string& dbPath, std::string s, void* res, int (*callbackP)(void*,int,char**,char**)) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if( rc ){
        LOGI("Can't open database %s: %s\n", dbPath.c_str(), sqlite3_errmsg(db));
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
        LOGI("SQL error: %s (asked = %s)\n", zErrMsg, s.c_str());
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    return true;
}
#endif

void StorageAPILinuxImpl::init() {
	#ifndef EMSCRIPTEN
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
			std::cerr << "Failed to create : '%s'" <<  dbPath << std::endl;
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

void StorageAPILinuxImpl::submitScore(Score scr, int mode, int diff) {
	#ifndef EMSCRIPTEN
    std::stringstream tmp;
    tmp << "INSERT INTO score VALUES ('" << scr.name <<"'," << mode<<","<<diff<<","<<scr.points<<","<<scr.time<<","<<scr.level<<")";
    request(dbPath, tmp.str().c_str(), 0, 0);
    #endif
}

std::vector<StorageAPI::Score> StorageAPILinuxImpl::savedScores(int mode, int difficulty) {
    std::vector<StorageAPI::Score> result;
	#ifndef EMSCRIPTEN
    std::stringstream tmp;
    tmp << "select * from score where mode= "<< mode << " and difficulty=" << difficulty;
    if (mode==Normal || mode==RandomNameToBeChanged)
        tmp << " order by points desc limit 5";
    else
        tmp << " order by time asc limit 5";
    request(dbPath, tmp.str().c_str(), &result, callbackScore);
    #endif
    return result;
}

bool StorageAPILinuxImpl::soundEnable(bool switchIt) {
	#ifndef EMSCRIPTEN
    std::string s;
    request(dbPath, "select value from info where opt like 'sound'", &s, 0);
    if (switchIt) {
        if (s=="on") request(dbPath, "UPDATE info SET value='off' where opt='sound'",0, 0);
        else request(dbPath, "UPDATE info SET value='on' where opt='sound'",0, 0);
        LOGI("switched to !%s", s.c_str());
    }
    return (s == "on");
    #else
    return false;
    #endif
}

int StorageAPILinuxImpl::getGameCountBeforeNextAd() {
	#ifndef EMSCRIPTEN
    std::string s;
    request(dbPath, "select value from info where opt='gameb4Ads'", &s, 0);
    return std::atoi(s.c_str());
    #else
    return 0;
   	#endif
}

void StorageAPILinuxImpl::setGameCountBeforeNextAd(int c) {
	#ifndef EMSCRIPTEN
    std::stringstream s;
    s << "update info set value='" << c << "' where opt='gameb4Ads'";
    request(dbPath, s.str(),0, 0);
    #endif
}

int StorageAPILinuxImpl::getSavedGamePointsSum() {
	#ifndef EMSCRIPTEN
    std::string s = "";
    request(dbPath, "select sum(points) from score", &s, 0);
    return atoi(s.c_str());
    #else
    return 0;
    #endif
}

bool StorageAPILinuxImpl::everyModesPlayed() {
	#ifndef EMSCRIPTEN
    int s = 0;
    request(dbPath, "select distinct difficulty,mode from score", &s, callbackResultSize);
    return (s==4);
    #else
    return false;
    #endif
}
