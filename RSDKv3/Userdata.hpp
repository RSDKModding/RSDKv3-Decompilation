#ifndef USERDATA_H
#define USERDATA_H

#define GLOBALVAR_COUNT (0x100)

#define ACHIEVEMENT_MAX (0x40)
#define LEADERBOARD_MAX (0x80)

#define MOD_MAX (0x100)

#define SAVEDATA_MAX (0x2000)

enum OnlineMenuTypes {
    ONLINEMENU_ACHIEVEMENTS = 0,
    ONLINEMENU_LEADERBOARDS = 1,
};

struct Achievement {
    char name[0x40];
    int status;
};

struct LeaderboardEntry {
    int score;
};

extern int globalVariablesCount;
extern int globalVariables[GLOBALVAR_COUNT];
extern char globalVariableNames[GLOBALVAR_COUNT][0x20];

extern char gamePath[0x100];
extern int saveRAM[SAVEDATA_MAX];
extern Achievement achievements[ACHIEVEMENT_MAX];
extern LeaderboardEntry leaderboards[LEADERBOARD_MAX];

extern int controlMode;
extern bool disableTouchControls;
extern bool disableFocusPause;
extern bool disableFocusPause_Config;

#if RETRO_USE_MOD_LOADER
extern bool forceUseScripts;
extern bool forceUseScripts_Config;
#endif

inline int GetGlobalVariableByName(const char *name)
{
    for (int v = 0; v < globalVariablesCount; ++v) {
        if (StrComp(name, globalVariableNames[v]))
            return globalVariables[v];
    }
    return 0;
}

inline void SetGlobalVariableByName(const char *name, int value)
{
    for (int v = 0; v < globalVariablesCount; ++v) {
        if (StrComp(name, globalVariableNames[v])) {
            globalVariables[v] = value;
            break;
        }
    }
}

extern bool useSGame;
inline bool ReadSaveRAMData()
{
    useSGame = false;
    char buffer[0x180];
#if RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/Sdata.bin",getResourcesPath());
    else
        sprintf(buffer, "%sSdata.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
    sprintf(buffer, "%s/SData.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/SData.bin", getDocumentsPath());
#else
        sprintf(buffer, "%sSdata.bin", gamePath);
#endif

    // Temp(?)
    saveRAM[33] = bgmVolume;
    saveRAM[34] = sfxVolume;

    FileIO *saveFile = fOpen(buffer, "rb");
    if (!saveFile) {
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/sSGame.bin", getResourcesPath());
        else
            sprintf(buffer, "%sSGame.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/sSGame.bin", gamePath);
#else
        sprintf(buffer, "%sSGame.bin", gamePath);
#endif
        saveFile = fOpen(buffer, "wb");
        if (!saveFile)
            return false;
        useSGame = true;
    }
    fRead(saveRAM, 4, SAVEDATA_MAX, saveFile);

    fClose(saveFile);
    return true;
}

inline bool WriteSaveRAMData()
{
    char buffer[0x180];
    if (!useSGame) {
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/SData.bin", getResourcesPath());
        else
            sprintf(buffer, "%sSData.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/SData.bin", gamePath);
#else
        sprintf(buffer, "%sSData.bin", gamePath);
#endif
    }
    else {
#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(buffer, "%s/sSGame.bin", getResourcesPath());
        else
            sprintf(buffer, "%sSGame.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(buffer, "%s/sSGame.bin", gamePath);
#else
        sprintf(buffer, "%sSGame.bin", gamePath);
#endif
    }
    
    FileIO *saveFile = fOpen(buffer, "wb");
    if (!saveFile)
        return false;

    //Temp
    saveRAM[33] = bgmVolume;
    saveRAM[34] = sfxVolume;

    fWrite(saveRAM, 4, SAVEDATA_MAX, saveFile);
    fClose(saveFile);
    return true;
}

void InitUserdata();
void writeSettings();
void ReadUserdata();
void WriteUserdata();

void AwardAchievement(int id, int status);
void SetAchievement(int achievementID, int achievementDone);
void SetLeaderboard(int leaderboardID, int result);
inline void LoadAchievementsMenu() { ReadUserdata(); }
inline void LoadLeaderboardsMenu() { ReadUserdata(); }

#endif //!USERDATA_H
