#pragma once

#include <string>
#include <vector>

class StorageAPI {
    public:
        struct Score {
           int points, level;
            float time;
            std::string name;
        };

        virtual void init() = 0;
        virtual void submitScore(Score scr, int mode, int diff) = 0;
        virtual std::vector<Score> savedScores(int mode, int difficulty) = 0;
        virtual bool soundEnable(bool switchIt) = 0;
        virtual int getGameCountBeforeNextAd() = 0;
        virtual void setGameCountBeforeNextAd(int c) = 0;
        virtual int getSavedGamePointsSum() = 0;
};
