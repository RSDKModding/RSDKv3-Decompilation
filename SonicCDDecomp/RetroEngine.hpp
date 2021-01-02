#ifndef RETROENGINE_H
#define RETROENGINE_H

// Disables POSIX use c++ name blah blah stuff
#pragma warning(disable : 4996)

// ================
// STANDARD LIBS
// ================
#include <stdio.h>
#include <string.h>

// ================
// STANDARD TYPES
// ================
typedef unsigned char byte;
typedef signed char sbyte;
#if defined _WIN32
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulong;
#endif

// Platforms
#define RETRO_WIN      (0)
#define RETRO_OSX      (1)
#define RETRO_XBOX_360 (2)
#define RETRO_PS3      (3)
#define RETRO_iOS      (4)
#define RETRO_ANDROID  (5)
#define RETRO_WP7      (6)
#define RETRO_LINUX    (7)

#if defined _WIN32
#define RETRO_PLATFORM (RETRO_WIN)
#elif defined __APPLE__
#define RETRO_PLATFORM (RETRO_OSX)
#else
#define RETRO_PLATFORM (RETRO_LINUX) //Default
#endif


#if RETRO_PLATFORM == RETRO_WINDOWS || RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_LINUX
#define RETRO_USING_SDL (1)
#else //Since its an else & not an elif these platforms probably aren't supported yet
#define RETRO_USING_SDL (0)
#endif

#define RETRO_GAME_STANDARD (0)
#define RETRO_GAME_MOBILE   (1)

#if RETRO_PLATFORM == RETRO_iOS || RETRO_PLATFORM == RETRO_ANDROID || RETRO_PLATFORM == RETRO_WP7
#define RETRO_GAMEPLATFORM (RETRO_GAME_MOBILE)
#else
#define RETRO_GAMEPLATFORM (RETRO_GAME_STANDARD)
#endif

#define RETRO_SW_RENDER  (0)
#define RETRO_HW_RENDER  (1)
#define RETRO_RENDERTYPE (RETRO_SW_RENDER)

#define RETRO_USE_HAPTICS (1)

enum RetroLanguages {
    RETRO_EN = 0,
    RETRO_FR = 1,
    RETRO_IT = 2,
    RETRO_DE = 3,
    RETRO_ES = 4,
    RETRO_JP = 5
};

enum RetroStates {
    ENGINE_DEVMENU         = 0,
    ENGINE_MAINGAME        = 1,
    ENGINE_INITDEVMENU     = 2,
    ENGINE_EXITGAME        = 3,
    ENGINE_SCRIPTERROR     = 4,
    ENGINE_ENTER_HIRESMODE = 5,
    ENGINE_EXIT_HIRESMODE  = 6,
    ENGINE_PAUSE           = 7,
    ENGINE_WAIT            = 8,
    ENGINE_VIDEOWAIT       = 9,
};

enum RetroEngineCallbacks {
    CALLBACK_DISPLAYLOGOS            = 0,
    CALLBACK_PRESS_START             = 1,
    CALLBACK_TIMEATTACK_NOTIFY_ENTER = 2,
    CALLBACK_TIMEATTACK_NOTIFY_EXIT  = 3,
    CALLBACK_FINISHGAME_NOTIFY       = 4,
    CALLBACK_RETURNSTORE_SELECTED    = 5,
    CALLBACK_RESTART_SELECTED        = 6,
    CALLBACK_EXIT_SELECTED           = 7,
    CALLBACK_BUY_FULL_GAME_SELECTED  = 8,
    CALLBACK_TERMS_SELECTED          = 9,
    CALLBACK_PRIVACY_SELECTED        = 10,
    CALLBACK_TRIAL_ENDED             = 11,
    CALLBACK_SETTINGS_SELECTED       = 12,
    CALLBACK_PAUSE_REQUESTED         = 13,
    CALLBACK_FULL_VERSION_ONLY       = 14,
    CALLBACK_STAFF_CREDITS           = 15,
    CALLBACK_16                      = 16,
};

