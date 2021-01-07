CXXFLAGS = -Og -ggdb3 -fsanitize=address

CXXFLAGS_ALL = $(shell pkg-config --cflags sdl2 theoraplay vorbisfile) $(CXXFLAGS)
LDFLAGS_ALL = $(LDFLAGS)
LIBS_ALL = $(shell pkg-config --libs sdl2 theoraplay vorbisfile) $(LIBS)

OBJECTS = Animation.cpp \
          Audio.cpp \
          Collision.cpp \
          Debug.cpp \
          Drawing.cpp \
          Ini.cpp \
          Input.cpp \
          main.cpp \
          Math.cpp \
          Object.cpp \
          Palette.cpp \
          Player.cpp \
          Reader.cpp \
          RetroEngine.cpp \
          Scene.cpp \
          Scene3D.cpp \
          Script.cpp \
          Sprite.cpp \
          String.cpp \
          Text.cpp \
          Userdata.cpp \
          Video.cpp

objects/%.o: SonicCDDecomp/%
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $^ -o $@ -c

soniccd: $(OBJECTS:%=objects/%.o)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)
