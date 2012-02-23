LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tilematch

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
                -g -O0 \
				-I$(LOCAL_PATH)/..

LOCAL_CXXFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
                -g -O0 \
            -I$(LOCAL_PATH)/../sources \
				-I$(LOCAL_PATH)/.. \
				-I$(LOCAL_PATH)/../sac/ \
				-I$(LOCAL_PATH)/../libpng-android/jni/

LOCAL_SRC_FILES := \
    importgl.c \
    tilematch.cpp \
	../sources/Game.cpp \
	../sources/GridSystem.cpp \
	../sources/states/BackgroundManager.cpp \
	../sources/states/DeleteGameStateManager.cpp \
	../sources/states/UserInputGameStateManager.cpp \
	../sources/states/SpawnGameStateManager.cpp \
	../sources/states/FallGameStateManager.cpp \
	../sources/states/EndMenuStateManager.cpp \
	../sources/states/MainMenuGameStateManager.cpp \
	../sources/states/ScoreBoardStateManager.cpp \

LOCAL_STATIC_LIBRARIES := android_native_app_glue sac png
LOCAL_LDLIBS := -lGLESv2 -lEGL -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../sac/build/android/Android.mk
include $(LOCAL_PATH)/../libpng-android/jni/Android.mk

$(call import-module,android/native_app_glue)
