#ifndef RETROENGINE_H
#define RETROENGINE_H

// Disables POSIX use c++ name blah blah stuff
#pragma warning(disable : 4996)

// Setting this to true removes (almost) ALL changes from the original code, the trade off is that a playable game cannot be built, it is advised to
// be set to true only for preservation purposes
#ifndef RETRO_USE_ORIGINAL_CODE
#define RETRO_USE_ORIGINAL_CODE (0)
#endif

#ifndef RETRO_USE_MOD_LOADER
#define RETRO_USE_MOD_LOADER (!RETRO_USE_ORIGINAL_CODE && 1)
#endif

// Forces all DLC flags to be disabled, this should be enabled in any public releases
#ifndef RSDK_AUTOBUILD
#define RSDK_AUTOBUILD (0)
#endif

// ================
// STANDARD LIBS
// ================
#include <stdio.h>
#include <string.h>
#include <cmath>
#if RETRO_USE_MOD_LOADER
#include <regex>
#endif

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
#define RETRO_LINUX (9)

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
#elif defined __ANDROID__
#define RETRO_PLATFORM (RETRO_ANDROID)
#elif defined __vita__
#define RETRO_PLATFORM (RETRO_VITA)
#elif defined __linux__
#define RETRO_PLATFORM (RETRO_LINUX)
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
#ifndef BASE_PATH
#define BASE_PATH            ""
#endif
#define RETRO_USING_MOUSE
#define RETRO_USING_TOUCH
#define DEFAULT_SCREEN_XSIZE 424
#define DEFAULT_FULLSCREEN   false
#endif

#if !defined(RETRO_USE_SDL2) && !defined(RETRO_USE_SDL1)
#define RETRO_USE_SDL2 (1)
#endif

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_iOS || RETRO_PLATFORM == RETRO_VITA                        \
    || RETRO_PLATFORM == RETRO_UWP || RETRO_PLATFORM == RETRO_ANDROID || RETRO_PLATFORM == RETRO_LINUX
#ifdef RETRO_USE_SDL2
#define RETRO_USING_SDL1 (0)
#define RETRO_USING_SDL2 (1)
#elif defined(RETRO_USE_SDL1)
#define RETRO_USING_SDL1 (1)
#define RETRO_USING_SDL2 (0)
#endif
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

#ifndef RETRO_USING_OPENGL
#define RETRO_USING_OPENGL (1)
#endif

#if RETRO_USING_OPENGL
#if RETRO_PLATFORM == RETRO_ANDROID
#define GL_GLEXT_PROTOTYPES

#include <GLES/gl.h>
#include <GLES/glext.h>

#undef glGenFramebuffers
#undef glBindFramebuffers
#undef glFramebufferTexture2D

#undef GL_FRAMEBUFFER
#undef GL_COLOR_ATTACHMENT0
#undef GL_FRAMEBUFFER_BINDING

#define glGenFramebuffers      glGenFramebuffersOES
#define glBindFramebuffer      glBindFramebufferOES
#define glFramebufferTexture2D glFramebufferTexture2DOES
#define glDeleteFramebuffers   glDeleteFramebuffersOES
#define glOrtho                glOrthof

#define GL_FRAMEBUFFER         GL_FRAMEBUFFER_OES
#define GL_COLOR_ATTACHMENT0   GL_COLOR_ATTACHMENT0_OES
#define GL_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_OES
#elif RETRO_PLATFORM == RETRO_OSX
#define GL_GLEXT_PROTOTYPES
#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#undef glGenFramebuffers
#undef glBindFramebuffer
#undef glFramebufferTexture2D
#undef glDeleteFramebuffers

#undef GL_FRAMEBUFFER
#undef GL_COLOR_ATTACHMENT0
#undef GL_FRAMEBUFFER_BINDING

#define glGenFramebuffers      glGenFramebuffersEXT
#define glBindFramebuffer      glBindFramebufferEXT
#define glFramebufferTexture2D glFramebufferTexture2DEXT
#define glDeleteFramebuffers   glDeleteFramebuffersEXT

#define GL_FRAMEBUFFER         GL_FRAMEBUFFER_EXT
#define GL_COLOR_ATTACHMENT0   GL_COLOR_ATTACHMENT0_EXT
#define GL_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_EXT
#else
#include <GL/glew.h>
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
#elif RETRO_PLATFORM == RETRO_LINUX
#define RETRO_GAMEPLATFORMID (RETRO_STANDARD)
#else
#error Unspecified RETRO_GAMEPLATFORMID
#endif

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

