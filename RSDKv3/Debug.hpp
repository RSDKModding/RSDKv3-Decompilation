#ifndef DEBUG_H
#define DEBUG_H

#if RETRO_PLATFORM == RETRO_ANDROID
#include <android/log.h>
#endif

inline void PrintLog(const char *msg, ...)
{
    if (engineDebugMode) {
        char buffer[0x100];

        // make the full string
        va_list args;
        va_start(args, msg);
        vsprintf(buffer, msg, args);
        printf("%s\n", buffer);
        sprintf(buffer, "%s\n", buffer);

        char pathBuffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
#if RETRO_PLATFORM == RETRO_OSX
        {
            getResourcesPath(pathBuffer, sizeof(pathBuffer));
            sprintf(pathBuffer, "%s/log.txt", pathBuffer);
        }
#else
            sprintf(pathBuffer, "%s/log.txt", getResourcesPath());
#endif
        else
            sprintf(pathBuffer, "log.txt");
#elif RETRO_PLATFORM == RETRO_ANDROID
        sprintf(pathBuffer, "%s/log.txt", gamePath);
        __android_log_print(ANDROID_LOG_INFO, "RSDKv3", "%s", buffer);
#else
        sprintf(pathBuffer, BASE_PATH "log.txt");
#endif
        FileIO *file = fOpen(pathBuffer, "a");
        if (file) {
            fWrite(&buffer, 1, StrLength(buffer), file);
            fClose(file);
        }
    }
}

enum DevMenuMenus {
    DEVMENU_MAIN,
    DEVMENU_PLAYERSEL,
    DEVMENU_STAGELISTSEL,
    DEVMENU_STAGESEL,
    DEVMENU_SCRIPTERROR,
#if RETRO_USE_MOD_LOADER
    DEVMENU_MODMENU,
#endif
};

void InitDevMenu();
void InitErrorMessage();
void ProcessStageSelect();

#endif //! DEBUG_H
