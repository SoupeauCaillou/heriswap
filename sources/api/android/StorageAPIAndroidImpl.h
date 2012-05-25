#pragma once

#include "../StorageAPI.h"
#include <jni.h>

class StorageAPIAndroidImpl : public StorageAPI {
    public:
        StorageAPIAndroidImpl(JNIEnv* env);
        ~StorageAPIAndroidImpl();
        void init();
        void submitScore(Score scr, int mode, int diff);
        std::vector<Score> savedScores(int mode, int difficulty);
        bool soundEnable(bool switchIt);
        int getGameCountBeforeNextAd();
        void setGameCountBeforeNextAd(int c);
        int getSavedGamePointsSum();

    private:
        class StorageAPIAndroidImplDatas;
        StorageAPIAndroidImplDatas* datas;
    public:
        JNIEnv* env;
};
