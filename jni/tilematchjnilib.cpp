/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>

#include "base/Log.h"
#include "sac/base/Vector2.h"
#include "../sources/Game.h"
#include "sac/systems/RenderingSystem.h"
#include "sac/systems/SoundSystem.h"
#include "sac/systems/MusicSystem.h"
#include "sac/base/TouchInputManager.h"

#include "sac/api/android/AssetAPIAndroidImpl.h"
#include "sac/api/android/MusicAPIAndroidImpl.h"
#include "sac/api/android/SoundAPIAndroidImpl.h"
#include "sac/api/android/LocalizeAPIAndroidImpl.h"
#include "sac/api/android/NameInputAPIAndroidImpl.h"

#include <png.h>
#include <algorithm>

#include <sys/time.h>
#define DT 1.0/60.

#ifndef _Included_net_damsy_soupeaucaillou_tilematch_TilematchJNILib
#define _Included_net_damsy_soupeaucaillou_tilematch_TilematchJNILib
#ifdef __cplusplus
extern "C" {
#endif

struct GameHolder;

class AndroidSuccessAPI : public SuccessAPI {
	public:
		GameHolder* holder;
		void successCompleted(const char* description, unsigned long successId);
};

class AndroidStorage: public ScoreStorage {
	public :
		GameHolder* holder;
		
		std::vector<ScoreStorage::Score> getScore(int mode);

		bool soundEnable(bool switchIt);

		void submitScore(ScoreStorage::Score scr, int mode, int diff);

		bool request(std::string s, std::string* res);
		bool initTable();
		void saveOpt(std::string opt, std::string name);
        std::vector<std::string> getName(std::string& result);
		
		void openfeintLB(int mode);

     bool request(std::string s, void* res, int (*callbackP)(void*,int,char**,char**)) {}
};

struct GameHolder {
	Game* game;
	int width, height;
	NameInputAPIAndroidImpl* nameInput;
	AndroidStorage sqlite;
	AndroidSuccessAPI success;
	LocalizeAPIAndroidImpl* localize;

	struct __input {
		 int touching;
		 float x, y;
	} input;
	bool firstCall;
	struct timeval startup_time;
	float dtAccumuled, time;

	JNIEnv *gameThreadEnv, *renderThreadEnv;
	jobject assetManager;
	int openGLESVersion;
};

struct AndroidNativeTouchState : public NativeTouchState{
	GameHolder* holder;
	AndroidNativeTouchState(GameHolder* h) : holder(h) {}

	bool isTouching (Vector2* windowCoords) const {
		windowCoords->X = holder->input.x;
		windowCoords->Y = holder->input.y;

		return holder->input.touching;
	}
};

struct AndroidNativeAssetLoader: public NativeAssetLoader {
	GameHolder* holder;
	AndroidNativeAssetLoader(GameHolder* h) : holder(h) {}

	char* decompressPngImage(const std::string& assetName, int* width, int* height);

	char* loadShaderFile(const std::string& assetName);
};


static char* loadTextfile(const char* assetName);
static char* loadPng(const char* assetName, int* width, int* height);

#define UPDATE_ENV_PTR(ptr, env) if (ptr != env) ptr = env

/*
 * Class:     net_damsy_soupeaucaillou_tilematch_TilematchJNILib
 * Method:    createGame
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_createGame
  (JNIEnv *env, jclass, jobject asset, jint openglesVersion) {
  	LOGW("%s -->", __FUNCTION__);
  	TimeUtil::init();
	GameHolder* hld = new GameHolder();
	hld->localize = new LocalizeAPIAndroidImpl(env);
    hld->nameInput = new NameInputAPIAndroidImpl();
	hld->game = new Game(new AndroidNativeAssetLoader(hld), &hld->sqlite, hld->nameInput, &hld->success, hld->localize);
	hld->renderThreadEnv = env;
	hld->openGLESVersion = openglesVersion;
	hld->assetManager = (jobject)env->NewGlobalRef(asset);
	theRenderingSystem.setNativeAssetLoader(new AndroidNativeAssetLoader(hld));
	theRenderingSystem.opengles2 = (hld->openGLESVersion == 2);
	theTouchInputManager.setNativeTouchStatePtr(new AndroidNativeTouchState(hld));
	hld->sqlite.holder = hld;
	hld->success.holder = hld;
	return (jlong)hld;
}

/*
 * Class:     net_damsy_soupeaucaillou_tilematch_TilematchJNILib
 * Method:    init
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_initFromRenderThread
  (JNIEnv *env, jclass, jlong g, jint w, jint h) {
  LOGW("%s -->", __FUNCTION__);
	GameHolder* hld = (GameHolder*) g;
	UPDATE_ENV_PTR(hld->renderThreadEnv, env);
	hld->width = w;
	hld->height = h;
	
	hld->game->sacInit(hld->width, hld->height);
	LOGW("%s <--", __FUNCTION__);
}

JNIEXPORT void JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_initFromGameThread
  (JNIEnv *env, jclass, jlong g, jbyteArray jstate) {
  	GameHolder* hld = (GameHolder*) g;
	UPDATE_ENV_PTR(hld->gameThreadEnv, env);
	theMusicSystem.musicAPI = new MusicAPIAndroidImpl(env);
	theMusicSystem.assetAPI = new AssetAPIAndroidImpl(env, hld->assetManager);
	theSoundSystem.soundAPI = new SoundAPIAndroidImpl(env, hld->assetManager);

    hld->nameInput->init(env);
	hld->localize->env = env;
	hld->localize->init();	
	theMusicSystem.init();
	theSoundSystem.init();
	theMusicSystem.assetAPI->init();
	
	uint8_t* state = 0;
	int size = 0;
	if (jstate) {
		size = env->GetArrayLength(jstate);
		state = (uint8_t*)env->GetByteArrayElements(jstate, NULL);
		LOGW("Restoring saved state (size:%d)", size);
	} else {
		LOGW("No saved state: creating a new Game instance from scratch");
	}

	hld->game->init(state, size);

	hld->firstCall = true;
	hld->dtAccumuled = 0;
}

/*
 * Class:     net_damsy_soupeaucaillou_tilematch_TilematchJNILib
 * Method:    step
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_step
  (JNIEnv *env, jclass, jlong g) {
  	GameHolder* hld = (GameHolder*) g;

	UPDATE_ENV_PTR(hld->gameThreadEnv, env);
	if (!hld->game)
  		return;
  		
  	if (hld->firstCall) {
		hld->time = TimeUtil::getTime();
		hld->firstCall = false;
	}

	float dt;
	do {
		dt = TimeUtil::getTime() - hld->time;
		if (dt < DT) {
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = (DT - dt) * 1000000000LL;
			nanosleep(&ts, 0);
		}
	} while (dt < DT);

	hld->dtAccumuled += dt;
	hld->time = TimeUtil::getTime();

	while (hld->dtAccumuled >= DT){
		hld->game->tick(hld->dtAccumuled);
		hld->dtAccumuled = 0;
		// hld->dtAccumuled -= DT;
	}
}

static int frameCount = 0;
static float tttttt = 0;

JNIEXPORT void JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_render
  (JNIEnv *env, jclass, jlong g) {
  	GameHolder* hld = (GameHolder*) g;
  	UPDATE_ENV_PTR(hld->renderThreadEnv, env);
	theRenderingSystem.render();  	
	
	frameCount++;
	if (frameCount >= 200) {
		LOGW("fps render: %.2f", 200.0 / (TimeUtil::getTime() - tttttt));
		tttttt = TimeUtil::getTime();
		frameCount = 0;
	}
}

JNIEXPORT void JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_pause
  (JNIEnv *env, jclass, jlong g) {
  	GameHolder* hld = (GameHolder*) g;
  	LOGW("%s -->", __FUNCTION__);
  	if (!hld->game)
  		return;

	hld->game->togglePause(true);
	LOGW("%s <--", __FUNCTION__);
}

/*
 * Class:     net_damsy_soupeaucaillou_tilematch_TilematchJNILib
 * Method:    handleInputEvent
 * Signature: (JIFF)V
 */
JNIEXPORT void JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_handleInputEvent
  (JNIEnv *env, jclass, jlong g, jint evt, jfloat x, jfloat y) {
	GameHolder* hld = (GameHolder*) g;

	/* ACTION_DOWN == 0 | ACTION_MOVE == 2 */
   if (evt == 0 || evt == 2) {
   	hld->input.touching = 1;
    	hld->input.x = x;
   	hld->input.y = y;
   }
   /* ACTION_UP == 1 */
   else if (evt == 1) {
    	hld->input.touching = 0;
   }
}

/*
 * Class:     net_damsy_soupeaucaillou_tilematch_TilematchJNILib
 * Method:    serialiazeState
 * Signature: (J)[B
 */
JNIEXPORT jbyteArray JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_serialiazeState
  (JNIEnv *env, jclass, jlong g) {
	LOGW("%s -->", __FUNCTION__);
	GameHolder* hld = (GameHolder*) g;
	uint8_t* state;
	int size = hld->game->saveState(&state);

	jbyteArray jb = 0;
	if (size) {
		jb = env->NewByteArray(size);
		env->SetByteArrayRegion(jb, 0, size, (jbyte*)state);
		LOGW("Serialized state size: %d", size);
	}

	LOGW("%s <--", __FUNCTION__);
	return jb;
}

/*
 * Class:     net_damsy_soupeaucaillou_tilematch_TilematchJNILib
 * Method:    restoreRenderingSystemState
 * Signature: (J[B)V
 */
JNIEXPORT void JNICALL Java_net_damsy_soupeaucaillou_tilematch_TilematchJNILib_initAndReloadTextures
  (JNIEnv *env, jclass, jlong g) {
  LOGW("%s -->", __FUNCTION__);
  GameHolder* hld = (GameHolder*) g;
  UPDATE_ENV_PTR(hld->renderThreadEnv, env);
  theRenderingSystem.init();
  theRenderingSystem.reloadTextures();
  LOGW("%s <--", __FUNCTION__);
}

static char* loadAsset(JNIEnv *env, jobject assetManager, const std::string& assetName, int* length) {
	jclass util = env->FindClass("net/damsy/soupeaucaillou/tilematch/TilematchJNILib");
	if (!util) {
		LOGW("ERROR - cannot find class (%p)", env);
	}
	jmethodID mid = env->GetStaticMethodID(util, "assetToByteArray", "(Landroid/content/res/AssetManager;Ljava/lang/String;)[B");
    jstring asset = env->NewStringUTF(assetName.c_str());
    jobject _a = env->CallStaticObjectMethod(util, mid, assetManager, asset);

	if (_a) {
		jbyteArray a = (jbyteArray)_a;
		*length = env->GetArrayLength(a);
		jbyte* res = new jbyte[*length + 1];
		env->GetByteArrayRegion(a, 0, *length, res);
		res[*length] = '\0';
		return (char*)res;
	} else {
		LOGW("%s failed to load '%s'\n", __FUNCTION__, assetName.c_str());
		return 0;
	}
}

void read_from_buffer(png_structp png_ptr, png_bytep outBytes,
   png_size_t byteCountToRead) {
   if(png_ptr->io_ptr == NULL)
      return;   // add custom error handling here
   char* buffer = (char*)png_ptr->io_ptr;
   memcpy(outBytes, buffer, byteCountToRead);

	png_ptr->io_ptr = buffer + byteCountToRead;
}

char* AndroidNativeAssetLoader::decompressPngImage(const std::string& assetName, int* width, int* height)
{
	LOGI("loadPng: %s\n", assetName.c_str());
	png_byte* PNG_image_buffer;
	int length = 0;
	char* data = loadAsset(holder->renderThreadEnv, holder->assetManager, assetName, &length);

	GLubyte PNG_header[8];

	memcpy(PNG_header, data, 8);
	if (png_sig_cmp(PNG_header, 0, 8) != 0) {
		LOGW("%s is not a PNG\n", assetName.c_str());
		return 0;
	}

	png_structp PNG_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (PNG_reader == NULL)
	{
		LOGW("Can't start reading %s.\n", assetName.c_str());
		delete[] data;
		return 0;
	}

	png_infop PNG_info = png_create_info_struct(PNG_reader);
	if (PNG_info == NULL)
	{
		LOGW("ERROR: Can't get info for %s\n", assetName.c_str());
		png_destroy_read_struct(&PNG_reader, NULL, NULL);
		delete[] data;
		return 0;
	}

	png_infop PNG_end_info = png_create_info_struct(PNG_reader);
	if (PNG_end_info == NULL)
	{
		LOGW("ERROR: Can't get end info for %s\n", assetName.c_str());
		png_destroy_read_struct(&PNG_reader, &PNG_info, NULL);
		delete[] data;
		return 0;
	}

	if (setjmp(png_jmpbuf(PNG_reader)))
	{
		LOGW("ERROR: Can't load %s\n", assetName.c_str());
		png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
		delete[] data;
		return 0;
	}

	png_set_read_fn(PNG_reader, &data[8], read_from_buffer);
	// png_init_io(PNG_reader, PNG_file);
	png_set_sig_bytes(PNG_reader, 8);

	png_read_info(PNG_reader, PNG_info);

	*width = png_get_image_width(PNG_reader, PNG_info);
	*height = png_get_image_height(PNG_reader, PNG_info);

	png_uint_32 bit_depth, color_type;
	bit_depth = png_get_bit_depth(PNG_reader, PNG_info);
	color_type = png_get_color_type(PNG_reader, PNG_info);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(PNG_reader);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		png_set_expand_gray_1_2_4_to_8(PNG_reader);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(PNG_reader);
	}

	if (png_get_valid(PNG_reader, PNG_info, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(PNG_reader);
	}
	else
	{
		png_set_filler(PNG_reader, 0xff, PNG_FILLER_AFTER);
	}

	if (bit_depth == 16)
	{
		png_set_strip_16(PNG_reader);
	}

	png_read_update_info(PNG_reader, PNG_info);

	PNG_image_buffer = (png_byte*)malloc(4 * (*width) * (*height));
	png_byte** PNG_rows = (png_byte**)malloc(*height * sizeof(png_byte*));

	unsigned int row;
	for (row = 0; row < *height; ++row) {
		PNG_rows[*height - 1 - row] = PNG_image_buffer + (row * 4 * *width);
	}

	png_read_image(PNG_reader, PNG_rows);

	free(PNG_rows);

	png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
	delete[] data;

	return (char*)PNG_image_buffer;
}

char* AndroidNativeAssetLoader::loadShaderFile(const std::string& assetName)
{
	LOGI("loadTextFile: %s", assetName.c_str());
	int length = 0;
	char* result = loadAsset(holder->renderThreadEnv, holder->assetManager, assetName, &length);
	return result;
}

std::vector<ScoreStorage::Score> AndroidStorage::getScore(int mode) {
	std::vector<ScoreStorage::Score> sav;
	JNIEnv* env = holder->gameThreadEnv;
	jclass c = env->FindClass("net/damsy/soupeaucaillou/tilematch/TilematchJNILib");
	jmethodID mid = (env->GetStaticMethodID(c, "getScores", "(I[I[I[F[Ljava/lang/String;)I"));
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
	int count = env->CallStaticIntMethod(c, mid, mode, points, levels, times, names);

	for (int i=0; i<count; i++) {
		ScoreStorage::Score s;
		// s.mode = mode;
		env->GetIntArrayRegion(points, i, 1, &s.points);
		env->GetIntArrayRegion(levels, i, 1, &s.level);
		env->GetFloatArrayRegion(times, i, 1, &s.time);
		jstring n = (jstring)env->GetObjectArrayElement(names, i);
		if (n) {
			const char *mfile = env->GetStringUTFChars(n, 0);
			s.name = (char*)mfile;
			env->ReleaseStringUTFChars(n, mfile);
		} else {
			s.name = "wtf";
		}
		sav.push_back(s);
	}
	
	return sav;
}

bool AndroidStorage::soundEnable(bool switchIt) {
	JNIEnv* env = holder->gameThreadEnv;
	jclass c = env->FindClass("net/damsy/soupeaucaillou/tilematch/TilematchJNILib");
	jmethodID mid = (env->GetStaticMethodID(c, "soundEnable", "(Z)Z"));
	return env->CallStaticBooleanMethod(c, mid, switchIt);
}

void AndroidStorage::submitScore(ScoreStorage::Score scr, int mode, int diff) {
	JNIEnv* env = holder->gameThreadEnv;
	jclass c = env->FindClass("net/damsy/soupeaucaillou/tilematch/TilematchJNILib");
	jmethodID mid = (env->GetStaticMethodID(c, "submitScore", "(IIIFLjava/lang/String;)V"));
	jstring name = env->NewStringUTF(scr.name.c_str());
	env->CallStaticVoidMethod(c, mid, scr.mode, scr.points, scr.level, scr.time, name);
}

void AndroidSuccessAPI::successCompleted(const char* description, unsigned long successId) {
	SuccessAPI::successCompleted(description, successId);
	// android spec stuff			
	JNIEnv* env = holder->gameThreadEnv;
	jclass c = env->FindClass("net/damsy/soupeaucaillou/tilematch/TilematchJNILib");
	jmethodID mid = (env->GetStaticMethodID(c, "unlockAchievement", "(I)V"));
	int sid = (int) successId;
	env->CallStaticVoidMethod(c, mid, sid);
}

bool AndroidStorage::request(std::string s, std::string* res) {
	return false;
	}

bool AndroidStorage::initTable() {
	return false;
}

void AndroidStorage::saveOpt(std::string opt, std::string name){ }
std::vector<std::string> AndroidStorage::getName(std::string& result)  {}

void AndroidStorage::openfeintLB(int mode) {
	JNIEnv* env = holder->gameThreadEnv;
	jclass c = env->FindClass("net/damsy/soupeaucaillou/tilematch/TilematchJNILib");
	jmethodID mid = env->GetStaticMethodID(c, "openfeintLeaderboard", "(I)V");
	env->CallStaticVoidMethod(c, mid, mode);
}

#ifdef __cplusplus
}
#endif
#endif
