#include "RetroEngine.hpp"

#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
#include <Windows.h>
#include <codecvt>
#include "../dependencies/windows/ValveFileVDF/vdf_parser.hpp"

HKEY hKey;

LONG GetDWORDRegKey(HKEY hKey, const std::wstring &strValueName, DWORD &nValue, DWORD nDefaultValue)
{
    nValue = nDefaultValue;
    DWORD dwBufferSize(sizeof(DWORD));
    DWORD nResult(0);
    LONG nError = ::RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, reinterpret_cast<LPBYTE>(&nResult), &dwBufferSize);
    if (ERROR_SUCCESS == nError) {
        nValue = nResult;
    }
    return nError;
}

LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
{
    strValue = strDefaultValue;
    WCHAR szBuffer[512];
    DWORD dwBufferSize = sizeof(szBuffer);
    ULONG nError;
    nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
    if (ERROR_SUCCESS == nError) {
        strValue = szBuffer;
    }
    return nError;
}

inline std::string utf16ToUtf8(const std::wstring &utf16Str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(utf16Str);
}

inline bool dirExists(const std::wstring &dirName_in)
{
    DWORD ftyp = GetFileAttributesW(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false; // something is wrong with your path!

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true; // this is a directory!

    return false; // this is not a directory!
}
#endif

int globalVariablesCount;
int globalVariables[GLOBALVAR_COUNT];
char globalVariableNames[GLOBALVAR_COUNT][0x20];

char gamePath[0x100];
int saveRAM[SAVEDATA_SIZE];
Achievement achievements[ACHIEVEMENT_COUNT];
LeaderboardEntry leaderboards[LEADERBOARD_COUNT];

#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#endif

int controlMode              = -1;
bool disableTouchControls    = false;
int disableFocusPause        = 0;
int disableFocusPause_Config = 0;

#if RETRO_USE_MOD_LOADER || !RETRO_USE_ORIGINAL_CODE
bool forceUseScripts        = false;
bool forceUseScripts_Config = false;
#endif

bool useSGame = false;

//#if RETRO_PLATFORM == RETRO_LINUX
//std::string getXDGDataPath() 
//{
  //  std::string path;
  //  char const *dataHome = getenv("XDG_DATA_HOME");
  //  if (dataHome == NULL) {
  //      char const *home = getenv("HOME");
  //      path += home;
  //      path += "/.local/share/";
  //  }
  //  else {
  //      path += dataHome;
  //  }
 //   path += "/RSDKv3";
  //  return path;
//}
//#endif

bool ReadSaveRAMData()
{
    useSGame = false;
    char buffer[0x180];

#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
  //  sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getXDGDataPath().c_str(), savePath);
#else
    sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sSData.bin", getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sSData.bin", getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
   // sprintf(buffer, "%s/%sSData.bin", getXDGDataPath().c_str(), savePath);
#else
    sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#endif
#endif

#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USE_MOD_LOADER
    if (!disableSaveIniOverride) {
#endif
        saveRAM[33] = bgmVolume;
        saveRAM[34] = sfxVolume;
#if RETRO_USE_MOD_LOADER
    }
#endif
#endif

    FileIO *saveFile = fOpen(buffer, "rb");
    if (!saveFile) {
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
    //    sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getXDGDataPath().c_str(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
  //      sprintf(buffer, "%s/%sSGame.bin", getXDGDataPath().c_str(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#endif
#endif

        saveFile = fOpen(buffer, "rb");
        if (!saveFile)
            return false;
        useSGame = true;
    }
    fRead(saveRAM, 4, SAVEDATA_SIZE, saveFile);

    fClose(saveFile);
    return true;
}

bool WriteSaveRAMData()
{
    char buffer[0x180];
    if (!useSGame) {
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
 //       sprintf(buffer, "%s/%sSData.bin", redirectSave ? modsPath : getXDGDataPath().c_str(), savePath);
#else
        sprintf(buffer, "%s%sSData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSData.bin", getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSData.bin", getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
   //     sprintf(buffer, "%s/%sSData.bin", getXDGDataPath().c_str(), savePath);
#else
        sprintf(buffer, "%s%sSData.bin", gamePath, savePath);
#endif
#endif
    }
    else {
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
  //      sprintf(buffer, "%s/%sSGame.bin", redirectSave ? modsPath : getXDGDataPath().c_str(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/%sSGame.bin", getResourcesPath(), savePath);
        else
            sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/%sSGame.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/%sSGame.bin", getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
  //      sprintf(buffer, "%s/%sSGame.bin", getXDGDataPath().c_str(), savePath);
#else
        sprintf(buffer, "%s%sSGame.bin", gamePath, savePath);
#endif
#endif
    }

    FileIO *saveFile = fOpen(buffer, "wb");
    if (!saveFile)
        return false;

#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USE_MOD_LOADER
    if (!disableSaveIniOverride) {
#endif
        saveRAM[33] = bgmVolume;
        saveRAM[34] = sfxVolume;
#if RETRO_USE_MOD_LOADER
    }
#endif
#endif

    fWrite(saveRAM, 4, SAVEDATA_SIZE, saveFile);
    fClose(saveFile);
    return true;
}

void InitUserdata()
{
    // userdata files are loaded from this directory
    sprintf(gamePath, "%s", BASE_PATH);
#if RETRO_USE_MOD_LOADER
    sprintf(modsPath, "%s", BASE_PATH);
#endif

#if RETRO_PLATFORM == RETRO_OSX
    getResourcesPath(gamePath, sizeof(gamePath));
    //sprintf(gamePath, "%s", getResourcesPath());
    sprintf(modsPath, "%s/", gamePath);

    mkdir(gamePath, 0777);
//#elif RETRO_PLATFORM == RETRO_LINUX
 //   sprintf(gamePath, "%s/", getXDGDataPath().c_str());
  //  sprintf(modsPath, "%s/", getXDGDataPath().c_str());

 //   mkdir(getXDGDataPath().c_str(), 0755);
#elif RETRO_PLATFORM == RETRO_ANDROID
    {
        char buffer[0x200];

        JNIEnv *env      = (JNIEnv *)SDL_AndroidGetJNIEnv();
        jobject activity = (jobject)SDL_AndroidGetActivity();
        jclass cls(env->GetObjectClass(activity));
        jmethodID method = env->GetMethodID(cls, "getBasePath", "()Ljava/lang/String;");
        auto ret         = env->CallObjectMethod(activity, method);

        strcpy(buffer, env->GetStringUTFChars((jstring)ret, NULL));

        sprintf(gamePath, "%s", buffer);
#if RETRO_USE_MOD_LOADER
        sprintf(modsPath, "%s", buffer);
#endif

        env->DeleteLocalRef(activity);
        env->DeleteLocalRef(cls);
    }
#endif

    char buffer[0x200];
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/settings.ini", getResourcesPath());
    else
        sprintf(buffer, "%ssettings.ini", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    sprintf(buffer, "%s/settings.ini", gamePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/settings.ini", getDocumentsPath());
//#elif RETRO_PLATFORM == RETRO_LINUX
  //  sprintf(buffer, "%s/settings.ini", getXDGDataPath().c_str());
#else
    sprintf(buffer, BASE_PATH "settings.ini");
#endif
    FileIO *file = fOpen(buffer, "rb");
    IniParser ini;
    if (!file) {
        ini.SetBool("Dev", "DevMenu", Engine.devMenu = false);
        ini.SetBool("Dev", "EngineDebugMode", engineDebugMode = false);
        ini.SetBool("Dev", "TxtScripts", forceUseScripts = false);
        forceUseScripts_Config = forceUseScripts;
        ini.SetInteger("Dev", "StartingCategory", Engine.startList = 0);
        ini.SetInteger("Dev", "StartingScene", Engine.startStage = 0);
        ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed = 8);
#if RETRO_PLATFORM == RETRO_WINDOWS
        ini.SetBool("Dev", "UseSteamDir", Engine.useSteamDir = false);
#endif
        ini.SetBool("Dev", "UseHQModes", Engine.useHQModes = true);
        sprintf(Engine.dataFile, "%s", "Data.rsdk");
        ini.SetString("Dev", "DataFile", Engine.dataFile);

        Engine.startList_Game  = Engine.startList;
        Engine.startStage_Game = Engine.startStage;

        ini.SetInteger("Game", "Language", Engine.language = RETRO_EN);
        ini.SetInteger("Game", "GameType", Engine.gameTypeID = 0);
        ini.SetInteger("Game", "OriginalControls", controlMode = -1);
        ini.SetBool("Game", "DisableTouchControls", disableTouchControls = false);
        ini.SetInteger("Game", "DisableFocusPause", disableFocusPause = 0);
        disableFocusPause_Config = disableFocusPause;

        ini.SetBool("Window", "FullScreen", Engine.startFullScreen = DEFAULT_FULLSCREEN);
        ini.SetBool("Window", "Borderless", Engine.borderless = false);
        ini.SetBool("Window", "VSync", Engine.vsync = false);
        ini.SetInteger("Window", "ScalingMode", Engine.scalingMode = 0);
        ini.SetInteger("Window", "WindowScale", Engine.windowScale = 2);
        ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE = DEFAULT_SCREEN_XSIZE);
        SCREEN_XSIZE_CONFIG = SCREEN_XSIZE;
        ini.SetInteger("Window", "RefreshRate", Engine.refreshRate = 60);
        ini.SetInteger("Window", "DimLimit", Engine.dimLimit = 300);
        Engine.dimLimit *= Engine.refreshRate;
        renderType = RENDER_SW;
        ini.SetBool("Window", "HardwareRenderer", false);

        ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
        ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

#if RETRO_USING_SDL2
        ini.SetComment("Keyboard 1", "IK1Comment",
                       "Keyboard Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDL2/SDLScancodeLookup.mediawiki)");
        ini.SetInteger("Keyboard 1", "Up", inputDevice[INPUT_UP].keyMappings = SDL_SCANCODE_UP);
        ini.SetInteger("Keyboard 1", "Down", inputDevice[INPUT_DOWN].keyMappings = SDL_SCANCODE_DOWN);
        ini.SetInteger("Keyboard 1", "Left", inputDevice[INPUT_LEFT].keyMappings = SDL_SCANCODE_LEFT);
        ini.SetInteger("Keyboard 1", "Right", inputDevice[INPUT_RIGHT].keyMappings = SDL_SCANCODE_RIGHT);
        ini.SetInteger("Keyboard 1", "A", inputDevice[INPUT_BUTTONA].keyMappings = SDL_SCANCODE_Z);
        ini.SetInteger("Keyboard 1", "B", inputDevice[INPUT_BUTTONB].keyMappings = SDL_SCANCODE_X);
        ini.SetInteger("Keyboard 1", "C", inputDevice[INPUT_BUTTONC].keyMappings = SDL_SCANCODE_C);
        ini.SetInteger("Keyboard 1", "Start", inputDevice[INPUT_START].keyMappings = SDL_SCANCODE_RETURN);

        ini.SetComment("Controller 1", "IC1Comment",
                       "Controller Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDL2/SDL_GameControllerButton.mediawiki)");
        ini.SetInteger("Controller 1", "Up", inputDevice[INPUT_UP].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP);
        ini.SetInteger("Controller 1", "Down", inputDevice[INPUT_DOWN].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        ini.SetInteger("Controller 1", "Left", inputDevice[INPUT_LEFT].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        ini.SetInteger("Controller 1", "Right", inputDevice[INPUT_RIGHT].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        ini.SetInteger("Controller 1", "A", inputDevice[INPUT_BUTTONA].contMappings = SDL_CONTROLLER_BUTTON_A);
        ini.SetInteger("Controller 1", "B", inputDevice[INPUT_BUTTONB].contMappings = SDL_CONTROLLER_BUTTON_B);
        ini.SetInteger("Controller 1", "C", inputDevice[INPUT_BUTTONC].contMappings = SDL_CONTROLLER_BUTTON_X);
        ini.SetInteger("Controller 1", "Start", inputDevice[INPUT_START].contMappings = SDL_CONTROLLER_BUTTON_START);

        ini.SetFloat("Controller 1", "LStickDeadzone", LSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RStickDeadzone", RSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "LTriggerDeadzone", LTRIGGER_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RTriggerDeadzone", RTRIGGER_DEADZONE = 0.3);
#endif

#if RETRO_USING_SDL1
        ini.SetComment("Keyboard 1", "IK1Comment", "Keyboard Mappings for P1 (Based on: https://wiki.libsdl.org/SDL_Scancode)");
        ini.SetInteger("Keyboard 1", "Up", inputDevice[INPUT_UP].keyMappings = SDLK_UP);
        ini.SetInteger("Keyboard 1", "Down", inputDevice[INPUT_DOWN].keyMappings = SDLK_DOWN);
        ini.SetInteger("Keyboard 1", "Left", inputDevice[INPUT_LEFT].keyMappings = SDLK_LEFT);
        ini.SetInteger("Keyboard 1", "Right", inputDevice[INPUT_RIGHT].keyMappings = SDLK_RIGHT);
        ini.SetInteger("Keyboard 1", "A", inputDevice[INPUT_BUTTONA].keyMappings = SDLK_z);
        ini.SetInteger("Keyboard 1", "B", inputDevice[INPUT_BUTTONB].keyMappings = SDLK_x);
        ini.SetInteger("Keyboard 1", "C", inputDevice[INPUT_BUTTONC].keyMappings = SDLK_c);
        ini.SetInteger("Keyboard 1", "Start", inputDevice[INPUT_START].keyMappings = SDLK_RETURN);

        ini.SetComment("Controller 1", "IC1Comment",
                       "Controller Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDL_GameControllerButton.mediawiki)");
        ini.SetInteger("Controller 1", "Up", inputDevice[INPUT_UP].contMappings = 1);
        ini.SetInteger("Controller 1", "Down", inputDevice[INPUT_DOWN].contMappings = 2);
        ini.SetInteger("Controller 1", "Left", inputDevice[INPUT_LEFT].contMappings = 3);
        ini.SetInteger("Controller 1", "Right", inputDevice[INPUT_RIGHT].contMappings = 4);
        ini.SetInteger("Controller 1", "A", inputDevice[INPUT_BUTTONA].contMappings = 5);
        ini.SetInteger("Controller 1", "B", inputDevice[INPUT_BUTTONB].contMappings = 6);
        ini.SetInteger("Controller 1", "C", inputDevice[INPUT_BUTTONC].contMappings = 7);
        ini.SetInteger("Controller 1", "Start", inputDevice[INPUT_START].contMappings = 8);

        ini.SetFloat("Controller 1", "LStickDeadzone", LSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RStickDeadzone", RSTICK_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "LTriggerDeadzone", LTRIGGER_DEADZONE = 0.3);
        ini.SetFloat("Controller 1", "RTriggerDeadzone", RTRIGGER_DEADZONE = 0.3);
#endif

        ini.Write(buffer, false);
    }
    else {
        fClose(file);
        ini = IniParser(buffer, false);

        if (!ini.GetBool("Dev", "DevMenu", &Engine.devMenu))
            Engine.devMenu = false;
        if (!ini.GetBool("Dev", "EngineDebugMode", &engineDebugMode))
            engineDebugMode = false;
        if (!ini.GetBool("Dev", "TxtScripts", &forceUseScripts))
            forceUseScripts = false;
        forceUseScripts_Config = forceUseScripts;
        if (!ini.GetInteger("Dev", "StartingCategory", &Engine.startList))
            Engine.startList = 0;
        if (!ini.GetInteger("Dev", "StartingScene", &Engine.startStage))
            Engine.startStage = 0;
        if (!ini.GetInteger("Dev", "FastForwardSpeed", &Engine.fastForwardSpeed))
            Engine.fastForwardSpeed = 8;
#if RETRO_PLATFORM == RETRO_WINDOWS
        if (!ini.GetBool("Dev", "UseSteamDir", &Engine.useSteamDir))
            Engine.useSteamDir = false;
#endif
        if (!ini.GetBool("Dev", "UseHQModes", &Engine.useHQModes))
            Engine.useHQModes = true;

        Engine.startList_Game  = Engine.startList;
        Engine.startStage_Game = Engine.startStage;

        if (!ini.GetString("Dev", "DataFile", Engine.dataFile))
            StrCopy(Engine.dataFile, "Data.rsdk");

        if (!ini.GetInteger("Game", "Language", &Engine.language))
            Engine.language = RETRO_EN;
        if (!ini.GetInteger("Game", "GameType", &Engine.gameTypeID))
            Engine.gameTypeID = 0;
        Engine.releaseType = Engine.gameTypeID ? "Use_Origins" : "Use_Standalone";

        if (!ini.GetInteger("Game", "OriginalControls", &controlMode))
            controlMode = -1;
        if (!ini.GetBool("Game", "DisableTouchControls", &disableTouchControls))
            disableTouchControls = false;
        if (!ini.GetInteger("Game", "DisableFocusPause", &disableFocusPause))
            disableFocusPause = 0;
        disableFocusPause_Config = disableFocusPause;

        int platype = -1;
        ini.GetInteger("Game", "Platform", &platype);
        if (platype != -1) {
            if (!platype)
                Engine.gamePlatform = "Standard";
            else if (platype == 1)
                Engine.gamePlatform = "Mobile";
        }

        if (!ini.GetBool("Window", "FullScreen", &Engine.startFullScreen))
            Engine.startFullScreen = DEFAULT_FULLSCREEN;
        if (!ini.GetBool("Window", "Borderless", &Engine.borderless))
            Engine.borderless = false;
        if (!ini.GetBool("Window", "VSync", &Engine.vsync))
            Engine.vsync = false;
        if (!ini.GetInteger("Window", "ScalingMode", &Engine.scalingMode))
            Engine.scalingMode = 0;
        if (!ini.GetInteger("Window", "WindowScale", &Engine.windowScale))
            Engine.windowScale = 2;
        if (!ini.GetInteger("Window", "ScreenWidth", &SCREEN_XSIZE))
            SCREEN_XSIZE = DEFAULT_SCREEN_XSIZE;
        SCREEN_XSIZE_CONFIG = SCREEN_XSIZE;
        if (!ini.GetInteger("Window", "RefreshRate", &Engine.refreshRate))
            Engine.refreshRate = 60;
        if (!ini.GetInteger("Window", "DimLimit", &Engine.dimLimit))
            Engine.dimLimit = 300; // 5 mins
        if (Engine.dimLimit >= 0)
            Engine.dimLimit *= Engine.refreshRate;
        bool hwRender = false;
        ini.GetBool("Window", "HardwareRenderer", &hwRender);
        if (hwRender)
            renderType = RENDER_HW;
        else
            renderType = RENDER_SW;
        Engine.gameRenderType = Engine.gameRenderTypes[renderType];

        float bv = 0, sv = 0;
        if (!ini.GetFloat("Audio", "BGMVolume", &bv))
            bv = 1.0f;
        if (!ini.GetFloat("Audio", "SFXVolume", &sv))
            sv = 1.0f;

        bgmVolume = bv * MAX_VOLUME;
        sfxVolume = sv * MAX_VOLUME;

        if (bgmVolume > MAX_VOLUME)
            bgmVolume = MAX_VOLUME;
        if (bgmVolume < 0)
            bgmVolume = 0;

        if (sfxVolume > MAX_VOLUME)
            sfxVolume = MAX_VOLUME;
        if (sfxVolume < 0)
            sfxVolume = 0;

#if RETRO_USING_SDL2
        if (!ini.GetInteger("Keyboard 1", "Up", &inputDevice[INPUT_UP].keyMappings))
            inputDevice[0].keyMappings = SDL_SCANCODE_UP;
        if (!ini.GetInteger("Keyboard 1", "Down", &inputDevice[INPUT_DOWN].keyMappings))
            inputDevice[1].keyMappings = SDL_SCANCODE_DOWN;
        if (!ini.GetInteger("Keyboard 1", "Left", &inputDevice[INPUT_LEFT].keyMappings))
            inputDevice[2].keyMappings = SDL_SCANCODE_LEFT;
        if (!ini.GetInteger("Keyboard 1", "Right", &inputDevice[INPUT_RIGHT].keyMappings))
            inputDevice[3].keyMappings = SDL_SCANCODE_RIGHT;
        if (!ini.GetInteger("Keyboard 1", "A", &inputDevice[INPUT_BUTTONA].keyMappings))
            inputDevice[4].keyMappings = SDL_SCANCODE_Z;
        if (!ini.GetInteger("Keyboard 1", "B", &inputDevice[INPUT_BUTTONB].keyMappings))
            inputDevice[5].keyMappings = SDL_SCANCODE_X;
        if (!ini.GetInteger("Keyboard 1", "C", &inputDevice[INPUT_BUTTONC].keyMappings))
            inputDevice[6].keyMappings = SDL_SCANCODE_C;
        if (!ini.GetInteger("Keyboard 1", "Start", &inputDevice[INPUT_START].keyMappings))
            inputDevice[7].keyMappings = SDL_SCANCODE_RETURN;

        if (!ini.GetInteger("Controller 1", "Up", &inputDevice[INPUT_UP].contMappings))
            inputDevice[0].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP;
        if (!ini.GetInteger("Controller 1", "Down", &inputDevice[INPUT_DOWN].contMappings))
            inputDevice[1].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        if (!ini.GetInteger("Controller 1", "Left", &inputDevice[INPUT_LEFT].contMappings))
            inputDevice[2].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        if (!ini.GetInteger("Controller 1", "Right", &inputDevice[INPUT_RIGHT].contMappings))
            inputDevice[3].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        if (!ini.GetInteger("Controller 1", "A", &inputDevice[INPUT_BUTTONA].contMappings))
            inputDevice[4].contMappings = SDL_CONTROLLER_BUTTON_A;
        if (!ini.GetInteger("Controller 1", "B", &inputDevice[INPUT_BUTTONB].contMappings))
            inputDevice[5].contMappings = SDL_CONTROLLER_BUTTON_B;
        if (!ini.GetInteger("Controller 1", "C", &inputDevice[INPUT_BUTTONC].contMappings))
            inputDevice[6].contMappings = SDL_CONTROLLER_BUTTON_X;
        if (!ini.GetInteger("Controller 1", "Start", &inputDevice[INPUT_START].contMappings))
            inputDevice[7].contMappings = SDL_CONTROLLER_BUTTON_START;

        if (!ini.GetFloat("Controller 1", "LStickDeadzone", &LSTICK_DEADZONE))
            LSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RStickDeadzone", &RSTICK_DEADZONE))
            RSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "LTriggerDeadzone", &LTRIGGER_DEADZONE))
            LTRIGGER_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RTriggerDeadzone", &RTRIGGER_DEADZONE))
            RTRIGGER_DEADZONE = 0.3;
#endif

#if RETRO_USING_SDL1
        if (!ini.GetInteger("Keyboard 1", "Up", &inputDevice[INPUT_UP].keyMappings))
            inputDevice[0].keyMappings = SDLK_UP;
        if (!ini.GetInteger("Keyboard 1", "Down", &inputDevice[INPUT_DOWN].keyMappings))
            inputDevice[1].keyMappings = SDLK_DOWN;
        if (!ini.GetInteger("Keyboard 1", "Left", &inputDevice[INPUT_LEFT].keyMappings))
            inputDevice[2].keyMappings = SDLK_LEFT;
        if (!ini.GetInteger("Keyboard 1", "Right", &inputDevice[INPUT_RIGHT].keyMappings))
            inputDevice[3].keyMappings = SDLK_RIGHT;
        if (!ini.GetInteger("Keyboard 1", "A", &inputDevice[INPUT_BUTTONA].keyMappings))
            inputDevice[4].keyMappings = SDLK_z;
        if (!ini.GetInteger("Keyboard 1", "B", &inputDevice[INPUT_BUTTONB].keyMappings))
            inputDevice[5].keyMappings = SDLK_x;
        if (!ini.GetInteger("Keyboard 1", "C", &inputDevice[INPUT_BUTTONC].keyMappings))
            inputDevice[6].keyMappings = SDLK_c;
        if (!ini.GetInteger("Keyboard 1", "Start", &inputDevice[INPUT_START].keyMappings))
            inputDevice[7].keyMappings = SDLK_RETURN;

        if (!ini.GetInteger("Controller 1", "Up", &inputDevice[INPUT_UP].contMappings))
            inputDevice[0].contMappings = 1;
        if (!ini.GetInteger("Controller 1", "Down", &inputDevice[INPUT_DOWN].contMappings))
            inputDevice[1].contMappings = 2;
        if (!ini.GetInteger("Controller 1", "Left", &inputDevice[INPUT_LEFT].contMappings))
            inputDevice[2].contMappings = 3;
        if (!ini.GetInteger("Controller 1", "Right", &inputDevice[INPUT_RIGHT].contMappings))
            inputDevice[3].contMappings = 4;
        if (!ini.GetInteger("Controller 1", "A", &inputDevice[INPUT_BUTTONA].contMappings))
            inputDevice[4].contMappings = 5;
        if (!ini.GetInteger("Controller 1", "B", &inputDevice[INPUT_BUTTONB].contMappings))
            inputDevice[5].contMappings = 6;
        if (!ini.GetInteger("Controller 1", "C", &inputDevice[INPUT_BUTTONC].contMappings))
            inputDevice[6].contMappings = 7;
        if (!ini.GetInteger("Controller 1", "Start", &inputDevice[INPUT_START].contMappings))
            inputDevice[7].contMappings = 8;

        if (!ini.GetFloat("Controller 1", "LStickDeadzone", &LSTICK_DEADZONE))
            LSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RStickDeadzone", &RSTICK_DEADZONE))
            RSTICK_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "LTriggerDeadzone", &LTRIGGER_DEADZONE))
            LTRIGGER_DEADZONE = 0.3;
        if (!ini.GetFloat("Controller 1", "RTriggerDeadzone", &RTRIGGER_DEADZONE))
            RTRIGGER_DEADZONE = 0.3;
#endif
    }

    // Loaded here so it can be disabled
#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
    if (Engine.useSteamDir) {
#if _WIN64
        LONG lRes             = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
        bool existsAndSuccess = lRes == ERROR_SUCCESS;
        std::wstring steamPath;

        if (existsAndSuccess) {
            GetStringRegKey(hKey, L"InstallPath", steamPath, L"");

            std::ifstream file(steamPath + L"/config/loginusers.vdf");
            auto root = tyti::vdf::read(file);

            std::vector<long long> SIDs;
            for (auto &child : root.childs) {
                long long sidVal = std::stoll(child.first);
                SIDs.push_back(sidVal & 0xFFFFFFFF);
            }

            for (auto &sid : SIDs) {
                std::wstring udataPath = steamPath.c_str() + std::wstring(L"/userdata/") + std::to_wstring(sid) + std::wstring(L"/200940/local/");

                if (dirExists(udataPath)) {
                    sprintf(gamePath, "%s", utf16ToUtf8(udataPath).c_str());
                    break;
                }
            }
        }

#elif _WIN32
        LONG lRes             = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_READ, &hKey);
        bool existsAndSuccess = lRes == ERROR_SUCCESS;
        std::wstring steamPath;

        if (existsAndSuccess) {
            GetStringRegKey(hKey, L"InstallPath", steamPath, L"");

            std::ifstream file(steamPath + L"/config/loginusers.vdf");
            auto root = tyti::vdf::read(file);

            std::vector<long long> SIDs;
            for (auto &child : root.childs) {
                long long sidVal = std::stoll(child.first);
                SIDs.push_back(sidVal & 0xFFFFFFFF);
            }

            for (auto &sid : SIDs) {
                std::wstring udataPath = steamPath.c_str() + std::wstring(L"/userdata/") + std::to_wstring(sid) + std::wstring(L"/200940/local/");

                if (dirExists(udataPath)) {
                    sprintf(gamePath, "%s", utf16ToUtf8(udataPath).c_str());
                    break;
                }
            }
        }
#endif
    }
#endif

    // Support for extra controller types SDL doesn't recognise
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/controllerdb.txt", getResourcesPath());
    else
        sprintf(buffer, "%scontrollerdb.txt", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    sprintf(buffer, "%s/controllerdb.txt", gamePath);
#else
    sprintf(buffer, BASE_PATH "controllerdb.txt");
#endif

#if RETRO_USING_SDL2
    file = fOpen(buffer, "rb");
    if (file) {
        fClose(file);

        int nummaps = SDL_GameControllerAddMappingsFromFile(buffer);
        if (nummaps >= 0)
            PrintLog("loaded %d controller mappings from '%s'\n", buffer, nummaps);
    }
#endif

#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/Udata.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUdata.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    sprintf(buffer, "%s/UData.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/UData.bin", getDocumentsPath());
//#elif RETRO_PLATFORM == RETRO_LINUX
   // sprintf(buffer, "%s/UData.bin", getXDGDataPath().c_str());
#else
    sprintf(buffer, "%sUdata.bin", gamePath);
#endif
    file = fOpen(buffer, "rb");
    if (file) {
        fClose(file);
        ReadUserdata();
    }
    else {
        WriteUserdata();
    }

    StrCopy(achievements[0].name, "88 Miles Per Hour");
    StrCopy(achievements[1].name, "Just One Hug is Enough");
    StrCopy(achievements[2].name, "Paradise Found");
    StrCopy(achievements[3].name, "Take the High Road");
    StrCopy(achievements[4].name, "King of the Rings");
    StrCopy(achievements[5].name, "Statue Saviour");
    StrCopy(achievements[6].name, "Heavy Metal");
    StrCopy(achievements[7].name, "All Stages Clear");
    StrCopy(achievements[8].name, "Treasure Hunter");
    StrCopy(achievements[9].name, "Dr Eggman Got Served");
    StrCopy(achievements[10].name, "Just In Time");
    StrCopy(achievements[11].name, "Saviour of the Planet");
}

void WriteSettings()
{
    IniParser ini;

    ini.SetComment("Dev", "DevMenuComment", "Enable this flag to activate dev menu via the ESC key");
    ini.SetBool("Dev", "DevMenu", Engine.devMenu);
    ini.SetComment("Dev", "DebugModeComment",
                   "Enable this flag to activate features used for debugging the engine (may result in slightly slower game speed)");
    ini.SetBool("Dev", "EngineDebugMode", engineDebugMode);
    ini.SetComment("Dev", "ScriptsComment", "Enable this flag to force the engine to load from the scripts folder instead of from bytecode");
    ini.SetBool("Dev", "TxtScripts", forceUseScripts_Config);
    ini.SetComment("Dev", "SCComment", "Sets the starting category ID");
    ini.SetInteger("Dev", "StartingCategory", Engine.startList);
    ini.SetComment("Dev", "SSComment", "Sets the starting scene ID");
    ini.SetInteger("Dev", "StartingScene", Engine.startStage);
    ini.SetComment("Dev", "FFComment", "Determines how fast the game will be when fastforwarding is active");
    ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed);
#if RETRO_PLATFORM == RETRO_WINDOWS
    ini.SetComment("Dev", "SDComment", "Determines if the game will try to use the steam directory for the game if it can locate it");
    ini.SetBool("Dev", "UseSteamDir", Engine.useSteamDir);
#endif
    ini.SetComment(
        "Dev", "UseHQComment",
        "Determines if applicable rendering modes (such as 3D floor from special stages) will render in \"High Quality\" mode or standard mode");
    ini.SetBool("Dev", "UseHQModes", Engine.useHQModes);

    ini.SetComment("Dev", "DataFileComment", "Determines what RSDK file will be loaded");
    ini.SetString("Dev", "DataFile", Engine.dataFile);

    ini.SetComment("Game", "LangComment", "Sets the game language (0 = EN, 1 = FR, 2 = IT, 3 = DE, 4 = ES, 5 = JP)");
    ini.SetInteger("Game", "Language", Engine.language);
    ini.SetComment("Game", "GameTypeComment", "Determines game type in scripts (0 = Standalone/Original releases, 1 = Origins release)");
    ini.SetInteger("Game", "GameType", Engine.gameTypeID);
    ini.SetComment("Game", "OGCtrlComment", "Sets the game's spindash style (-1 = let save file decide, 0 = S2, 1 = CD)");
    ini.SetInteger("Game", "OriginalControls", controlMode);
    ini.SetComment("Game", "DTCtrlComment", "Determines if the game should hide the touch controls UI");
    ini.SetBool("Game", "DisableTouchControls", disableTouchControls);
    ini.SetComment("Game", "DFPMenuComment",
                   "Handles pausing behaviour when focus is lost\n; 0 = Game focus enabled, engine focus enabled\n; 1 = Game focus enabled, engine focus disabled\n; 2 = Game focus disabled, engine focus disabled");
    ini.SetInteger("Game", "DisableFocusPause", disableFocusPause_Config);
    ini.SetComment("Game", "PlatformComment", "The platform type. 0 is standard (PC/Console), 1 is mobile");
    ini.SetInteger("Game", "Platform", !StrComp(Engine.gamePlatform, "Standard"));

    ini.SetComment("Window", "FSComment", "Determines if the window will be fullscreen or not");
    ini.SetBool("Window", "FullScreen", Engine.startFullScreen);
    ini.SetComment("Window", "BLComment", "Determines if the window will be borderless or not");
    ini.SetBool("Window", "Borderless", Engine.borderless);
    ini.SetComment("Window", "VSComment",
                   "Determines if VSync will be active or not (not recommended as the engine is built around running at 60 FPS)");
    ini.SetBool("Window", "VSync", Engine.vsync);
    ini.SetComment("Window", "SMComment", "Determines what scaling is used. 0 is nearest neighbour, 1 or higher is linear.");
    ini.SetInteger("Window", "ScalingMode", Engine.scalingMode);
    ini.SetComment("Window", "WSComment", "The window size multiplier");
    ini.SetInteger("Window", "WindowScale", Engine.windowScale);
    ini.SetComment("Window", "SWComment", "How wide the base screen will be in pixels");
    ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE_CONFIG);
    ini.SetComment("Window", "RRComment", "Determines the target FPS");
    ini.SetInteger("Window", "RefreshRate", Engine.refreshRate);
    ini.SetComment("Window", "DLComment", "Determines the dim timer in seconds, set to -1 to disable dimming");
    ini.SetInteger("Window", "DimLimit", Engine.dimLimit >= 0 ? Engine.dimLimit / Engine.refreshRate : -1);
    ini.SetComment("Window", "HWComment", "Determines the game uses hardware rendering (like mobile) or software rendering (like PC)");
    ini.SetBool("Window", "HardwareRenderer", renderType == RENDER_HW);

    ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
    ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

#if RETRO_USING_SDL2
    ini.SetComment("Keyboard 1", "IK1Comment",
                   "Keyboard Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDL2/SDLScancodeLookup.mediawiki)");
#endif
#if RETRO_USING_SDL1
    ini.SetComment("Keyboard 1", "IK1Comment", "Keyboard Mappings for P1 (Based on: https://wiki.libsdl.org/SDLKeycodeLookup)");
#endif
    ini.SetInteger("Keyboard 1", "Up", inputDevice[INPUT_UP].keyMappings);
    ini.SetInteger("Keyboard 1", "Down", inputDevice[INPUT_DOWN].keyMappings);
    ini.SetInteger("Keyboard 1", "Left", inputDevice[INPUT_LEFT].keyMappings);
    ini.SetInteger("Keyboard 1", "Right", inputDevice[INPUT_RIGHT].keyMappings);
    ini.SetInteger("Keyboard 1", "A", inputDevice[INPUT_BUTTONA].keyMappings);
    ini.SetInteger("Keyboard 1", "B", inputDevice[INPUT_BUTTONB].keyMappings);
    ini.SetInteger("Keyboard 1", "C", inputDevice[INPUT_BUTTONC].keyMappings);
    ini.SetInteger("Keyboard 1", "Start", inputDevice[INPUT_START].keyMappings);

#if RETRO_USING_SDL2
    ini.SetComment("Controller 1", "IC1Comment",
                   "Controller Mappings for P1 (Based on: https://github.com/libsdl-org/sdlwiki/blob/main/SDL2/SDL_GameControllerButton.mediawiki)");
    ini.SetComment("Controller 1", "IC1Comment2", "Extra buttons can be mapped with the following IDs:");
    ini.SetComment("Controller 1", "IC1Comment3", "CONTROLLER_BUTTON_ZL             = 16");
    ini.SetComment("Controller 1", "IC1Comment4", "CONTROLLER_BUTTON_ZR             = 17");
    ini.SetComment("Controller 1", "IC1Comment5", "CONTROLLER_BUTTON_LSTICK_UP      = 18");
    ini.SetComment("Controller 1", "IC1Comment6", "CONTROLLER_BUTTON_LSTICK_DOWN    = 19");
    ini.SetComment("Controller 1", "IC1Comment7", "CONTROLLER_BUTTON_LSTICK_LEFT    = 20");
    ini.SetComment("Controller 1", "IC1Comment8", "CONTROLLER_BUTTON_LSTICK_RIGHT   = 21");
    ini.SetComment("Controller 1", "IC1Comment9", "CONTROLLER_BUTTON_RSTICK_UP      = 22");
    ini.SetComment("Controller 1", "IC1Comment10", "CONTROLLER_BUTTON_RSTICK_DOWN    = 23");
    ini.SetComment("Controller 1", "IC1Comment11", "CONTROLLER_BUTTON_RSTICK_LEFT    = 24");
    ini.SetComment("Controller 1", "IC1Comment12", "CONTROLLER_BUTTON_RSTICK_RIGHT   = 25");
#endif
    ini.SetInteger("Controller 1", "Up", inputDevice[INPUT_UP].contMappings);
    ini.SetInteger("Controller 1", "Down", inputDevice[INPUT_DOWN].contMappings);
    ini.SetInteger("Controller 1", "Left", inputDevice[INPUT_LEFT].contMappings);
    ini.SetInteger("Controller 1", "Right", inputDevice[INPUT_RIGHT].contMappings);
    ini.SetInteger("Controller 1", "A", inputDevice[INPUT_BUTTONA].contMappings);
    ini.SetInteger("Controller 1", "B", inputDevice[INPUT_BUTTONB].contMappings);
    ini.SetInteger("Controller 1", "C", inputDevice[INPUT_BUTTONC].contMappings);
    ini.SetInteger("Controller 1", "Start", inputDevice[INPUT_START].contMappings);

    ini.SetComment("Controller 1", "DeadZoneComment", "Deadzones, 0.0-1.0");
    ini.SetFloat("Controller 1", "LStickDeadzone", LSTICK_DEADZONE);
    ini.SetFloat("Controller 1", "RStickDeadzone", RSTICK_DEADZONE);
    ini.SetFloat("Controller 1", "LTriggerDeadzone", LTRIGGER_DEADZONE);
    ini.SetFloat("Controller 1", "RTriggerDeadzone", RTRIGGER_DEADZONE);

    char buffer[0x200];
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/settings.ini", getResourcesPath());
    else
        sprintf(buffer, "%ssettings.ini", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    sprintf(buffer, "%s/settings.ini", gamePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/settings.ini", getDocumentsPath());
//#elif RETRO_PLATFORM == RETRO_LINUX
  //  sprintf(buffer, "%s/settings.ini", getXDGDataPath().c_str());
#else
    sprintf(buffer, BASE_PATH "settings.ini");
#endif

    ini.Write(buffer, false);
}

void ReadUserdata()
{
    char buffer[0x200];
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
  //  sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getXDGDataPath().c_str(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
  //  sprintf(buffer, "%s/%sUData.bin", getXDGDataPath().c_str(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#endif
#endif

    FileIO *userFile = fOpen(buffer, "rb");
    if (!userFile)
        return;

    int buf = 0;
    for (int a = 0; a < ACHIEVEMENT_COUNT; ++a) {
        fRead(&buf, 4, 1, userFile);
        achievements[a].status = buf;
    }
    for (int l = 0; l < LEADERBOARD_COUNT; ++l) {
        fRead(&buf, 4, 1, userFile);
        leaderboards[l].score = buf;
        if (!leaderboards[l].score)
            leaderboards[l].score = 0x7FFFFFF;
    }

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void WriteUserdata()
{
    char buffer[0x200];
#if RETRO_USE_MOD_LOADER
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
 //   sprintf(buffer, "%s/%sUData.bin", redirectSave ? modsPath : getXDGDataPath().c_str(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", redirectSave ? modsPath : gamePath, savePath);
#endif
#else
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/%sUData.bin", getResourcesPath(), savePath);
    else
        sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/%sUData.bin", gamePath, savePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/%sUData.bin", getDocumentsPath(), savePath);
//#elif RETRO_PLATFORM == RETRO_LINUX
 //   sprintf(buffer, "%s/%sUData.bin", getXDGDataPath().c_str(), savePath);
#else
    sprintf(buffer, "%s%sUData.bin", gamePath, savePath);
#endif
#endif

    FileIO *userFile = fOpen(buffer, "wb");
    if (!userFile)
        return;

    for (int a = 0; a < ACHIEVEMENT_COUNT; ++a) fWrite(&achievements[a].status, 4, 1, userFile);
    for (int l = 0; l < LEADERBOARD_COUNT; ++l) fWrite(&leaderboards[l].score, 4, 1, userFile);

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void AwardAchievement(int id, int status)
{
    if (id < 0 || id >= ACHIEVEMENT_COUNT)
        return;

    if (status != achievements[id].status)
        PrintLog("Achieved achievement: %s (%d)!", achievements[id].name, status);

    achievements[id].status = status;

    if (Engine.onlineActive) {
        // Set Achievement online
    }
    WriteUserdata();
}

void SetAchievement(int achievementID, int achievementDone)
{
    if (!Engine.trialMode && !debugMode) {
        AwardAchievement(achievementID, achievementDone);
    }
}
void SetLeaderboard(int leaderboardID, int result)
{
    if (!Engine.trialMode && !debugMode) {
        if (result < leaderboards[leaderboardID].score) {
            PrintLog("Set leaderboard (%d) value to %d", leaderboardID, result);
            leaderboards[leaderboardID].score = result;
            WriteUserdata();
        }
        else {
            PrintLog("Attempted to set leaderboard (%d) value to %d... but score was already %d!", leaderboardID, result,
                     leaderboards[leaderboardID].score);
        }
    }
}
