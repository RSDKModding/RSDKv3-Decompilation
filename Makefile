RETRO_FORCE_CASE_INSENSITIVE ?= 1
RETRO_DISABLE_PLUS           ?= 0
RETRO_USE_HW_RENDER          ?= 1

ifeq ($(STATIC),1)
  PKG_CONFIG_STATIC_FLAG = --static
  CXXFLAGS_ALL += -static
endif

CXXFLAGS_ALL += -MMD -MP -MF objects/$*.d $(shell pkg-config --cflags $(PKG_CONFIG_STATIC_FLAG) vorbisfile vorbis theoradec sdl2 glew) $(CXXFLAGS) \
   -DBASE_PATH='"$(BASE_PATH)"' \
   -Idependencies/all/filesystem/include \
   -Idependencies/all/theoraplay \
   -Idependencies/all/tinyxml2/

LDFLAGS_ALL += $(LDFLAGS)
LIBS_ALL += $(shell pkg-config --libs $(PKG_CONFIG_STATIC_FLAG) vorbisfile vorbis theoradec sdl2 glew) -pthread $(LIBS)

SOURCES = \
  dependencies/all/tinyxml2/tinyxml2.cpp   \
  dependencies/all/theoraplay/theoraplay.c \
  RSDKv3/Animation.cpp       \
  RSDKv3/Audio.cpp           \
  RSDKv3/Collision.cpp       \
  RSDKv3/Debug.cpp           \
  RSDKv3/Drawing.cpp         \
  RSDKv3/Ini.cpp             \
  RSDKv3/Input.cpp           \
  RSDKv3/main.cpp            \
  RSDKv3/ModAPI.cpp          \
  RSDKv3/Math.cpp            \
  RSDKv3/Object.cpp          \
  RSDKv3/Palette.cpp         \
  RSDKv3/Player.cpp          \
  RSDKv3/Reader.cpp          \
  RSDKv3/RetroEngine.cpp     \
  RSDKv3/Scene.cpp           \
  RSDKv3/Scene3D.cpp         \
  RSDKv3/Script.cpp          \
  RSDKv3/Sprite.cpp          \
  RSDKv3/String.cpp          \
  RSDKv3/Text.cpp            \
  RSDKv3/Userdata.cpp        \
  RSDKv3/Video.cpp

ifeq ($(RETRO_FORCE_CASE_INSENSITIVE),1)
  CXXFLAGS_ALL += -DFORCE_CASE_INSENSITIVE
  SOURCES += RSDKv3/fcaseopen.c
endif

ifeq ($(RETRO_USE_HW_RENDER), 0)
	CXXFLAGS_ALL += -RETRO_USING_OPENGL=$(RETRO_USE_HW_RENDER)
endif

ifeq ($(RETRO_DISABLE_PLUS), 1)
	CXXFLAGS_ALL += -DRSDK_AUTOBUILD
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
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

install: bin/RSDKv3
	install -Dp -m755 bin/RSDKv3 $(prefix)/bin/RSDKv3

clean:
	 rm -r -f bin && rm -r -f objects

