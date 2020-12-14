#include "RetroEngine.hpp"

RetroEngine Engine = RetroEngine();

int GlobalVariablesCount;
int GlobalVariables[GLOBALVAR_COUNT];
char GlobalVariableNames[GLOBALVAR_COUNT][0x20];

bool processEvents()
{
#if RETRO_USING_SDL
    while (SDL_PollEvent(&Engine.sdlEvents)) {
        switch (Engine.sdlEvents.window.event) {
            case SDL_WINDOWEVENT_MAXIMIZED: {
                SDL_RestoreWindow(Engine.window);
                SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                Engine.isFullScreen = true;
                break;
            }
            case SDL_WINDOWEVENT_CLOSE: Engine.gameMode = ENGINE_EXITGAME; return false;
        }

        // Main Events
        switch (Engine.sdlEvents.type) {
            case SDL_CONTROLLERDEVICEADDED: controllerInit(SDL_NumJoysticks() - 1); break;
            case SDL_CONTROLLERDEVICEREMOVED: controllerClose(SDL_NumJoysticks() - 1); break;
            case SDL_WINDOWEVENT_CLOSE:
                if (Engine.window) {
                    SDL_DestroyWindow(Engine.window);
                    Engine.window = NULL;
                }
                Engine.gameMode = ENGINE_EXITGAME;
                return false;
            case SDL_APP_WILLENTERBACKGROUND: /*Engine.Callback(CALLBACK_ENTERBG);*/ break;
            case SDL_APP_WILLENTERFOREGROUND: /*Engine.Callback(CALLBACK_ENTERFG);*/ break;
            case SDL_APP_TERMINATING: Engine.gameMode = ENGINE_EXITGAME; break;
            case SDL_MOUSEMOTION:
                if (SDL_GetNumTouchFingers(SDL_GetTouchDevice(1)) <= 0) { // Touch always takes priority over mouse
                    SDL_GetMouseState(&touchX[0], &touchY[0]);
                    touchX[0] /= Engine.windowScale;
                    touchY[0] /= Engine.windowScale;
                    touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (SDL_GetNumTouchFingers(SDL_GetTouchDevice(1)) <= 0) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = 1; break;
                    }
                    touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (SDL_GetNumTouchFingers(SDL_GetTouchDevice(1)) <= 0) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = 0; break;
                    }
                    touches = 1;
                }
                break;
            case SDL_FINGERMOTION:
                touches = SDL_GetNumTouchFingers(SDL_GetTouchDevice(1));
                for (int i = 0; i < touches; i++) {
                    touchDown[i]       = true;
                    SDL_Finger *finger = SDL_GetTouchFinger(SDL_GetTouchDevice(1), i);
                    touchX[i]          = (finger->x * SCREEN_XSIZE * Engine.windowScale) / Engine.windowScale;

                    touchY[i] = (finger->y * SCREEN_YSIZE * Engine.windowScale) / Engine.windowScale;
                }
                break;
            case SDL_FINGERDOWN:
                touches = SDL_GetNumTouchFingers(SDL_GetTouchDevice(1));
                for (int i = 0; i < touches; i++) {
                    touchDown[i]       = true;
                    SDL_Finger *finger = SDL_GetTouchFinger(SDL_GetTouchDevice(1), i);
                    touchX[i]          = (finger->x * SCREEN_XSIZE * Engine.windowScale) / Engine.windowScale;

                    touchY[i] = (finger->y * SCREEN_YSIZE * Engine.windowScale) / Engine.windowScale;
                }
                break;
            case SDL_KEYDOWN:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
                    case SDLK_ESCAPE:
                        if (Engine.devMenu)
                            Engine.gameMode = ENGINE_INITDEVMENU;
                        break;
                    case SDLK_F4:
                        Engine.isFullScreen ^= 1;
                        if (Engine.isFullScreen) {
                            SDL_RestoreWindow(Engine.window);
                            SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                        else {
                            SDL_SetWindowFullscreen(Engine.window, 0);
                            SDL_SetWindowSize(Engine.window, SCREEN_XSIZE * Engine.windowScale, SCREEN_YSIZE * Engine.windowScale);
                            SDL_RestoreWindow(Engine.window);
                        }
                        break;
                    case SDLK_BACKSPACE: Engine.gameSpeed = Engine.fastForwardSpeed; break;
                    case SDLK_F11:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;
                    case SDLK_F12: Engine.masterPaused ^= 1; break;
                }
                break;
            case SDL_KEYUP:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
                    case SDLK_BACKSPACE: Engine.gameSpeed = 1; break;
                }
                break;
            case SDL_QUIT: Engine.gameMode = ENGINE_EXITGAME; return false;
        }
    }