enum RetroEngineMessages {
    MESSAGE_NONE         = 0,
    MESSAGE_MESSAGE_1    = 1,
    MESSAGE_LOSTFOCUS    = 2,
    MESSAGE_YES_SELECTED = 3, // Used for old android confirmation popups
    MESSAGE_NO_SELECTED  = 4, // Used for old android confirmation popups
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
    CALLBACK_MOREGAMES               = 16,
    CALLBACK_SHOWREMOVEADS           = 20,
    CALLBACK_AGEGATE                 = 100,

    // Sonic Origins Notify Callbacks
    NOTIFY_DEATH_EVENT         = 128,
    NOTIFY_TOUCH_SIGNPOST      = 129,
    NOTIFY_HUD_ENABLE          = 130,
    NOTIFY_ADD_COIN            = 131,
    NOTIFY_KILL_ENEMY          = 132,
    NOTIFY_SAVESLOT_SELECT     = 133,
    NOTIFY_FUTURE_PAST         = 134,
    NOTIFY_GOTO_FUTURE_PAST    = 135,
    NOTIFY_BOSS_END            = 136,
    NOTIFY_SPECIAL_END         = 137,
    NOTIFY_DEBUGPRINT          = 138,
    NOTIFY_KILL_BOSS           = 139,
    NOTIFY_TOUCH_EMERALD       = 140,
    NOTIFY_STATS_ENEMY         = 141,
    NOTIFY_STATS_CHARA_ACTION  = 142,
    NOTIFY_STATS_RING          = 143,
    NOTIFY_STATS_MOVIE         = 144,
    NOTIFY_STATS_PARAM_1       = 145,
    NOTIFY_STATS_PARAM_2       = 146,
    NOTIFY_CHARACTER_SELECT    = 147,
    NOTIFY_SPECIAL_RETRY       = 148,
    NOTIFY_TOUCH_CHECKPOINT    = 149,
    NOTIFY_ACT_FINISH          = 150,
    NOTIFY_1P_VS_SELECT        = 151,
    NOTIFY_CONTROLLER_SUPPORT  = 152,
    NOTIFY_STAGE_RETRY         = 153,
    NOTIFY_SOUND_TRACK         = 154,
    NOTIFY_GOOD_ENDING         = 155,
    NOTIFY_BACK_TO_MAINMENU    = 156,
    NOTIFY_LEVEL_SELECT_MENU   = 157,
    NOTIFY_PLAYER_SET          = 158,
    NOTIFY_EXTRAS_MODE         = 159,
    NOTIFY_SPIN_DASH_TYPE      = 160,
    NOTIFY_TIME_OVER           = 161,
    NOTIFY_TIMEATTACK_MODE     = 162,
    NOTIFY_STATS_BREAK_OBJECT  = 163,
    NOTIFY_STATS_SAVE_FUTURE   = 164,
    NOTIFY_STATS_CHARA_ACTION2 = 165,

    // Sega Forever stuff
    // Mod CBs start at about 1000
    CALLBACK_SHOWMENU_2                       = 997,
    CALLBACK_SHOWHELPCENTER                   = 998,
    CALLBACK_CHANGEADSTYPE                    = 999,
    CALLBACK_NONE_1000                        = 1000,
    CALLBACK_NONE_1001                        = 1001,
    CALLBACK_NONE_1006                        = 1002,
    CALLBACK_ONSHOWINTERSTITIAL               = 1003,
    CALLBACK_ONSHOWBANNER                     = 1004,
    CALLBACK_ONSHOWBANNER_PAUSESTART          = 1005,
    CALLBACK_ONHIDEBANNER                     = 1006,
    CALLBACK_REMOVEADSBUTTON_FADEOUT          = 1007,
    CALLBACK_REMOVEADSBUTTON_FADEIN           = 1008,
    CALLBACK_ONSHOWINTERSTITIAL_2             = 1009,
    CALLBACK_ONSHOWINTERSTITIAL_3             = 1010,
    CALLBACK_ONSHOWINTERSTITIAL_4             = 1011,
    CALLBACK_ONVISIBLEGRIDBTN_1               = 1012,
    CALLBACK_ONVISIBLEGRIDBTN_0               = 1013,
    CALLBACK_ONSHOWINTERSTITIAL_PAUSEDURATION = 1014,
    CALLBACK_SHOWCOUNTDOWNMENU                = 1015,
    CALLBACK_ONVISIBLEMAINMENU_1              = 1016,
    CALLBACK_ONVISIBLEMAINMENU_0              = 1017,
    CALLBACK_ONSHOWREWARDADS                  = 1018,
    CALLBACK_ONSHOWBANNER_2                   = 1019,
    CALLBACK_ONSHOWINTERSTITIAL_5             = 1020, 

#if RETRO_USE_MOD_LOADER
    // Mod CBs start at 0x1000
    CALLBACK_SET1P = 0x1001,
    CALLBACK_SET2P = 0x1002,
#endif
};

