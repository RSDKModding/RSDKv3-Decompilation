ifeq ($(STATIC),1)
  PKG_CONFIG_STATIC_FLAG = --static
  CXXFLAGS_ALL += -static
endif

PKG_CONFIG = pkg-config

ifeq ($(WIIU),1)
  include $(DEVKITPRO)/wut/share/wut_rules
  CXXFLAGS_ALL += -D__WIIU__ -D__WUT__ -ffunction-sections -I$(WUT_ROOT)/include
  LDFLAGS_ALL += $(MACHDEP) $(RPXSPECS) -L$(WUT_ROOT)/lib
  LIBS_ALL += -lwut
  PKG_CONFIG = $(DEVKITPRO)/portlibs/wiiu/bin/powerpc-eabi-pkg-config
endif

CXXFLAGS_ALL += -MMD -MP -MF objects/$*.d $(shell $(PKG_CONFIG) --cflags $(PKG_CONFIG_STATIC_FLAG) sdl2 vorbisfile vorbis theoradec) -Idependencies/all/theoraplay $(CXXFLAGS)
LDFLAGS_ALL += $(LDFLAGS)
LIBS_ALL += $(shell $(PKG_CONFIG) --libs $(PKG_CONFIG_STATIC_FLAG) sdl2 vorbisfile vorbis theoradec) $(LIBS)

SOURCES = \
  dependencies/all/theoraplay/theoraplay.c \
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


ifeq ($(FORCE_CASE_INSENSITIVE),1)
  CXXFLAGS_ALL += -DFORCE_CASE_INSENSITIVE
  SOURCES += SonicCDDecomp/fcaseopen.c
endif

OBJECTS = $(SOURCES:%=objects/%.o)
DEPENDENCIES = $(SOURCES:%=objects/%.d)


ifeq ($(WIIU),1)
all: bin/soniccd.rpx
else
all: bin/soniccd
endif

include $(wildcard $(DEPENDENCIES))

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $< -o $@ -c

ifeq ($(WIIU),1)
bin/soniccd.elf: $(OBJECTS)
else
bin/soniccd: $(OBJECTS)
endif
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

ifneq ($(WIIU),1)
install: bin/soniccd
	install -Dp -m755 bin/soniccd $(prefix)/bin/soniccd
endif

clean:
	 rm -r -f bin && rm -r -f objects
