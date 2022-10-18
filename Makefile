# Set for web build. Should be off by default
WEB ?= 0 

ifeq ($(STATIC),1)
		PKG_CONFIG_STATIC_FLAG = --static
		CXXFLAGS_ALL += -static
endif
ifeq ($(WEB), 0)
 CXXFLAGS_ALL += $(shell pkg-config --cflags $(PKG_CONFIG_STATIC_FLAG) vorbisfile vorbis theoradec sdl2 glew) -MMD -MP -MF objects/$*.d -DBASE_PATH='"$(BASE_PATH)"'
 LIBS_ALL += $(shell pkg-config --libs $(PKG_CONFIG_STATIC_FLAG) vorbisfile vorbis theoradec sdl2 glew) $(LIBS)
 TARGET = $@
else
 CXXFLAGS_ALL += -O2 -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]' -sUSE_OGG=1 -sUSE_VORBIS=1
	EMLIBS += libtheoradec.a
  TARGET = $@.html
endif

CXXFLAGS_ALL += $(CXXFLAGS) \
			-Idependencies/all/filesystem/include \
			-Idependencies/all/theoraplay \
			-Idependencies/all/tinyxml2/
LDFLAGS_ALL += $(LDFLAGS)
ifeq ($(WEB), 1)
LDFLAGS_ALL += -lm -lGL -lSDL2 -sLEGACY_GL_EMULATION -sINITIAL_MEMORY=33554432 -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 -s"EXPORTED_RUNTIME_METHODS=['callMain']" --preload-file Data.rsdk --preload-file settings.ini -Idependencies/all/theoraplay \
			-Idependencies/all/tinyxml2/
endif

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

ifeq ($(WEB),0)
CXX = g++
else
CXX = em++
endif

			
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
	$(CXX) $(CXXFLAGS_ALL) -std=c++17 $^ $(EMLIBS) -o $(TARGET) $(LDFLAGS_ALL) $(LIBS_ALL)


install: bin/RSDKv3
	install -Dp -m755 bin/RSDKv3 $(prefix)/bin/RSDKv3


clean:
		rm -r -f bin && rm -r -f objects
