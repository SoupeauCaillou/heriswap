#include "StorageAPIAndroidImpl.h"
#include "sac/base/Log.h"

static jmethodID jniMethodLookup(JNIEnv* env, jclass c, const std::string& name, const std::string& signature) {
    jmethodID mId = env->GetStaticMethodID(c, name.c_str(), signature.c_str());
    if (!mId) {
        LOGW("JNI Error : could not find method '%s'/'%s'", name.c_str(), signature.c_str());
    }
    return mId;
}

StorageAPIAndroidImpl::StorageAPIAndroidImpl(JNIEnv* penv) : env(penv) {}

struct StorageAPIAndroidImpl::StorageAPIAndroidImplDatas {
    jclass cls;

    jmethodID soundEnable;
    jmethodID getGameCountBeforeNextAd;
    jmethodID setGameCountBeforeNextAd;
    jmethodID getSavedGamePointsSum;
    jmethodID submitScore;
    jmethodID getScores;
};

StorageAPIAndroidImpl::~StorageAPIAndroidImpl() {
    env->DeleteGlobalRef(datas->cls);
    delete datas;
}

void StorageAPIAndroidImpl::init() {
    datas = new StorageAPIAndroidImplDatas();

    datas->cls = (jclass)env->NewGlobalRef(env->FindClass("net/damsy/soupeaucaillou/tilematch/TilematchJNILib"));
    datas->soundEnable = jniMethodLookup(env, datas->cls, "soundEnable", "(Z)Z");
    datas->getGameCountBeforeNextAd = jniMethodLookup(env, datas->cls, "getGameCountBeforeNextAd", "()I");
    datas->setGameCountBeforeNextAd = jniMethodLookup(env, datas->cls, "setGameCountBeforeNextAd", "(I)V");
    datas->getSavedGamePointsSum = jniMethodLookup(env, datas->cls, "getSavedGamePointsSum", "()I");
    datas->submitScore = jniMethodLookup(env, datas->cls, "submitScore", "(IIIIFLjava/lang/String;)V");
    datas->getScores = jniMethodLookup(env, datas->cls, "getScores", "(II[I[I[F[Ljava/lang/String;)I");
}

void StorageAPIAndroidImpl::submitScore(Score scr, int mode, int diff) {
    jstring name = env->NewStringUTF(scr.name.c_str());
    env->CallStaticVoidMethod(datas->cls, datas->submitScore, mode, diff, scr.points, scr.level, scr.time, name);
}

std::vector<StorageAPI::Score> StorageAPIAndroidImpl::savedScores(int mode, int difficulty) {
    std::vector<StorageAPI::Score> sav;

    // build arrays params
    jintArray points = env->NewIntArray(5);
    jintArray levels = env->NewIntArray(5);
    jfloatArray times = env->NewFloatArray(5);
    jobjectArray names = env->NewObjectArray(5, env->FindClass("java/lang/String"), env->NewStringUTF(""));

    jint idummy[5];
    jfloat fdummy[5];
    for (int i=0; i<5; i++) {
        idummy[i] = i;
        fdummy[i] = 2.0 * i;
    }
    env->SetIntArrayRegion(points, 0, 5, idummy);
    env->SetIntArrayRegion(levels, 0, 5, idummy);
    env->SetFloatArrayRegion(times, 0, 5, fdummy);
    int count = env->CallStaticIntMethod(datas->cls, datas->getScores, mode, difficulty, points, levels, times, names);

    for (int i=0; i<count; i++) {
        StorageAPI::Score s;
        env->GetIntArrayRegion(points, i, 1, &s.points);
        env->GetIntArrayRegion(levels, i, 1, &s.level);
        env->GetFloatArrayRegion(times, i, 1, &s.time);
        jstring n = (jstring)env->GetObjectArrayElement(names, i);
        if (n) {
            const char *mfile = env->GetStringUTFChars(n, 0);
            s.name = (char*)mfile;
            env->ReleaseStringUTFChars(n, mfile);
        } else {
            s.name = "unknown";
        }
        sav.push_back(s);
    }
    
    return sav;
}

bool StorageAPIAndroidImpl::soundEnable(bool switchIt) {
    return env->CallStaticBooleanMethod(datas->cls, datas->soundEnable, switchIt);
}

int StorageAPIAndroidImpl::getGameCountBeforeNextAd() {
    return env->CallStaticIntMethod(datas->cls, datas->getGameCountBeforeNextAd);
}

void StorageAPIAndroidImpl::setGameCountBeforeNextAd(int c) {
    env->CallStaticVoidMethod(datas->cls, datas->setGameCountBeforeNextAd, c);
}

int StorageAPIAndroidImpl::getSavedGamePointsSum() {
    return env->CallStaticIntMethod(datas->cls, datas->getSavedGamePointsSum);
}
