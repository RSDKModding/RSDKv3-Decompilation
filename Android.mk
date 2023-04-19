LOCAL_PATH := $(call my-dir)

OGG_DIR := dependencies/android/libogg
VORBIS_DIR := dependencies/android/libvorbis
THEORA_DIR := dependencies/android/libtheora

OGG_INCLUDES    := $(LOCAL_PATH)/$(OGG_DIR)/include
VORBIS_INCLUDES := $(LOCAL_PATH)/$(VORBIS_DIR)/include \
	                 $(LOCAL_PATH)/$(VORBIS_DIR)/lib
THEORA_INCLUDES := $(LOCAL_PATH)/$(THEORA_DIR)/include \
	                 $(LOCAL_PATH)/$(THEORA_DIR)/lib

######################################################################
# OGG
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE   := libogg
LOCAL_CFLAGS   := -ffast-math -fsigned-char -O2 -fPIC -DPIC \
                  -DBYTE_ORDER=LITTLE_ENDIAN -D_ARM_ASSEM_

LOCAL_C_INCLUDES := $(OGG_INCLUDES)

WILDCARD_SETUP := \
  $(wildcard $(LOCAL_PATH)/$(OGG_DIR)/src/*.c)


LOCAL_SRC_FILES := \
	$(subst jni/src/, , $(WILDCARD_SETUP))

include $(BUILD_STATIC_LIBRARY)

######################################################################
# VORBIS
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE   := libvorbis
LOCAL_CFLAGS   := -ffast-math -fsigned-char -O2 -fPIC -DPIC \
                  -DBYTE_ORDER=LITTLE_ENDIAN -D_ARM_ASSEM_

LOCAL_C_INCLUDES := $(OGG_INCLUDES) $(VORBIS_INCLUDES)

WILDCARD_SETUP := \
  $(wildcard $(LOCAL_PATH)/$(VORBIS_DIR)/lib/*.c)
FILTERED := $(filter-out $(LOCAL_PATH)/$(VORBIS_DIR)/lib/psytune.c, $(WILDCARD_SETUP))

LOCAL_SRC_FILES := \
	$(subst jni/src/, , $(FILTERED))

include $(BUILD_STATIC_LIBRARY)

######################################################################
# THEORA
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE   := libtheora
LOCAL_CFLAGS   := -ffast-math -fsigned-char -O2 -fPIC -DPIC \
                  -DBYTE_ORDER=LITTLE_ENDIAN -D_ARM_ASSEM_

LOCAL_C_INCLUDES := $(OGG_INCLUDES) $(THEORA_INCLUDES)

WILDCARD_SETUP := \
  $(wildcard $(LOCAL_PATH)/$(THEORA_DIR)/lib/*.c)

LOCAL_SRC_FILES := \
	$(subst jni/src/, , $(WILDCARD_SETUP))

include $(BUILD_STATIC_LIBRARY)

######################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_CFLAGS   := -fexceptions
LOCAL_CPPFLAGS :=  -std=c++17
APP_CPPFLAGS :=  -std=c++17

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/$(SDL_PATH)/include \
    $(LOCAL_PATH)/dependencies/all \
    $(LOCAL_PATH)/$(VORBIS_DIR)/include \
    $(LOCAL_PATH)/$(OGG_DIR)/include \
    $(LOCAL_PATH)/$(THEORA_DIR)/include \
    $(LOCAL_PATH)/dependencies/all/theoraplay \
    $(LOCAL_PATH)/dependencies/all/tinyxml2 \


WILDCARD_SETUP := \
  $(wildcard $(LOCAL_PATH)/dependencies/all/theoraplay/*.c) \
  $(wildcard $(LOCAL_PATH)/dependencies/all/tinyxml2/*.cpp) \
  $(wildcard $(LOCAL_PATH)/RSDKv3/*.cpp)

LOCAL_SRC_FILES := $(subst jni/src/, , $(WILDCARD_SETUP))

LOCAL_LDLIBS := -lGLESv1_CM -llog

LOCAL_SHARED_LIBRARIES := SDL2 libogg libvorbis libtheora

include $(BUILD_SHARED_LIBRARY)