enum RetroBytecodeFormat {
    BYTECODE_MOBILE = 0,
    BYTECODE_PC     = 1,
};

// General Defines
#define SCREEN_YSIZE (240)
#define SCREEN_CENTERY (SCREEN_YSIZE / 2)

#if RETRO_PLATFORM == RETRO_WIN
#include <SDL.h>
#include <vorbis/vorbisfile.h>
#include <theora/theora.h>
#include <theoraplay.h>
#elif RETRO_PLATFORM == RETRO_OSX
#include <SDL2/SDL.h>
#include <Vorbis/vorbisfile.h>
#include <Theora/theora.h>
#include "theoraplay.h"

#include "cocoaHelpers.hpp"
#elif RETRO_PLATFORM == RETRO_LINUX
#include <SDL2/SDL.h>
#include <vorbis/vorbisfile.h>
#include <theora/theora.h>
#include <theoraplay.h>
#endif

extern bool usingCWD;
extern bool engineDebugMode;

//Utils
#include "Ini.hpp"

#include "Math.hpp"
#include "String.hpp"
#include "Reader.hpp"
#include "Animation.hpp"
#include "Audio.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "Player.hpp"
#include "Palette.hpp"
#include "Drawing.hpp"
#include "Scene3D.hpp"
#include "Collision.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Sprite.hpp"
#include "Text.hpp"
#include "Video.hpp"
#include "Userdata.hpp"
#include "Debug.hpp"

class RetroEngine
{
public:
    bool usingDataFile = false;
    bool usingBytecode = false;
    byte bytecodeMode  = BYTECODE_MOBILE;

    bool initialised = false;
    bool running     = false;

    int gameMode     = 1;
    int language     = RETRO_EN;
    int message      = 0;
    bool highResMode = false;

    bool trialMode      = false;
    bool onlineActive   = true;
    bool hapticsEnabled = true;

    int frameSkipSetting = 0;
    int frameSkipTimer   = 0;

    bool useSteamDir = true;
    
    // Ported from RSDKv5
    bool devMenu = false;
    int startList  = 0;
    int startStage = 0;
    int gameSpeed        = 1;
    int fastForwardSpeed = 8;
    bool masterPaused    = false;
    bool frameStep       = false;

    bool showPaletteOverlay = false;

    void Init();
    void Run();

    bool LoadGameConfig(const char *Filepath);

    int callbackMessage = 0;
    int prevMessage     = 0;
    int waitValue       = 0;
    void Callback(int callbackID);

    char gameWindowText[0x40];
    char gameDescriptionText[0x100];
    const char *gameVersion       = "1.0.0";
#if RETRO_GAMEPLATFORM == RETRO_GAME_STANDARD
    const char *gamePlatform = "Standard";
#elif RETRO_GAMEPLATFORM == RETRO_GAME_MOBILE
    const char *gamePlatform   = "Mobile";
#endif

#if RETRO_RENDERTYPE == RETRO_SW_RENDER
    const char *gameRenderType = "SW_Rendering";
#elif RETRO_RENDERTYPE == RETRO_HW_RENDER
    const char *gameRenderType = "HW_Rendering";
#endif

#if RETRO_USE_HAPTICS
    const char *gameHapticSetting = "Use_Haptics"; // No_Haptics is default for pc but people with controllers exist
#endif


    ushort *frameBuffer = nullptr;
    uint *videoFrameBuffer = nullptr;

    bool isFullScreen = false;

    bool fullScreen = false;
    bool borderless = false;
    bool vsync = false;
    int windowScale = 2;
    int refreshRate = 60;

#if RETRO_USING_SDL
    SDL_Window *window        = nullptr;
    SDL_Renderer *renderer    = nullptr;
    SDL_Texture *screenBuffer = nullptr;
    SDL_Texture *videoBuffer = nullptr;

    SDL_Event sdlEvents;
#endif
};

extern RetroEngine Engine;
#endif // !RETROENGINE_H
