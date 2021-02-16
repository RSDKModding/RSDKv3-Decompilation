#ifndef RETROENGINE_H
#define RETROENGINE_H

// Disables POSIX use c++ name blah blah stuff
#pragma warning(disable : 4996)

// ================
// STANDARD LIBS
// ================
#include <stdio.h>
#include <string.h>
#include <cmath>

// ================
// STANDARD TYPES
// ================
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;

// Platforms (RSDKv3 only defines these 7, but feel free to add your own custom platform define for easier platform code changes)
#define RETRO_WIN      (0)
#define RETRO_OSX      (1)
#define RETRO_XBOX_360 (2)
#define RETRO_PS3      (3)
#define RETRO_iOS      (4)
#define RETRO_ANDROID  (5)
#define RETRO_WP7      (6)
// Custom Platforms start here
#define RETRO_VITA (7)
#define RETRO_UWP  (8)

// Platform types (Game manages platform-specific code such as HUD position using this rather than the above)
#define RETRO_STANDARD (0)
#define RETRO_MOBILE   (1)

// use this macro (RETRO_PLATFORM) to define platform specific code blocks and etc to run the engine
#if defined _WIN32
#if defined WINAPI_FAMILY
#if WINAPI_FAMILY != WINAPI_FAMILY_APP
#define RETRO_PLATFORM (RETRO_WIN)
#else
#include <WinRTIncludes.hpp>
#define RETRO_PLATFORM (RETRO_UWP)
#endif
#else
#define RETRO_PLATFORM (RETRO_WIN)
#endif
#elif defined __APPLE__
#define RETRO_USING_MOUSE
#define RETRO_USING_TOUCH
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
#define RETRO_PLATFORM (RETRO_iOS)
#elif TARGET_OS_IPHONE
#define RETRO_PLATFORM (RETRO_iOS)
#elif TARGET_OS_MAC
#define RETRO_PLATFORM (RETRO_OSX)
#else
#error "Unknown Apple platform"
#endif
#elif defined __vita__
#define RETRO_PLATFORM (RETRO_VITA)
#else
#define RETRO_PLATFORM (RETRO_WIN) // Default
#endif

#if RETRO_PLATFORM == RETRO_VITA
#define BASE_PATH            "ux0:data/SonicCD/"
#define DEFAULT_SCREEN_XSIZE 480
#define DEFAULT_FULLSCREEN   true
#elif RETRO_PLATFORM == RETRO_UWP
#define BASE_PATH            ""
#define DEFAULT_SCREEN_XSIZE 424
#define DEFAULT_FULLSCREEN   false
#else
#define BASE_PATH            ""
#define RETRO_USING_MOUSE
#define RETRO_USING_TOUCH
#define DEFAULT_SCREEN_XSIZE 424
#define DEFAULT_FULLSCREEN   false
#endif

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_iOS || RETRO_PLATFORM == RETRO_VITA                        \
    || RETRO_PLATFORM == RETRO_UWP
#define RETRO_USING_SDL1 (0)
#define RETRO_USING_SDL2 (1)
#else // Since its an else & not an elif these platforms probably aren't supported yet
#define RETRO_USING_SDL1 (0)
#define RETRO_USING_SDL2 (0)
#endif

#if RETRO_PLATFORM == RETRO_iOS || RETRO_PLATFORM == RETRO_ANDROID || RETRO_PLATFORM == RETRO_WP7
#define RETRO_GAMEPLATFORM (RETRO_MOBILE)
#elif RETRO_PLATFORM == RETRO_UWP
#define RETRO_GAMEPLATFORM (UAP_GetRetroGamePlatform())
#else
#define RETRO_GAMEPLATFORM (RETRO_STANDARD)
#endif

#define RETRO_SW_RENDER  (0)
#define RETRO_HW_RENDER  (1)
#define RETRO_RENDERTYPE (RETRO_SW_RENDER)

#ifdef USE_SW_REN
#undef RETRO_RENDERTYPE
#define RETRO_RENDERTYPE (RETRO_SW_RENDER)
#endif

#ifdef USE_HW_REN
#undef RETRO_RENDERTYPE
#define RETRO_RENDERTYPE (RETRO_HW_RENDER)
#endif

#if RETRO_RENDERTYPE == RETRO_SW_RENDER
#define RETRO_USING_OPENGL (0)
#elif RETRO_RENDERTYPE == RETRO_HW_RENDER
#define RETRO_USING_OPENGL (1)
#endif

#define RETRO_SOFTWARE_RENDER (RETRO_RENDERTYPE == RETRO_SW_RENDER)
#define RETRO_HARDWARE_RENDER (RETRO_RENDERTYPE == RETRO_HW_RENDER)

#if RETRO_USING_OPENGL
#include <GL/glew.h>
#include <GL/glu.h>

#if RETRO_USING_SDL2
#include <SDL_opengl.h>
#endif
#endif

#define RETRO_USE_HAPTICS (1)

#if RETRO_PLATFORM <= RETRO_WP7
#define RETRO_GAMEPLATFORMID (RETRO_PLATFORM)
#else

// use *this* macro to determine what platform the game thinks its running on (since only the first 7 platforms are supported natively by scripts)
#if RETRO_PLATFORM == RETRO_VITA
#define RETRO_GAMEPLATFORMID (RETRO_WIN)
#elif RETRO_PLATFORM == RETRO_UWP
#define RETRO_GAMEPLATFORMID (UAP_GetRetroGamePlatformId())
#else
#error Unspecified RETRO_GAMEPLATFORMID
#endif

