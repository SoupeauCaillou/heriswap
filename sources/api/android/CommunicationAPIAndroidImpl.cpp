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
    jmethodID shareFacebook;
    jmethodID shareTwitter;

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
    datas->swarmInstalled = jniMethodLookup(env, datas->cls, "swarmInstalled", "(Z)V");
    datas->shareFacebook = jniMethodLookup(env, datas->cls, "shareFacebook", "()V");
    datas->shareTwitter = jniMethodLookup(env, datas->cls, "shareTwitter", "()V");

    datas->initialized = true;
}

void CommunicationAPIAndroidImpl::uninit() {
	if (datas->initialized) {
		env->DeleteGlobalRef(datas->cls);
		datas->initialized = false;
	}
}

bool CommunicationAPIAndroidImpl::swarmInstalled() {
	//return env->CallStaticBooleanMethod(datas->cls, datas->swarmInstalled);
	return false;
}

void CommunicationAPIAndroidImpl::swarmRegistering() {
	//env->CallStaticBooleanMethod(datas->cls, datas->swarmRegistering);
}

void CommunicationAPIAndroidImpl::shareFacebook() {
	//env->CallStaticBooleanMethod(datas->cls, datas->shareFacebook);
	LOGI("share facebook !");
}

void CommunicationAPIAndroidImpl::shareTwitter() {
	//env->CallStaticBooleanMethod(datas->cls, datas->shareTwitter);
	LOGI("share twitter !");
}
