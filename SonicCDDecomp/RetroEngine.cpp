#include "RetroEngine.hpp"

bool usingCWD = false;
bool engineDebugMode = false;

RetroEngine Engine = RetroEngine();

bool processEvents()
{
#if RETRO_USING_SDL
    while (SDL_PollEvent(&Engine.sdlEvents)) {
        // Main Events
        switch (Engine.sdlEvents.type) {
            case SDL_WINDOWEVENT:
                switch (Engine.sdlEvents.window.event) {
                    case SDL_WINDOWEVENT_MAXIMIZED: {
                        SDL_RestoreWindow(Engine.window);
                        SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        Engine.isFullScreen = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE: Engine.gameMode = ENGINE_EXITGAME; return false;
                }
            break;
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
                    case SDLK_F1:
                        if (Engine.devMenu) {
                            activeStageList   = 0;
                            stageListPosition = 0;
                            stageMode         = STAGEMODE_LOAD;
                            Engine.gameMode   = ENGINE_MAINGAME;
                        }
                        break;
                    case SDLK_F2:
                        if (Engine.devMenu) {
                            stageListPosition--;
                            if (stageListPosition < 0) {
                                activeStageList--;

                                if (activeStageList < 0) {
                                    activeStageList = 3;
                                }
                                stageListPosition = stageListCount[activeStageList] - 1;
                            }
                            stageMode         = STAGEMODE_LOAD;
                            Engine.gameMode   = ENGINE_MAINGAME;
                        }
                        break;
                    case SDLK_F3:
                        if (Engine.devMenu) {
                            stageListPosition++;
                            if (stageListPosition >= stageListCount[activeStageList]) {
                                activeStageList++;

                                stageListPosition = 0;

                                if (activeStageList >= 4) {
                                    activeStageList = 0;
                                }
                            }
                            stageMode         = STAGEMODE_LOAD;
                            Engine.gameMode   = ENGINE_MAINGAME;
                        }
                        break;
                    case SDLK_F10:
                        if (Engine.devMenu)
                            Engine.showPaletteOverlay ^= 1;
                        break;
#if RETRO_PLATFORM == RETRO_OSX
                    case SDLK_TAB:
                        if (Engine.devMenu)
                            Engine.gameSpeed = Engine.fastForwardSpeed;
                        break;
                    case SDLK_F6:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;
                    case SDLK_F7:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#else
                    case SDLK_BACKSPACE:
                        if (Engine.devMenu)
                            Engine.gameSpeed = Engine.fastForwardSpeed;
                        break;
                    case SDLK_F11:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;
                    case SDLK_F12:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#endif
                }
                break;
            case SDL_KEYUP:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
#if RETRO_PLATFORM == RETRO_OSX
                    case SDLK_TAB: Engine.gameSpeed = 1; break;
#else
                    case SDLK_BACKSPACE: Engine.gameSpeed = 1; break;
#endif
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
    running  = false;
    if (LoadGameConfig("Data/Game/GameConfig.bin")) {
        if (InitRenderDevice()) {
            if (InitAudioPlayback()) {
                InitFirstStage();
                ClearScriptData();
                initialised = true;
                running     = true;
                gameMode    = ENGINE_MAINGAME;
            }
        }
    }

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

            running = processEvents();

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

bool RetroEngine::LoadGameConfig(const char *filePath)
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
        globalVariablesCount = varCount;
        for (int v = 0; v < varCount; ++v) {
            // Read Variable Name
            FileRead(&fileBuffer, 1);
            FileRead(&globalVariableNames[v], fileBuffer);
            globalVariableNames[v][fileBuffer] = 0;

            // Read Variable Value
            FileRead(&fileBuffer2, 1);
            globalVariables[v] = fileBuffer2 << 24;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2 << 16;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2 << 8;
            FileRead(&fileBuffer2, 1);
            globalVariables[v] += fileBuffer2;

            if (devMenu) {
                if (StrComp("Options.DevMenuFlag", globalVariableNames[v]))
                    globalVariables[v] = 1;
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
        return true;
    }
    return false;
}

void RetroEngine::Callback(int callbackID)
{
    switch (callbackID) {
        default:
            printLog("Callback: Unknown (%d)", callbackID);
            break;
        case CALLBACK_DISPLAYLOGOS: // Display Logos, Called immediately
            /*if (ActiveStageList) {
                callbackMessage = 1;
                GameMode        = 7;
            }
            else {
                callbackMessage = 10;
            }*/
            printLog("Callback: Display Logos");
            break;
        case CALLBACK_PRESS_START: // Called when "Press Start" is activated, PC = NONE
            /*if (ActiveStageList) {
                callbackMessage = 2;
                GameMode        = 7;
            }
            else {
                callbackMessage = 10;
            }*/
            printLog("Callback: Press Start");
            break;
        case CALLBACK_TIMEATTACK_NOTIFY_ENTER:
            printLog("Callback: Time Attack Notify Enter");
            break;
        case CALLBACK_TIMEATTACK_NOTIFY_EXIT:
            printLog("Callback: Time Attack Notify Exit");
            break;
        case CALLBACK_FINISHGAME_NOTIFY: // PC = NONE
            printLog("Callback: Finish Game Notify");
            break;
        case CALLBACK_RETURNSTORE_SELECTED: gameMode = ENGINE_EXITGAME;
            printLog("Callback: Return To Store Selected");
            break;
        case CALLBACK_RESTART_SELECTED:
            printLog("Callback: Restart Selected");
            break;
        case CALLBACK_EXIT_SELECTED: gameMode = ENGINE_EXITGAME;
            printLog("Callback: Exit Selected");
            break;
        case CALLBACK_BUY_FULL_GAME_SELECTED: //, Mobile = Buy Full Game Selected (Trial Mode Only)
            gameMode = ENGINE_EXITGAME;
            printLog("Callback: Buy Full Game Selected");
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
            printLog("Callback: PC = How to play Menu, Mobile = Terms & Conditions Screen");
            break;
        case CALLBACK_PRIVACY_SELECTED: // PC = Controls, Mobile = Full Game Only Screen
            printLog("Callback: PC = Controls Menu, Mobile = Privacy Screen");
            break;
        case CALLBACK_TRIAL_ENDED:
            printLog("Callback: PC = ???, Mobile = Trial Ended Screen");
            break;                       // PC = ???, Mobile = Trial Ended Screen
        case CALLBACK_SETTINGS_SELECTED: // PC = Settings, Mobile = Full Game Only Screen (Trial Mode Only)
            printLog("Callback: PC = Settings, Mobile = Full Game Only Screen (Trial Mode Only)");
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
            printLog("Callback: Pause Menu Requested");
            break;
        case CALLBACK_FULL_VERSION_ONLY:
            printLog("Callback: Full Version Only Notify");
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
            printLog("Callback: Staff Credits Requested");
            break;
        case CALLBACK_16: //, PC = ??? (only when online), Mobile = NONE
            printLog("Callback: Unknown (%d)", callbackID);
            break;
        case CALLBACK_AGEGATE: 
            //Newer versions of the game wont continue without this
            //Thanks to Sappharad for pointing this out
            globalVariables[135] = 1;
            break;
    }
}
