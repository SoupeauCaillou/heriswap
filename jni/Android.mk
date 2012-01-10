LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tilematch

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
				-I$(LOCAL_PATH)/..

LOCAL_CXXFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
				-I$(LOCAL_PATH)/.. \
				-I$(LOCAL_PATH)/../sac/

LOCAL_SRC_FILES := \
    importgl.c \
    tilematch.cpp \
	../sources/Game.cpp \

LOCAL_SHARED_LIBRARIES := sac
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../sac/build/android/Android.mk