#endif
    return true;
}

void RetroEngine::Init()
{
    CalculateTrigAngles();
    GenerateBlendLookupTable();

    CheckRSDKFile("data.rsdk");
    InitUserdata();

    gameMode = ENGINE_EXITGAME;
    LoadGameConfig("Data/Game/GameConfig.bin");
    if (InitRenderDevice()) {
        if (InitAudioPlayback()) {
            InitFirstStage();
            ClearScriptData();
            initialised = true;
            gameMode    = ENGINE_MAINGAME;
        }
    }

    running = true;
}

void RetroEngine::Run()
{
    uint frameStart, frameEnd = SDL_GetTicks();
    float frameDelta = 0.0f;

    while (running) {
        frameStart = SDL_GetTicks();
        frameDelta = frameStart - frameEnd;

        if (frameDelta > 1000.0f / (float)refreshRate) {
            frameEnd = frameStart;

            processEvents();

            for (int s = 0; s < gameSpeed; ++s) {
                ProcessInput();

                if (!masterPaused || frameStep) {
                    switch (gameMode) {
                        case ENGINE_DEVMENU: processStageSelect(); break;
                        case ENGINE_MAINGAME: ProcessStage(); break;
                        case ENGINE_INITDEVMENU:
                            LoadGameConfig("Data/Game/GameConfig.bin");
                            initDevMenu();
                            ResetCurrentStageFolder();
                            break;
                        case ENGINE_EXITGAME: running = false; break;
                        case ENGINE_SCRIPTERROR:
                            LoadGameConfig("Data/Game/GameConfig.bin");
                            initErrorMessage();
                            ResetCurrentStageFolder();
                            break;
                        case ENGINE_ENTER_HIRESMODE:
                            gameMode    = ENGINE_MAINGAME;
                            highResMode = true;
                            break;
                        case ENGINE_EXIT_HIRESMODE:
                            gameMode    = ENGINE_MAINGAME;
                            highResMode = false;
                            break;
                        case ENGINE_PAUSE: break;
                        case ENGINE_WAIT: break;
                        case ENGINE_VIDEOWAIT:
                            if (ProcessVideo() == 1)
                                gameMode = ENGINE_MAINGAME;
                            break;
                        default: break;
                    }

                    RenderRenderDevice();

                    frameStep = false;
                }
            }
        }
    }

    ReleaseAudioDevice();
    ReleaseRenderDevice();
    writeSettings();

#if RETRO_USING_SDL
    SDL_Quit();
#endif
}

