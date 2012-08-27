LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

APP_DIR := $(LOCAL_PATH)

LOCAL_MODULE := heriswap

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
				-I$(LOCAL_PATH)/..

LOCAL_CXXFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
            -I$(LOCAL_PATH)/../sources \
				-I$(LOCAL_PATH)/.. \
				-I$(LOCAL_PATH)/../sac/ \
				-I$(LOCAL_PATH)/../sac/libs/libpng/jni/ \
				-I$(LOCAL_PATH)/../sac/libs/

LOCAL_SRC_FILES := \
    importgl.c \
    tilematchjnilib.cpp \
    ../sources/Game.cpp \
    ../sources/Game_State.cpp \
    ../sources/Game_Private.cpp \
	../sources/GridSystem.cpp \
	../sources/SuccessManager.cpp \
   ../sources/Jukebox.cpp \
   ../sources/CombinationMark.cpp \
   ../sources/TwitchSystem.cpp \
	../sources/AnimedActor.cpp \
	../sources/states/AdsStateManager.cpp \
	../sources/states/BackgroundManager.cpp \
	../sources/states/CountDownStateManager.cpp \
	../sources/states/DeleteGameStateManager.cpp \
	../sources/states/FadeStateManager.cpp	\
	../sources/states/FallGameStateManager.cpp \
	../sources/states/HelpStateManager.cpp \
	../sources/states/LevelStateManager.cpp \
	../sources/states/LogoStateManager.cpp \
	../sources/states/MainMenuGameStateManager.cpp \
	../sources/states/ModeMenuStateManager.cpp \
	../sources/states/PauseStateManager.cpp \
	../sources/states/RateItStateManager.cpp \
	../sources/states/SpawnGameStateManager.cpp \
	../sources/states/UserInputGameStateManager.cpp \
	../sources/modes/GameModeManager.cpp \
	../sources/modes/TilesAttackModeManager.cpp \
	../sources/modes/InGameUiHelper.cpp \
	../sources/modes/NormalModeManager.cpp \
    ../sources/api/android/StorageAPIAndroidImpl.cpp \
    ../sources/api/android/CommunicationAPIAndroidImpl.cpp

LOCAL_STATIC_LIBRARIES := sac png tremor
LOCAL_LDLIBS := -lGLESv2 -lGLESv1_CM -lEGL -llog -lz

include $(BUILD_SHARED_LIBRARY)

include $(APP_DIR)/../sac/build/android/Android.mk
include $(APP_DIR)/../sac/libs/build/android/tremor/Android.mk
include $(APP_DIR)/../sac/libs/build/android/libpng/Android.mk

$(call import-module,android/native_app_glue)