#endif

// this macro defines the touch device read by the game (UWP requires DIRECT)
#if defined(RETRO_UWP) && defined(SDL_TOUCH_DEVICE_DIRECT)
#define RETRO_TOUCH_DEVICE SDL_TOUCH_DEVICE_DIRECT
#elif defined(SDL_TOUCH_DEVICE_INDIRECT_ABSOLUTE)
#define RETRO_TOUCH_DEVICE SDL_TOUCH_DEVICE_INDIRECT_ABSOLUTE
#else
#define RETRO_TOUCH_DEVICE 0
#endif

enum RetroLanguages { RETRO_EN = 0, RETRO_FR = 1, RETRO_IT = 2, RETRO_DE = 3, RETRO_ES = 4, RETRO_JP = 5 };

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
    CALLBACK_AGEGATE                 = 100,
};

enum RetroBytecodeFormat {
    BYTECODE_MOBILE = 0,
    BYTECODE_PC     = 1,
};

// General Defines
#define SCREEN_YSIZE   (240)
#define SCREEN_CENTERY (SCREEN_YSIZE / 2)

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_UWP
#if RETRO_USING_SDL2
#include <SDL.h>
#elif RETRO_USING_SDL1
#include <SDL.h>
#endif
#include <vorbis/vorbisfile.h>
#include <theora/theora.h>
#include <theoraplay.h>
#elif RETRO_PLATFORM == RETRO_OSX
#include <SDL2/SDL.h>
#include <Vorbis/vorbisfile.h>
#include <Theora/theora.h>
#include "theoraplay.h"

#include "cocoaHelpers.hpp"
#elif RETRO_PLATFORM == RETRO_iOS
#include <SDL2/SDL.h>
#include <vorbis/vorbisfile.h>
#include <Theora/theora.h>
#include "theoraplay.h"

#include "cocoaHelpers.hpp"
#elif RETRO_PLATFORM == RETRO_VITA
#include <SDL2/SDL.h>
#include <vorbis/vorbisfile.h>
#include <theora/theora.h>
#include <theoraplay.h>
#endif

extern bool usingCWD;
extern bool engineDebugMode;

// Utils
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
    RetroEngine()
    {
        if (RETRO_GAMEPLATFORM == RETRO_STANDARD)
            gamePlatform = "Standard";
        else
            gamePlatform = "Mobile";
    }

    bool usingDataFile      = false;
    bool usingDataFileStore = false;
    bool usingBytecode      = false;
    byte bytecodeMode       = BYTECODE_MOBILE;
    bool forceFolder        = false;

    char dataFile[0x80];

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
    bool devMenu         = false;
    int startList        = 0;
    int startStage       = 0;
    int gameSpeed        = 1;
    int fastForwardSpeed = 8;
    bool masterPaused    = false;
    bool frameStep       = false;

    bool showPaletteOverlay = false;
    bool useHQModes         = true;

    void Init();
    void Run();

    bool LoadGameConfig(const char *Filepath);

    int callbackMessage = 0;
    int prevMessage     = 0;
    int waitValue       = 0;
    void Callback(int callbackID);

    char gameWindowText[0x40];
    char gameDescriptionText[0x100];
    const char *gameVersion = "1.1.0";
    const char *gamePlatform;

#if RETRO_SOFTWARE_RENDER
    const char *gameRenderType = "SW_Rendering";
#elif RETRO_HARDWARE_RENDER
    const char *gameRenderType = "HW_Rendering";
#endif

#if RETRO_USE_HAPTICS
    const char *gameHapticSetting = "Use_Haptics"; // No_Haptics is default for pc but people with controllers exist
#endif

#if RETRO_SOFTWARE_RENDER
    ushort *frameBuffer   = nullptr;
    ushort *frameBuffer2x = nullptr;
#endif

    bool isFullScreen = false;

    bool startFullScreen  = false; // if should start as fullscreen
    bool borderless       = false;
    bool vsync            = false;
    bool enhancedScaling  = true; // enable enhanced scaling
    int windowScale       = 2;
    int refreshRate       = 60; // user-picked screen update rate
    int screenRefreshRate = 60; // hardware screen update rate
    int targetRefreshRate = 60; // game logic update rate

    uint frameCount      = 0; // frames since scene load
    int renderFrameIndex = 0;
    int skipFrameIndex   = 0;

    int windowXSize; // width of window/screen in the previous frame
    int windowYSize; // height of window/screen in the previous frame

#if RETRO_USING_SDL2
    SDL_Window *window          = nullptr;
    SDL_Renderer *renderer      = nullptr;
#if RETRO_SOFTWARE_RENDER
    SDL_Texture *screenBuffer   = nullptr;
    SDL_Texture *screenBuffer2x = nullptr;
    SDL_Texture *videoBuffer    = nullptr;
#endif

    SDL_Event sdlEvents;

#if RETRO_USING_OPENGL
    SDL_GLContext m_glContext; // OpenGL context
#endif

#endif

#if RETRO_USING_SDL1
    SDL_Surface *windowSurface = nullptr;

    SDL_Surface *screenBuffer   = nullptr;
    SDL_Surface *screenBuffer2x = nullptr;
    SDL_Surface *videoBuffer    = nullptr;

    SDL_Event sdlEvents;
#endif
};

extern RetroEngine Engine;
#endif // !RETROENGINE_H
