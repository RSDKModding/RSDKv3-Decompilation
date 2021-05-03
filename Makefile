ifeq ($(STATIC),1)
  PKG_CONFIG_STATIC_FLAG = --static
  CXXFLAGS_ALL += -static
endif

CXXFLAGS_ALL += -MMD -MP -MF objects/$*.d $(shell pkg-config --cflags $(PKG_CONFIG_STATIC_FLAG) sdl2 vorbisfile vorbis theoradec) $(CXXFLAGS) \
   -Idependencies/all/filesystem/include \
   -Idependencies/all/theoraplay \
   -Idependencies/all/upng
LDFLAGS_ALL += $(LDFLAGS)
LIBS_ALL += $(shell pkg-config --libs $(PKG_CONFIG_STATIC_FLAG) sdl2 vorbisfile vorbis theoradec) -pthread $(LIBS)

SOURCES = \
  dependencies/all/theoraplay/theoraplay.c \
  RSDKv3/Animation.cpp \
  RSDKv3/Audio.cpp \
  RSDKv3/Collision.cpp \
  RSDKv3/Debug.cpp \
  RSDKv3/Drawing.cpp \
  RSDKv3/Ini.cpp \
  RSDKv3/Input.cpp \
  RSDKv3/main.cpp \
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

ifeq ($(USE_HW_REN),1)
  CXXFLAGS_ALL += -DUSE_HW_REN
  LIBS_ALL += -lGL -lGLEW
endif

OBJECTS = $(SOURCES:%=objects/%.o)
DEPENDENCIES = $(SOURCES:%=objects/%.d)

all: bin/soniccd

include $(wildcard $(DEPENDENCIES))

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) -std=c++17 $< -o $@ -c

bin/soniccd: $(OBJECTS)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

install: bin/soniccd
	install -Dp -m755 bin/soniccd $(prefix)/bin/soniccd

clean:
	 rm -r -f bin && rm -r -f objects
