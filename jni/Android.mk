LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tilematch

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
				-I$(LOCAL_PATH)/..

LOCAL_CXXFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
            -I$(LOCAL_PATH)/../sources \
				-I$(LOCAL_PATH)/.. \
				-I$(LOCAL_PATH)/../sac/ \
				-I$(LOCAL_PATH)/../libpng-android/jni/

LOCAL_SRC_FILES := \
    importgl.c \
    tilematchjnilib.cpp \
	../sources/Game.cpp \
	../sources/GridSystem.cpp \
   ../sources/PlacementHelper.cpp \
   ../sources/Sound.cpp \
   ../sources/CombinationMark.cpp \
   ../sources/TwitchSystem.cpp \
	../sources/AnimedEntity.cpp \
	../sources/states/BackgroundManager.cpp \
	../sources/states/DeleteGameStateManager.cpp \
	../sources/states/FadeStateManager.cpp	\
	../sources/states/FallGameStateManager.cpp \
	../sources/states/LevelStateManager.cpp \
	../sources/states/LogoStateManager.cpp \
	../sources/states/MainMenuGameStateManager.cpp \
	../sources/states/ModeMenuStateManager.cpp \
	../sources/states/PauseStateManager.cpp \
	../sources/states/SpawnGameStateManager.cpp \
	../sources/states/UserInputGameStateManager.cpp \
	../sources/modes/GameModeManager.cpp \
	../sources/modes/InGameUiHelper.cpp \
	../sources/modes/NormalModeManager.cpp \
	../sources/modes/StaticTimeModeManager.cpp \
	../sources/modes/ScoreAttackModeManager.cpp

LOCAL_STATIC_LIBRARIES := sac png
LOCAL_LDLIBS := -lGLESv2 -lGLESv1_CM -lEGL -llog -lz

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../sac/build/android/Android.mk
include $(LOCAL_PATH)/../libpng-android/jni/Android.mk

$(call import-module,android/native_app_glue)
