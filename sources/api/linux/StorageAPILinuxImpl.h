#pragma once

#include "../StorageAPI.h"

class StorageAPILinuxImpl : public StorageAPI {
    public:
        void init();
        void submitScore(Score scr, int mode, int diff);
        std::vector<Score> savedScores(int mode, int difficulty);
        bool soundEnable(bool switchIt);
        int getGameCountBeforeNextAd();
        void setGameCountBeforeNextAd(int c);
        int getSavedGamePointsSum();
        bool everyModesPlayed();
};
