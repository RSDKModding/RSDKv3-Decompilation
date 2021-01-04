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
int saveRAM[SAVEDATA_MAX];
Achievement achievements[ACHIEVEMENT_MAX];
LeaderboardEntry leaderboard[LEADERBOARD_MAX];

void InitUserdata()
{
    // userdata files are loaded from this directory
    sprintf(gamePath, "");

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

    char buffer[0x200];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/settings.ini", getResourcesPath());
    else
        sprintf(buffer, "%ssettings.ini", gamePath);
#else
    sprintf(buffer, "settings.ini");
#endif
    FileIO *file = fOpen(buffer, "rb");
    IniParser ini;
    if (!file) {
        ini.SetBool("Dev", "DevMenu", Engine.devMenu = false);
        ini.SetBool("Dev", "EngineDebugMode", engineDebugMode = false);
        ini.SetInteger("Dev", "StartingCategory", Engine.startList = 0);
        ini.SetInteger("Dev", "StartingScene", Engine.startStage = 0);
        ini.SetBool("Dev", "FastForwardSpeed", Engine.fastForwardSpeed = 8);
        ini.SetBool("Dev", "UseSteamDir", Engine.useSteamDir = true);

        ini.SetBool("Game", "Language", Engine.language = RETRO_EN);

        ini.SetBool("Window", "Fullscreen", Engine.startFullScreen = false);
        ini.SetBool("Window", "Borderless", Engine.borderless = false);
        ini.SetBool("Window", "VSync", Engine.vsync = false);
        ini.SetInteger("Window", "WindowScale", Engine.windowScale = 2);
        ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE = 424);
        ini.SetInteger("Window", "RefreshRate", Engine.refreshRate = 60);

        ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
        ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

        ini.SetComment("Keyboard 1", "IK1Comment", "Keyboard Mappings for P1 (Based on: https://wiki.libsdl.org/SDL_Scancode)");
        ini.SetInteger("Keyboard 1", "Up", inputDevice[0].keyMappings = SDL_SCANCODE_UP);
        ini.SetInteger("Keyboard 1", "Down", inputDevice[1].keyMappings = SDL_SCANCODE_DOWN);
        ini.SetInteger("Keyboard 1", "Left", inputDevice[2].keyMappings = SDL_SCANCODE_LEFT);
        ini.SetInteger("Keyboard 1", "Right", inputDevice[3].keyMappings = SDL_SCANCODE_RIGHT);
        ini.SetInteger("Keyboard 1", "A", inputDevice[4].keyMappings = SDL_SCANCODE_Z);
        ini.SetInteger("Keyboard 1", "B", inputDevice[5].keyMappings = SDL_SCANCODE_X);
        ini.SetInteger("Keyboard 1", "C", inputDevice[6].keyMappings = SDL_SCANCODE_C);
        ini.SetInteger("Keyboard 1", "Start", inputDevice[7].keyMappings = SDL_SCANCODE_RETURN);

        ini.SetComment("Controller 1", "IC1Comment", "Controller Mappings for P1 (Based on: https://wiki.libsdl.org/SDL_GameControllerButton)");
        ini.SetInteger("Controller 1", "Up", inputDevice[0].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP);
        ini.SetInteger("Controller 1", "Down", inputDevice[1].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        ini.SetInteger("Controller 1", "Left", inputDevice[2].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        ini.SetInteger("Controller 1", "Right", inputDevice[3].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        ini.SetInteger("Controller 1", "A", inputDevice[4].contMappings = SDL_CONTROLLER_BUTTON_A);
        ini.SetInteger("Controller 1", "B", inputDevice[5].contMappings = SDL_CONTROLLER_BUTTON_B);
        ini.SetInteger("Controller 1", "C", inputDevice[6].contMappings = SDL_CONTROLLER_BUTTON_X);
        ini.SetInteger("Controller 1", "Start", inputDevice[7].contMappings = SDL_CONTROLLER_BUTTON_START);

        ini.Write("settings.ini");
    }
    else {
        fClose(file);
        ini = IniParser("settings.ini");

        if (!ini.GetBool("Dev", "DevMenu", &Engine.devMenu))
            Engine.devMenu = false;
        if (!ini.GetBool("Dev", "EngineDebugMode", &engineDebugMode))
            engineDebugMode = false;
        if (!ini.GetInteger("Dev", "StartingCategory", &Engine.startList))
            Engine.startList = 0;
        if (!ini.GetInteger("Dev", "StartingScene", &Engine.startStage))
            Engine.startStage = 0;
        if (!ini.GetInteger("Dev", "FastForwardSpeed", &Engine.fastForwardSpeed))
            Engine.fastForwardSpeed = 8;
        if (!ini.GetBool("Dev", "UseSteamDir", &Engine.useSteamDir))
            Engine.useSteamDir = true;

        if (!ini.GetInteger("Game", "Language", &Engine.language))
            Engine.language = RETRO_EN;

        if (!ini.GetBool("Window", "FullScreen", &Engine.startFullScreen))
            Engine.startFullScreen = false;
        if (!ini.GetBool("Window", "Borderless", &Engine.borderless))
            Engine.borderless = false;
        if (!ini.GetBool("Window", "VSync", &Engine.vsync))
            Engine.vsync = false;
        if (!ini.GetInteger("Window", "WindowScale", &Engine.windowScale))
            Engine.windowScale = 2;
        if (!ini.GetInteger("Window", "ScreenWidth", &SCREEN_XSIZE))
            SCREEN_XSIZE = 424;
        if (!ini.GetInteger("Window", "Refresh Rate", &Engine.refreshRate))
            Engine.refreshRate = 60;

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

        if (!ini.GetInteger("Keyboard 1", "Up", &inputDevice[0].keyMappings))
            inputDevice[0].keyMappings = SDL_SCANCODE_UP;
        if (!ini.GetInteger("Keyboard 1", "Down", &inputDevice[1].keyMappings))
            inputDevice[1].keyMappings = SDL_SCANCODE_DOWN;
        if (!ini.GetInteger("Keyboard 1", "Left", &inputDevice[2].keyMappings))
            inputDevice[2].keyMappings = SDL_SCANCODE_LEFT;
        if (!ini.GetInteger("Keyboard 1", "Right", &inputDevice[3].keyMappings))
            inputDevice[3].keyMappings = SDL_SCANCODE_RIGHT;
        if (!ini.GetInteger("Keyboard 1", "A", &inputDevice[4].keyMappings))
            inputDevice[4].keyMappings = SDL_SCANCODE_Z;
        if (!ini.GetInteger("Keyboard 1", "B", &inputDevice[5].keyMappings))
            inputDevice[5].keyMappings = SDL_SCANCODE_X;
        if (!ini.GetInteger("Keyboard 1", "C", &inputDevice[6].keyMappings))
            inputDevice[6].keyMappings = SDL_SCANCODE_C;
        if (!ini.GetInteger("Keyboard 1", "Start", &inputDevice[7].keyMappings))
            inputDevice[7].keyMappings = SDL_SCANCODE_RETURN;

        if (!ini.GetInteger("Controller 1", "Up", &inputDevice[0].contMappings))
            inputDevice[0].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP;
        if (!ini.GetInteger("Controller 1", "Down", &inputDevice[1].contMappings))
            inputDevice[1].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        if (!ini.GetInteger("Controller 1", "Left", &inputDevice[2].contMappings))
            inputDevice[2].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        if (!ini.GetInteger("Controller 1", "Right", &inputDevice[3].contMappings))
            inputDevice[3].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        if (!ini.GetInteger("Controller 1", "A", &inputDevice[4].contMappings))
            inputDevice[4].contMappings = SDL_CONTROLLER_BUTTON_A;
        if (!ini.GetInteger("Controller 1", "B", &inputDevice[5].contMappings))
            inputDevice[5].contMappings = SDL_CONTROLLER_BUTTON_B;
        if (!ini.GetInteger("Controller 1", "C", &inputDevice[6].contMappings))
            inputDevice[6].contMappings = SDL_CONTROLLER_BUTTON_X;
        if (!ini.GetInteger("Controller 1", "Start", &inputDevice[7].contMappings))
            inputDevice[7].contMappings = SDL_CONTROLLER_BUTTON_START;
    }
    SetScreenSize(SCREEN_XSIZE, SCREEN_YSIZE);

#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/Udata.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUdata.bin", gamePath);
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

void writeSettings() {
    IniParser ini;

    ini.SetComment("Dev", "DevMenuComment", "Enable this flag to activate dev menu via the ESC key");
    ini.SetBool("Dev", "DevMenu", Engine.devMenu);
    ini.SetComment("Dev", "DebugModeComment", "Enable this flag to activate features used for debugging the engine (may result in slightly slower game speed)");
    ini.SetBool("Dev", "EngineDebugMode", engineDebugMode);
    ini.SetComment("Dev", "SCComment", "Sets the starting category ID");
    ini.SetInteger("Dev", "StartingCategory", Engine.startList);
    ini.SetComment("Dev", "SSComment", "Sets the starting scene ID");
    ini.SetInteger("Dev", "StartingScene", Engine.startStage);
    ini.SetComment("Dev", "FFComment", "Determines how fast the game will be when fastforwarding is active");
    ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed);
    ini.SetComment("Dev", "SDComment", "Determines if the game will try to use the steam directory for the game if it can locate it (windows only)");
    ini.SetBool("Dev", "UseSteamDir", Engine.useSteamDir);

    ini.SetComment("Game", "LangComment", "Sets the game language (0 = EN, 1 = FR, 2 = IT, 3 = DE, 4 = ES, 5 = JP)");
    ini.SetInteger("Game", "Language", Engine.language);

    ini.SetComment("Window", "FSComment", "Determines if the window will be fullscreen or not");
    ini.SetBool("Window", "Fullscreen", Engine.startFullScreen);
    ini.SetComment("Window", "BLComment", "Determines if the window will be borderless or not");
    ini.SetBool("Window", "Borderless", Engine.borderless);
    ini.SetComment("Window", "VSComment", "Determines if VSync will be active or not");
    ini.SetBool("Window", "VSync", Engine.vsync);
    ini.SetComment("Window", "WSComment", "How big the window will be");
    ini.SetInteger("Window", "WindowScale", Engine.windowScale);
    ini.SetComment("Window", "SWComment", "How wide the base screen will be in pixels");
    ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE);
    ini.SetComment("Window", "RRComment", "Determines the target FPS");
    ini.SetInteger("Window", "RefreshRate", Engine.refreshRate);

    ini.SetFloat("Audio", "BGMVolume", bgmVolume / (float)MAX_VOLUME);
    ini.SetFloat("Audio", "SFXVolume", sfxVolume / (float)MAX_VOLUME);

    ini.SetComment("Keyboard 1", "IK1Comment", "Keyboard Mappings for P1 (Based on: https://wiki.libsdl.org/SDL_Scancode)");
    ini.SetInteger("Keyboard 1", "Up", inputDevice[0].keyMappings);
    ini.SetInteger("Keyboard 1", "Down", inputDevice[1].keyMappings);
    ini.SetInteger("Keyboard 1", "Left", inputDevice[2].keyMappings);
    ini.SetInteger("Keyboard 1", "Right", inputDevice[3].keyMappings);
    ini.SetInteger("Keyboard 1", "A", inputDevice[4].keyMappings);
    ini.SetInteger("Keyboard 1", "B", inputDevice[5].keyMappings);
    ini.SetInteger("Keyboard 1", "C", inputDevice[6].keyMappings);
    ini.SetInteger("Keyboard 1", "Start", inputDevice[7].keyMappings);

    ini.SetComment("Controller 1", "IC1Comment", "Controller Mappings for P1 (Based on: https://wiki.libsdl.org/SDL_GameControllerButton)");
    ini.SetInteger("Controller 1", "Up", inputDevice[0].contMappings);
    ini.SetInteger("Controller 1", "Down", inputDevice[1].contMappings);
    ini.SetInteger("Controller 1", "Left", inputDevice[2].contMappings);
    ini.SetInteger("Controller 1", "Right", inputDevice[3].contMappings);
    ini.SetInteger("Controller 1", "A", inputDevice[4].contMappings);
    ini.SetInteger("Controller 1", "B", inputDevice[5].contMappings);
    ini.SetInteger("Controller 1", "C", inputDevice[6].contMappings);
    ini.SetInteger("Controller 1", "Start", inputDevice[7].contMappings);

    ini.Write("settings.ini");
}

void ReadUserdata()
{
    char buffer[0x200];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/Udata.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUdata.bin", gamePath);
#else
    sprintf(buffer, "%sUdata.bin", gamePath);
#endif
    FileIO *userFile = fOpen(buffer, "rb");
    if (!userFile)
        return;

    int buf = 0;
    for (int a = 0; a < ACHIEVEMENT_MAX; ++a) {
        fRead(&buf, 4, 1, userFile);
        achievements[a].status = buf;
    }
    for (int l = 0; l < LEADERBOARD_MAX; ++l) {
        fRead(&buf, 4, 1, userFile);
        leaderboard[l].status = buf;
    }

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void WriteUserdata()
{
    char buffer[0x200];
#if RETRO_PLATFORM == RETRO_OSX
    if (!usingCWD)
        sprintf(buffer, "%s/Udata.bin", getResourcesPath());
    else
        sprintf(buffer, "%sUdata.bin", gamePath);
#else
    sprintf(buffer, "%sUdata.bin", gamePath);
#endif
    FileIO *userFile = fOpen(buffer, "wb");
    if (!userFile)
        return;

    for (int a = 0; a < ACHIEVEMENT_MAX; ++a) fWrite(&achievements[a].status, 4, 1, userFile);
    for (int l = 0; l < LEADERBOARD_MAX; ++l) fWrite(&leaderboard[l].status, 4, 1, userFile);

    fClose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void AwardAchievement(int id, int status)
{
    if (id < 0 || id >= ACHIEVEMENT_MAX)
        return;

        if (status != achievements[id].status)
            printLog("Achieved achievement: %s (%d)!", achievements[id].name, status);

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
        printLog("Set leaderboard (%d) value to %d", leaderboard, result);
        switch (leaderboardID) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
                leaderboard[leaderboardID].status = result;
                WriteUserdata();
                return;
        }
    }
}