enum RetroRenderTypes {
    RENDER_SW = 0,
    RENDER_HW = 1,
};

enum RetroBytecodeFormat {
    BYTECODE_MOBILE = 0,
    BYTECODE_PC     = 1,
};

// General Defines
#define SCREEN_YSIZE   (240)
#define SCREEN_CENTERY (SCREEN_YSIZE / 2)

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_UWP || RETRO_PLATFORM == RETRO_ANDROID || RETRO_PLATFORM == RETRO_LINUX
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

#if RETRO_PLATFORM == RETRO_ANDROID
#include <jni.h>
#endif

extern bool usingCWD;
extern bool engineDebugMode;
extern byte renderType;

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
#include "ModAPI.hpp"

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

#if !RETRO_USE_ORIGINAL_CODE
    bool usingDataFile_Config = false;
    bool usingDataFileStore   = false;
#endif
    bool usingDataFile = false;
    bool usingBytecode = false;
    byte bytecodeMode  = BYTECODE_MOBILE;
    bool forceFolder   = false;

    char dataFile[0x80];

    bool initialised = false;
    bool running     = false;

    int gameMode      = ENGINE_MAINGAME;
    int language      = RETRO_EN;
    int message       = 0;
    bool highResMode  = false;
    bool useFBTexture = false;

    bool trialMode      = false;
    bool onlineActive   = true;
#if RETRO_USE_HAPTICS
    bool hapticsEnabled = true;
#endif

    int frameSkipSetting = 0;
    int frameSkipTimer   = 0;

    bool useSteamDir = true;

#if !RETRO_USE_ORIGINAL_CODE
    // Ported from RSDKv5
    int startList_Game  = -1;
    int startStage_Game = -1;

    bool consoleEnabled  = false;
    bool devMenu         = false;
    int startList        = -1;
    int startStage       = -1;
    int gameSpeed        = 1;
    int fastForwardSpeed = 8;
    bool masterPaused    = false;
    bool frameStep       = false;
    int dimTimer         = 0;
    int dimLimit         = 0;
    float dimPercent     = 1.0;
    float dimMax         = 1.0;

    char startSceneFolder[0x10];
    char startSceneID[0x10];

    bool showPaletteOverlay = false;
    bool useHQModes         = true;
#endif

    void Init();
    void Run();

    bool LoadGameConfig(const char *filepath);
#if RETRO_USE_MOD_LOADER
    void LoadXMLWindowText();
    void LoadXMLVariables();
    void LoadXMLPalettes();
    void LoadXMLObjects();
    void LoadXMLSoundFX();
    void LoadXMLPlayers(TextMenu *menu);
    void LoadXMLStages(TextMenu *menu, int listNo);
#endif

    bool hasFocus   = true;
    byte focusState = 0;

    int callbackMessage = 0;
    int prevMessage     = 0;
    int waitValue       = 0;
    void Callback(int callbackID);

    char gameWindowText[0x40];
    char gameDescriptionText[0x100];
#ifdef DECOMP_VERSION
    const char *gameVersion = DECOMP_VERSION;
#else
    const char *gameVersion = "1.3.2";
#endif
    const char *gamePlatform;

    const char *gameRenderTypes[2] = { "SW_Rendering", "HW_Rendering" };

    const char *gameRenderType = gameRenderTypes[RENDER_SW];

    // No_Haptics is default for pc but people with controllers exist
#if RETRO_USE_HAPTICS
    const char *gameHapticSetting = "Use_Haptics";
#else
    const char *gameHapticSetting = "No_Haptics";
#endif

    int gameTypeID          = 0;
    const char *releaseType = "Use_Standalone";

    ushort *frameBuffer   = nullptr;
    ushort *frameBuffer2x = nullptr;

    uint *texBuffer   = nullptr;
    uint *texBuffer2x = nullptr;

    bool isFullScreen = false;

    bool startFullScreen  = false; // if should start as fullscreen
    bool borderless       = false;
    bool vsync            = false;
    int scalingMode       = 0;
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
    SDL_Window *window = nullptr;
#if !RETRO_USING_OPENGL
    SDL_Renderer *renderer      = nullptr;
    SDL_Texture *screenBuffer   = nullptr;
    SDL_Texture *screenBuffer2x = nullptr;
    SDL_Texture *videoBuffer    = nullptr;
#endif

    SDL_Event sdlEvents;

#if RETRO_USING_OPENGL
    SDL_GLContext glContext; // OpenGL context
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
