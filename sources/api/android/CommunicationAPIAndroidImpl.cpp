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
#include "CommunicationAPIAndroidImpl.h"
#include "sac/base/Log.h"

static jmethodID jniMethodLookup(JNIEnv* env, jclass c, const std::string& name, const std::string& signature) {
    jmethodID mId = env->GetStaticMethodID(c, name.c_str(), signature.c_str());
    if (!mId) {
        LOGW("JNI Error : could not find method '%s'/'%s'", name.c_str(), signature.c_str());
    }
    return mId;
}

struct CommunicationAPIAndroidImpl::CommunicationAPIAndroidImplDatas {
    jclass cls;

    jmethodID swarmInstalled;
    jmethodID swarmRegistering;
    jmethodID shareFacebook;
    jmethodID shareTwitter;
    jmethodID rateItNow;
    jmethodID rateItLater;
    jmethodID rateItNever;

    bool initialized;
};

CommunicationAPIAndroidImpl::CommunicationAPIAndroidImpl() {
	datas = new CommunicationAPIAndroidImplDatas();
	datas->initialized = false;
}

CommunicationAPIAndroidImpl::~CommunicationAPIAndroidImpl() {
	if (datas->initialized) {
    	env->DeleteGlobalRef(datas->cls);
	}
    delete datas;
}

void CommunicationAPIAndroidImpl::init(JNIEnv* pEnv) {
	if (datas->initialized) {
		LOGW("CommunicationAPI not properly uninitialized");
	}
	env = pEnv;

    datas->cls = (jclass)env->NewGlobalRef(env->FindClass("net/damsy/soupeaucaillou/heriswap/HeriswapJNILib"));
    datas->swarmInstalled = jniMethodLookup(env, datas->cls, "swarmEnabled", "()Z");
    datas->swarmRegistering = jniMethodLookup(env, datas->cls, "swarmRegistering", "()V");
    datas->shareFacebook = jniMethodLookup(env, datas->cls, "shareFacebook", "()V");
    datas->shareTwitter = jniMethodLookup(env, datas->cls, "shareTwitter", "()V");
    datas->rateItNow = jniMethodLookup(env, datas->cls, "rateItNow", "()V");
    datas->rateItLater = jniMethodLookup(env, datas->cls, "rateItLater", "()V");
    datas->rateItNever = jniMethodLookup(env, datas->cls, "rateItNever", "()V");

    datas->initialized = true;
}

void CommunicationAPIAndroidImpl::uninit() {
	if (datas->initialized) {
		env->DeleteGlobalRef(datas->cls);
		datas->initialized = false;
	}
}

bool CommunicationAPIAndroidImpl::swarmInstalled() {
	return env->CallStaticBooleanMethod(datas->cls, datas->swarmInstalled);
}

void CommunicationAPIAndroidImpl::swarmRegistering() {
	env->CallStaticBooleanMethod(datas->cls, datas->swarmRegistering);
}

void CommunicationAPIAndroidImpl::shareFacebook() {
	LOGI("share facebook !");
	env->CallStaticBooleanMethod(datas->cls, datas->shareFacebook);
}

void CommunicationAPIAndroidImpl::shareTwitter() {
	LOGI("share twitter !");
	env->CallStaticBooleanMethod(datas->cls, datas->shareTwitter);
}


void CommunicationAPIAndroidImpl::rateItNow(){
	env->CallStaticBooleanMethod(datas->cls, datas->rateItNow);
}

void CommunicationAPIAndroidImpl::rateItLater(){
	env->CallStaticBooleanMethod(datas->cls, datas->rateItLater);
}

void CommunicationAPIAndroidImpl::rateItNever(){
	env->CallStaticBooleanMethod(datas->cls, datas->rateItNever);
}