void RetroEngine::LoadGameConfig(const char *filePath)
{
    FileInfo info;
    int fileBuffer  = 0;
    int fileBuffer2 = 0;
    char data[0x40];

    if (LoadFile(filePath, &info)) {
        FileRead(&fileBuffer, 1);
        FileRead(gameWindowText, fileBuffer);
        gameWindowText[fileBuffer] = 0;

        FileRead(&fileBuffer, 1);
        FileRead(&data, fileBuffer); // Load 'Data'
        data[fileBuffer] = 0;

        FileRead(&fileBuffer, 1);
        FileRead(gameDescriptionText, fileBuffer);
        gameDescriptionText[fileBuffer] = 0;

        // Read Obect Names
        int objectCount = 0;
        FileRead(&objectCount, 1);
        for (int o = 0; o < objectCount; ++o) {
            FileRead(&fileBuffer, 1);
            for (int i = 0; i < fileBuffer; ++i) FileRead(&fileBuffer2, 1);
        }

        // Read Script Paths
        for (int s = 0; s < objectCount; ++s) {
            FileRead(&fileBuffer, 1);
            for (int i = 0; i < fileBuffer; ++i) FileRead(&fileBuffer2, 1);
        }

        int varCount = 0;
        FileRead(&varCount, 1);
        GlobalVariablesCount = varCount;
        for (int v = 0; v < varCount; ++v) {
            // Read Variable Name
            FileRead(&fileBuffer, 1);
            FileRead(&GlobalVariableNames[v], fileBuffer);
            GlobalVariableNames[v][fileBuffer] = 0;

            // Read Variable Value
            FileRead(&fileBuffer2, 1);
            GlobalVariables[v] = fileBuffer2 << 24;
            FileRead(&fileBuffer2, 1);
            GlobalVariables[v] += fileBuffer2 << 16;
            FileRead(&fileBuffer2, 1);
            GlobalVariables[v] += fileBuffer2 << 8;
            FileRead(&fileBuffer2, 1);
            GlobalVariables[v] += fileBuffer2;

            if (devMenu) {
                if (StrComp("Options.DevMenuFlag", GlobalVariableNames[v]))
                    GlobalVariables[v] = 1;
            }
        }

        // Read SFX
        int sfxCount = 0;
        FileRead(&sfxCount, 1);
        for (int s = 0; s < sfxCount; ++s) {
            FileRead(&fileBuffer, 1);
            for (int i = 0; i < fileBuffer; ++i) FileRead(&fileBuffer2, 1);
        }

        // Read Player Names
        int playerCount = 0;
        FileRead(&playerCount, 1);
        for (int p = 0; p < playerCount; ++p) {
            FileRead(&fileBuffer, 1);
            for (int i = 0; i < fileBuffer; ++i) FileRead(&fileBuffer2, 1);
        }

        for (int c = 0; c < 4; ++c) {
            // Special Stages are stored as cat 2 in file, but cat 3 in game :(
            int cat = c;
            if (c == 2)
                cat = 3;
            else if (c == 3)
                cat = 2;
            stageListCount[cat] = 0;
            FileRead(&stageListCount[cat], 1);
            for (int s = 0; s < stageListCount[cat]; ++s) {

                // Read Stage Folder
                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].folder, fileBuffer);
                stageList[cat][s].folder[fileBuffer] = 0;

                // Read Stage ID
                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].id, fileBuffer);
                stageList[cat][s].id[fileBuffer] = 0;

                // Read Stage Name
                FileRead(&fileBuffer, 1);
                FileRead(&stageList[cat][s].name, fileBuffer);
                stageList[cat][s].name[fileBuffer] = 0;

                // Read Stage Mode
                FileRead(&fileBuffer, 1);
                stageList[cat][s].highlighted = fileBuffer;
            }
        }

        CloseFile();
    }
}

void RetroEngine::AwardAchievement(int id, int status)
{
    if (id < 0 || id >= ACHIEVEMENT_MAX)
        return;

    achievements[id].status = status;

    if (onlineActive) {
        // Set Achievement online
    }
    WriteUserdata();
}

