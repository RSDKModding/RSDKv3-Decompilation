#ifndef USERDATA_H
#define USERDATA_H

#define GLOBALVAR_COUNT (0x100)

#define ACHIEVEMENT_MAX (0x40)
#define LEADERBOARD_MAX (0x80)

#define MOD_MAX (0x100)

#define SAVEDATA_MAX (0x2000)

#include <string>
#include <map>
#include <unordered_map>

enum OnlineMenuTypes {
    ONLINEMENU_ACHIEVEMENTS = 0,
    ONLINEMENU_LEADERBOARDS = 1,
};

struct Achievement {
    char name[0x40];
    int status;
};

struct LeaderboardEntry {
    int status;
};

struct ModInfo {
    std::string name;
    std::string desc;
    std::string author;
    std::string version;
    std::map<std::string, std::string> fileMap;
    std::string folder;
    bool useScripts;
    bool active;
};

extern int globalVariablesCount;
extern int globalVariables[GLOBALVAR_COUNT];
extern char globalVariableNames[GLOBALVAR_COUNT][0x20];

extern char gamePath[0x100];
extern char modsPath[0x100];
extern int saveRAM[SAVEDATA_MAX];
extern Achievement achievements[ACHIEVEMENT_MAX];
extern LeaderboardEntry leaderboard[LEADERBOARD_MAX];

extern int controlMode;
extern bool disableTouchControls;

extern ModInfo modList[MOD_MAX];
extern int modCount;
extern bool forceUseScripts;

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

inline bool ReadSaveRAMData()
{
    char buffer[0x200];
#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/Sdata.bin",getResourcesPath());
    else
        sprintf(buffer, "%sSdata.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_iOS
        sprintf(buffer, "%s/SData.bin", getDocumentsPath());
#else
        sprintf(buffer, "%sSdata.bin", gamePath);
#endif

    // Temp(?)
    saveRAM[33] = bgmVolume;
    saveRAM[34] = sfxVolume;

    FileIO *saveFile = fOpen(buffer, "rb");
    if (!saveFile)
        return false;
    fRead(saveRAM, 4u, SAVEDATA_MAX, saveFile);

    fClose(saveFile);
    return true;
}

inline bool WriteSaveRAMData()
{
    char buffer[0x200];
#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_UWP
    if (!usingCWD)
        sprintf(buffer, "%s/Sdata.bin",getResourcesPath());
    else
        sprintf(buffer, "%sSdata.bin", gamePath);
#elif RETRO_PLATFORM == RETRO_iOS
    sprintf(buffer, "%s/SData.bin", getDocumentsPath());
#else
    sprintf(buffer, "%sSdata.bin", gamePath);
#endif
    
    FileIO *saveFile = fOpen(buffer, "wb");
    if (!saveFile)
        return false;

    //Temp
    saveRAM[33] = bgmVolume;
    saveRAM[34] = sfxVolume;

    fWrite(saveRAM, 4u, SAVEDATA_MAX, saveFile);
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

void initMods();
void saveMods();

#endif //!USERDATA_H
