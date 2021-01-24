ifeq ($(STATIC),1)
    PKG_CONFIG_STATIC_FLAG = --static
    CXXFLAGS_ALL += -static
endif

CXXFLAGS_ALL += -MMD -MP -MF objects/$*.d $(shell pkg-config --cflags $(PKG_CONFIG_STATIC_FLAG) sdl2 vorbisfile vorbis theoradec) -Idependencies/all/theoraplay $(CXXFLAGS)
LDFLAGS_ALL += $(LDFLAGS)
LIBS_ALL += $(shell pkg-config --libs $(PKG_CONFIG_STATIC_FLAG) sdl2 vorbisfile vorbis theoradec) -pthread $(LIBS)

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

	  
ifneq ($(FORCE_CASE_INSENSITIVE),)
	CXXFLAGS_ALL += -DFORCE_CASE_INSENSITIVE
	SOURCES += SonicCDDecomp/fcaseopen.c
endif

OBJECTS = $(SOURCES:%=objects/%.o)
DEPENDENCIES = $(SOURCES:%=objects/%.d)

include $(wildcard $(DEPENDENCIES))

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $< -o $@ -c

bin/soniccd: $(SOURCES:%=objects/%.o)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

install: bin/soniccd
	install -Dp -m755 bin/soniccd $(prefix)/bin/soniccd

clean:
	 rm -r -f bin && rm -r -f objects
