#include "StorageAPILinuxImpl.h"
#include <string>
#include <sstream>
#include <sqlite3.h>
#include "base/Log.h"
#include "Callback.h"
#include "../../modes/GameModeManager.h"

static bool request(std::string s, void* res, int (*callbackP)(void*,int,char**,char**)) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc = sqlite3_open("tilematch.db", &db);
    if( rc ){
        LOGI("Can't open database tilematch.db: %s\n", sqlite3_errmsg(db));
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

void StorageAPILinuxImpl::init() {
    bool r = request("", 0, 0);
    if (r) {
        LOGI("initializing database...");
        request("create table score(name varchar2(11) default 'Anonymous', mode number(1) default '0', difficulty number(1) default '1', points number(7) default '0', time number(5) default '0', level number(3) default '1')", 0, 0);
        request("create table info(opt varchar2(8), value varchar2(11), constraint f1 primary key(opt,value))", 0, 0);
        std::string s;
        request("select value from info where opt like 'sound'", &s, 0);
        if (s.length()==0) {
            request("insert into info values('sound', 'on')", 0, 0);
        }
        s = "";
        request("select value from info where opt like 'helpActive'", &s, 0);
        if (s.length()==0) {
            request("insert into info values('helpActive', '1')", 0, 0);
        }
        s = "";
        request("select value from info where opt like 'gameb4Ads'", &s, 0);
        if (s.length()==0) {
            request("insert into info values('gameb4Ads', '2')", 0, 0);
        } else {
            request("UPDATE info SET value='2' where opt='gameb4Ads'",0, 0);
        }
    }
}

void StorageAPILinuxImpl::submitScore(Score scr, int mode, int diff) {
    std::stringstream tmp;
    tmp << "INSERT INTO score VALUES ('" << scr.name <<"'," << mode<<","<<diff<<","<<scr.points<<","<<scr.time<<","<<scr.level<<")";
    request(tmp.str().c_str(), 0, 0);
}

std::vector<StorageAPI::Score> StorageAPILinuxImpl::savedScores(int mode, int difficulty) {
    std::vector<StorageAPI::Score> result;

    std::stringstream tmp;
    tmp << "select * from score where mode= "<< mode << " and difficulty=" << difficulty;
    if (mode==Normal)
        tmp << " order by points desc limit 5";
    else
        tmp << " order by time asc limit 5";
    request(tmp.str().c_str(), &result, callbackScore);
    return result;
}

bool StorageAPILinuxImpl::soundEnable(bool switchIt) {
    std::string s;
    request("select value from info where opt like 'sound'", &s, 0);
    if (switchIt) {
        if (s=="on") request("UPDATE info SET value='off' where opt='sound'",0, 0);
        else request("UPDATE info SET value='on' where opt='sound'",0, 0);
        LOGI("switched to !%s", s.c_str());
    }
    return (s == "on");
}

int StorageAPILinuxImpl::getGameCountBeforeNextAd() {
    std::string s;
    request("select value from info where opt='gameb4Ads'", &s, 0);
    return std::atoi(s.c_str());
}

void StorageAPILinuxImpl::setGameCountBeforeNextAd(int c) {
    std::stringstream s;
    s << "update info set value='" << c << "' where opt='gameb4Ads'";
    request(s.str(),0, 0);
}

int StorageAPILinuxImpl::getSavedGamePointsSum() {
    std::string s = "";
    request("select sum(points) from score", &s, 0);
    return atoi(s.c_str());
}

bool StorageAPILinuxImpl::everyModesPlayed() {
    int s = 0;
    request("select distinct difficulty,mode from score", &s, callbackResultSize);
    return (s==4);
}
