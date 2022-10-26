# Set for web build. Should be off by default
WEB ?= 0 

ifeq ($(STATIC),1)
		PKG_CONFIG_STATIC_FLAG = --static
		CXXFLAGS_ALL += -static
endif
ifeq ($(WEB), 0)
 CXXFLAGS_ALL += $(shell pkg-config --cflags $(PKG_CONFIG_STATIC_FLAG) vorbisfile vorbis theoradec sdl2 glew) -MMD -MP -MF objects/$*.d -DBASE_PATH='"$(BASE_PATH)"' -pthread
 LIBS_ALL += $(shell pkg-config --libs $(PKG_CONFIG_STATIC_FLAG) vorbisfile vorbis theoradec sdl2 glew) $(LIBS)
 TARGET = $@
 CXX = g++
else
 CXXFLAGS_ALL += -g -sUSE_SDL=2 -sUSE_OGG=1 -sUSE_VORBIS=1 -Idependencies/web/gl4es/include/
 EMLIB = ./dependencies/web/libtheora/lib/.libs/libtheoradec.a ./dependencies/web/gl4es/lib/libGL.a
 TARGET = $@.html
 LDFLAGS_ALL += -sFULL_ES2 -sMIN_WEBGL_VERSION=2 -sTOTAL_MEMORY=96MB --preload-file Data.rsdk -Idependencies/all/theoraplay \
			-Idependencies/all/tinyxml2/
 CXX = em++
endif

CXXFLAGS_ALL += $(CXXFLAGS) \
			-Idependencies/all/filesystem/include \
			-Idependencies/all/theoraplay \
			-Idependencies/all/tinyxml2/
LDFLAGS_ALL += $(LDFLAGS)

SOURCES = \
		dependencies/all/tinyxml2/tinyxml2.cpp \
		dependencies/all/theoraplay/theoraplay.c \
		RSDKv3/Animation.cpp \
		RSDKv3/Audio.cpp \
		RSDKv3/Collision.cpp \
		RSDKv3/Debug.cpp \
		RSDKv3/Drawing.cpp \
		RSDKv3/Ini.cpp \
		RSDKv3/Input.cpp \
		RSDKv3/main.cpp \
		RSDKv3/ModAPI.cpp \
		RSDKv3/Math.cpp \
		RSDKv3/Object.cpp \
		RSDKv3/Palette.cpp \
		RSDKv3/Player.cpp \
		RSDKv3/Reader.cpp \
		RSDKv3/RetroEngine.cpp \
		RSDKv3/Scene.cpp \
		RSDKv3/Scene3D.cpp \
		RSDKv3/Script.cpp \
		RSDKv3/Sprite.cpp \
		RSDKv3/String.cpp \
		RSDKv3/Text.cpp \
		RSDKv3/Userdata.cpp \
		RSDKv3/Video.cpp

			
ifeq ($(FORCE_CASE_INSENSITIVE),1)
		CXXFLAGS_ALL += -DFORCE_CASE_INSENSITIVE
		SOURCES += RSDKv3/fcaseopen.c
endif

OBJECTS = $(SOURCES:%=objects/%.o)
DEPENDENCIES = $(SOURCES:%=objects/%.d)

all: bin/RSDKv3

include $(wildcard $(DEPENDENCIES))

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) -std=c++17 $< -o $@ -c

bin/RSDKv3: $(OBJECTS)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) -std=c++17 $^ $(EMLIB) -o $(TARGET) $(LDFLAGS_ALL) $(LIBS_ALL)


install: bin/RSDKv3
	install -Dp -m755 bin/RSDKv3 $(prefix)/bin/RSDKv3


clean:
		rm -r -f bin && rm -r -f objects