void RetroEngine::SetAchievement(int achievementID, int achievementDone)
{
    if (!trialMode && !debugMode) {
        AwardAchievement(achievementID, achievementDone);
    }
}
void RetroEngine::SetLeaderboard(int leaderboardID, int result)
{
    if (!trialMode && !debugMode) {
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
void RetroEngine::LoadAchievementsMenu() { ReadUserdata(); }
void RetroEngine::LoadLeaderboardsMenu() { ReadUserdata(); }

void RetroEngine::Callback(int callbackID)
{
    int test = 0;
    switch (callbackID) {
        default:
#if RSDK_DEBUG
            printf("Callback: Unknown (%d)\n", callbackID);
#endif
            break;
        case CALLBACK_DISPLAYLOGOS: // Display Logos, Called immediately
            /*if (ActiveStageList) {
                callbackMessage = 1;
                GameMode        = 7;
            }
            else {
                callbackMessage = 10;
            }*/
#if RSDK_DEBUG
            printf("Callback: Display Logos\n");
#endif
            break;
        case CALLBACK_PRESS_START: // Called when "Press Start" is activated, PC = NONE
            /*if (ActiveStageList) {
                callbackMessage = 2;
                GameMode        = 7;
            }
            else {
                callbackMessage = 10;
            }*/
#if RSDK_DEBUG
            printf("Callback: Press Start\n");
#endif
            break;
        case CALLBACK_TIMEATTACK_NOTIFY_ENTER:
#if RSDK_DEBUG
            printf("Callback: Time Attack Notify Enter\n");
#endif
            break;
        case CALLBACK_TIMEATTACK_NOTIFY_EXIT:
#if RSDK_DEBUG
            printf("Callback: Time Attack Notify Exit\n");
#endif
            break;
        case CALLBACK_FINISHGAME_NOTIFY: // PC = NONE
#if RSDK_DEBUG
            printf("Callback: Finish Game Notify\n");
#endif
            break;
        case CALLBACK_RETURNSTORE_SELECTED: gameMode = ENGINE_EXITGAME;
#if RSDK_DEBUG
            printf("Callback: Return To Store Selected\n");
#endif
            break;
        case CALLBACK_RESTART_SELECTED:
#if RSDK_DEBUG
            printf("Callback: Restart Selected\n");
#endif
            break;
        case CALLBACK_EXIT_SELECTED: gameMode = ENGINE_EXITGAME;
#if RSDK_DEBUG
            printf("Callback: Exit Selected\n");
#endif
            break;
        case CALLBACK_BUY_FULL_GAME_SELECTED: //, Mobile = Buy Full Game Selected (Trial Mode Only)
            gameMode = ENGINE_EXITGAME;
#if RSDK_DEBUG
            printf("Callback: Buy Full Game Selected\n");
#endif
            break;
        case CALLBACK_TERMS_SELECTED: // PC = How to play, Mobile = Full Game Only Screen
            if (bytecodeMode == BYTECODE_PC) {
                for (int s = 0; s < stageListCount[STAGELIST_PRESENTATION]; ++s) {
                    if (StrComp("HELP", stageList[STAGELIST_PRESENTATION][s].name)) {
                        activeStageList   = STAGELIST_PRESENTATION;
                        stageListPosition = s;
                        stageMode         = STAGEMODE_LOAD;
                    }
                }
            }
#if RSDK_DEBUG
            printf("Callback: PC = How to play Menu, Mobile = Terms & Conditions Screen\n");
#endif
            break;
        case CALLBACK_PRIVACY_SELECTED: // PC = Controls, Mobile = Full Game Only Screen
#if RSDK_DEBUG
            printf("Callback: PC = Controls Menu, Mobile = Privacy Screen\n");
#endif
            break;
        case CALLBACK_TRIAL_ENDED:
#if RSDK_DEBUG
            printf("Callback: PC = ???, Mobile = Trial Ended Screen\n");
#endif
            break;                       // PC = ???, Mobile = Trial Ended Screen
        case CALLBACK_SETTINGS_SELECTED: // PC = Settings, Mobile = Full Game Only Screen (Trial Mode Only)
#if RSDK_DEBUG
            printf("Callback: PC = Settings, Mobile = Full Game Only Screen (Trial Mode Only)\n");
#endif
            break;
        case CALLBACK_PAUSE_REQUESTED: // PC/Mobile = Pause Requested (Mobile uses in-game menu, PC does as well if devMenu is active)
            // I know this is kinda lazy and a copout, buuuuuuut the in-game menu is so much better than the janky PC one
            stageMode = STAGEMODE_PAUSED;
            for (int o = 0; o < OBJECT_COUNT; ++o) {
                if (StrComp("PauseMenu", typeNames[o])) {
                    objectEntityList[9].type      = o;
                    objectEntityList[9].drawOrder = 6;
                    objectEntityList[9].priority  = PRIORITY_ACTIVE_PAUSED;
                    for (int s = 0; s < globalSFXCount + stageSFXCount; ++s) {
                        if (StrComp("Global/Select.wav", sfxList[s].name))
                            PlaySfx(s, 0);

                        if (StrComp("Global/Flying.wav", sfxList[s].name))
                            StopSfx(s);

                        if (StrComp("Global/Tired.wav", sfxList[s].name))
                            StopSfx(s);
                    }
                }
            }
#if RSDK_DEBUG
            printf("Callback: Pause Menu Requested\n");
#endif
            break;
        case CALLBACK_FULL_VERSION_ONLY:
#if RSDK_DEBUG
            printf("Callback: Full Version Only Notify\n");
#endif
            break;                   // PC = ???, Mobile = Full Game Only Screen
        case CALLBACK_STAFF_CREDITS: // PC = Staff Credits, Mobile = NONE
            if (bytecodeMode == BYTECODE_PC) {
                for (int s = 0; s < stageListCount[STAGELIST_PRESENTATION]; ++s) {
                    if (StrComp("CREDITS", stageList[STAGELIST_PRESENTATION][s].name)) {
                        activeStageList   = STAGELIST_PRESENTATION;
                        stageListPosition = s;
                        stageMode         = STAGEMODE_LOAD;
                    }
                }
            }
#if RSDK_DEBUG
            printf("Callback: Staff Credits Requested\n");
#endif
            break;
        case CALLBACK_16: //, PC = ??? (only when online), Mobile = NONE
#if RSDK_DEBUG
            printf("Callback: Unknown (%d)\n", callbackID);
#endif
            break;
    }
}