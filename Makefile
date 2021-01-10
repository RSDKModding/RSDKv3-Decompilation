CXXFLAGS_ALL = $(shell pkg-config --cflags sdl2 vorbisfile vorbis theoradec) -Idependencies/all/theoraplay $(CXXFLAGS)
LDFLAGS_ALL = $(LDFLAGS)
LIBS_ALL = $(shell pkg-config --libs sdl2 vorbisfile vorbis theoradec) -pthread $(LIBS)

SOURCES = dependencies/all/theoraplay/theoraplay.c \
          SonicCDDecomp/Animation.cpp \
          SonicCDDecomp/Audio.cpp \
          SonicCDDecomp/Collision.cpp \
          SonicCDDecomp/Debug.cpp \
          SonicCDDecomp/Drawing.cpp \
          SonicCDDecomp/Ini.cpp \
          SonicCDDecomp/Input.cpp \
          SonicCDDecomp/main.cpp \
          SonicCDDecomp/Math.cpp \
          SonicCDDecomp/Object.cpp \
          SonicCDDecomp/Palette.cpp \
          SonicCDDecomp/Player.cpp \
          SonicCDDecomp/Reader.cpp \
          SonicCDDecomp/RetroEngine.cpp \
          SonicCDDecomp/Scene.cpp \
          SonicCDDecomp/Scene3D.cpp \
          SonicCDDecomp/Script.cpp \
          SonicCDDecomp/Sprite.cpp \
          SonicCDDecomp/String.cpp \
          SonicCDDecomp/Text.cpp \
          SonicCDDecomp/Userdata.cpp \
          SonicCDDecomp/Video.cpp

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $^ -o $@ -c

soniccd: $(SOURCES:%=objects/%.o)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

install: soniccd
	install -Dp -m755 soniccd $(prefix)/bin/soniccd
