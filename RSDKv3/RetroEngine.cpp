#include "RetroEngine.hpp"
#if RETRO_PLATFORM == RETRO_UWP
#include <winrt/base.h>
#include <winrt/Windows.Storage.h>
#endif

#if RETRO_PLATFORM == RETRO_ANDROID
#include <unistd.h>
#endif

bool usingCWD        = false;
bool engineDebugMode = false;
byte renderType      = RENDER_SW;

RetroEngine Engine = RetroEngine();

inline int GetLowerRate(int intendRate, int targetRate)
{
    int result   = 0;
    int valStore = 0;

    result = targetRate;
    if (intendRate) {
        do {
            valStore   = result % intendRate;
            result     = intendRate;
            intendRate = valStore;
        } while (valStore);
    }
    return result;
}

bool ProcessEvents()
{
#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    while (SDL_PollEvent(&Engine.sdlEvents)) {
        // Main Events
        switch (Engine.sdlEvents.type) {
#if RETRO_USING_SDL2
            case SDL_WINDOWEVENT:
                switch (Engine.sdlEvents.window.event) {
                    case SDL_WINDOWEVENT_MAXIMIZED: {
                        SDL_RestoreWindow(Engine.window);
                        SDL_SetWindowFullscreen(Engine.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        Engine.isFullScreen = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE: Engine.gameMode = ENGINE_EXITGAME; return false;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        if (!((disableFocusPause + 1) & 1))
                            Engine.message = MESSAGE_LOSTFOCUS;
                        Engine.hasFocus = false;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED: Engine.hasFocus = true; break;
                }
                break;
            case SDL_CONTROLLERDEVICEADDED: ControllerInit(Engine.sdlEvents.cdevice.which); break;
            case SDL_CONTROLLERDEVICEREMOVED: ControllerClose(Engine.sdlEvents.cdevice.which); break;
            case SDL_APP_WILLENTERBACKGROUND:
                if (!((disableFocusPause + 1) & 1))
                    Engine.message = MESSAGE_LOSTFOCUS;
                Engine.hasFocus = false;
                break;
            case SDL_APP_WILLENTERFOREGROUND: Engine.hasFocus = true; break;
            case SDL_APP_TERMINATING: Engine.gameMode = ENGINE_EXITGAME; return false;

#endif

#if defined(RETRO_USING_MOUSE) && RETRO_USING_SDL2
            case SDL_MOUSEMOTION:
                if (touches <= 1) { // Touch always takes priority over mouse
                    uint state = SDL_GetMouseState(&touchX[0], &touchY[0]);

                    int width = 0, height = 0;
                    SDL_GetWindowSize(Engine.window, &width, &height);
                    touchX[0] = ((touchX[0] - viewOffsetX) / (float)width) * SCREEN_XSIZE;
                    touchY[0] = (touchY[0] / (float)height) * SCREEN_YSIZE;

                    touchDown[0] = state & SDL_BUTTON_LMASK;
                    if (touchDown[0])
                        touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (touches <= 1) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = true; break;
                    }
                    touches = 1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (touches <= 1) { // Touch always takes priority over mouse
                    switch (Engine.sdlEvents.button.button) {
                        case SDL_BUTTON_LEFT: touchDown[0] = false; break;
                    }
                    touches = 0;
                }
                break;
#endif

#if RETRO_USING_SDL2 && defined(RETRO_USING_TOUCH)
            case SDL_FINGERMOTION:
            case SDL_FINGERDOWN:
            case SDL_FINGERUP: {
                int count = SDL_GetNumTouchFingers(Engine.sdlEvents.tfinger.touchId);
                touches   = 0;
                for (int i = 0; i < count; i++) {
                    SDL_Finger *finger = SDL_GetTouchFinger(Engine.sdlEvents.tfinger.touchId, i);
                    if (finger) {
                        touchDown[touches] = true;
                        touchX[touches]    = finger->x * SCREEN_XSIZE;
                        touchY[touches]    = finger->y * SCREEN_YSIZE;
                        touches++;
                    }
                }
                break;
            }
#endif
            case SDL_KEYDOWN:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;

                    case SDLK_ESCAPE:
                        if (Engine.devMenu) {
#if RETRO_USE_MOD_LOADER
                            // hacky patch because people can escape
                            if (Engine.gameMode == ENGINE_DEVMENU && stageMode == DEVMENU_MODMENU) {
                                RefreshEngine();
                            }
#endif

                            Engine.gameMode = ENGINE_INITDEVMENU;
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
                            stageMode       = STAGEMODE_LOAD;
                            Engine.gameMode = ENGINE_MAINGAME;
                            SetGlobalVariableByName("LampPost.Check", 0);
                            SetGlobalVariableByName("Warp.XPos", 0);
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
                            stageMode       = STAGEMODE_LOAD;
                            Engine.gameMode = ENGINE_MAINGAME;
                            SetGlobalVariableByName("LampPost.Check", 0);
                            SetGlobalVariableByName("Warp.XPos", 0);
                        }
                        break;

                    case SDLK_F4:
                        Engine.isFullScreen ^= 1;
                        SetFullScreen(Engine.isFullScreen);
                        break;

                    case SDLK_F5:
                        if (Engine.devMenu) {
                            currentStageFolder[0] = 0; // reload all assets & scripts
                            stageMode             = STAGEMODE_LOAD;
                        }
                        break;

                    case SDLK_F8:
                        if (Engine.devMenu)
                            showHitboxes ^= 2;
                        break;

                    case SDLK_F9:
                        if (Engine.devMenu)
                            showHitboxes ^= 1;
                        break;

                    case SDLK_F10:
                        if (Engine.devMenu)
                            Engine.showPaletteOverlay ^= 1;
                        break;

                    case SDLK_BACKSPACE:
                        if (Engine.devMenu)
                            Engine.gameSpeed = Engine.fastForwardSpeed;
                        break;

#if RETRO_PLATFORM == RETRO_OSX
                    case SDLK_F6:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;

                    case SDLK_F7:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#else
                    case SDLK_F11:
                    case SDLK_INSERT:
                        if (Engine.masterPaused)
                            Engine.frameStep = true;
                        break;

                    case SDLK_F12:
                    case SDLK_PAUSE:
                        if (Engine.devMenu)
                            Engine.masterPaused ^= 1;
                        break;
#endif
                }

#if RETRO_USING_SDL1
                keyState[Engine.sdlEvents.key.keysym.sym] = 1;
#endif
                break;
            case SDL_KEYUP:
                switch (Engine.sdlEvents.key.keysym.sym) {
                    default: break;
                    case SDLK_BACKSPACE: Engine.gameSpeed = 1; break;
                }
#if RETRO_USING_SDL1
                keyState[Engine.sdlEvents.key.keysym.sym] = 0;
#endif
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
    InitUserdata();
#if RETRO_USE_MOD_LOADER
    InitMods();
#endif
    char dest[0x200];
#if RETRO_PLATFORM == RETRO_UWP
    static char resourcePath[256] = { 0 };

    if (strlen(resourcePath) == 0) {
        auto folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        auto path   = to_string(folder.Path());

        std::copy(path.begin(), path.end(), resourcePath);
    }

    strcat(dest, resourcePath);
    strcat(dest, "\\");
    strcat(dest, Engine.dataFile);
#elif RETRO_PLATFORM == RETRO_ANDROID
    StrCopy(dest, gamePath);
    StrAdd(dest, Engine.dataFile);
    disableFocusPause = 0; // focus pause is ALWAYS enabled.
#else
    StrCopy(dest, BASE_PATH);
    StrAdd(dest, Engine.dataFile);
#endif
    CheckRSDKFile(dest);

    Engine.useFBTexture = Engine.scalingMode;

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

    // Calculate Skip frame
    int lower        = GetLowerRate(targetRefreshRate, refreshRate);
    renderFrameIndex = targetRefreshRate / lower;
    skipFrameIndex   = refreshRate / lower;

#if !RETRO_USE_ORIGINAL_CODE
    // "error message"
    if (!running) {
        char rootDir[0x80];
        char pathBuffer[0x80];

#if RETRO_PLATFORM == RETRO_UWP
        if (!usingCWD)
            sprintf(rootDir, "%s/", getResourcesPath());
        else
            sprintf(rootDir, "%s", "");
#elif RETRO_PLATFORM == RETRO_OSX
        sprintf(rootDir, "%s/", gamePath);
#else
        sprintf(rootDir, "%s", "");
#endif
        sprintf(pathBuffer, "%s%s", rootDir, "usage.txt");

        FileIO *f;
        if ((f = fOpen(pathBuffer, "w")) == NULL) {
            PrintLog("ERROR: Couldn't open file '%s' for writing!", "usage.txt");
            return;
        }

        char textBuf[0x100];
        sprintf(textBuf, "RETRO ENGINE v3 USAGE:\n");
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- Open the asset directory '%s' in a file browser\n", !rootDir[0] ? "./" : rootDir);
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- Place a data pack named '%s' in the asset directory\n", Engine.dataFile);
        fWrite(textBuf, 1, strlen(textBuf), f);

        sprintf(textBuf, "- OR extract a data pack and place the \"Data\" folder in the asset directory\n");
        fWrite(textBuf, 1, strlen(textBuf), f);

        fClose(f);
    }
#endif
}

void RetroEngine::Run()
{
    unsigned long long targetFreq = SDL_GetPerformanceFrequency() / Engine.refreshRate;
    unsigned long long curTicks   = 0;
    unsigned long long prevTicks  = 0;

    while (running) {
#if !RETRO_USE_ORIGINAL_CODE
        if (!vsync) {
            curTicks = SDL_GetPerformanceCounter();
            if (curTicks < prevTicks + targetFreq)
                continue;
            prevTicks = curTicks;
        }
#endif
        running = ProcessEvents();

        // Focus Checks
        if (!((disableFocusPause + 1) & 2)) {
            if (!Engine.hasFocus) {
                if (!(Engine.focusState & 1))
                    Engine.focusState = PauseSound() ? 3 : 1;
            }
            else if (Engine.focusState) {
                if ((Engine.focusState & 2))
                    ResumeSound();
                Engine.focusState = 0;
            }
        }

        if (!(Engine.focusState & 1)) {
            for (int s = 0; s < gameSpeed; ++s) {
                ProcessInput();

                if (!masterPaused || frameStep) {
                    switch (gameMode) {
                        case ENGINE_DEVMENU:
                            if (renderType == RENDER_HW) {
                                gfxIndexSize        = 0;
                                gfxVertexSize       = 0;
                                gfxIndexSizeOpaque  = 0;
                                gfxVertexSizeOpaque = 0;
                            }
                            ProcessStageSelect();
                            break;

                        case ENGINE_MAINGAME:
                            if (renderType == RENDER_HW) {
                                gfxIndexSize        = 0;
                                gfxVertexSize       = 0;
                                gfxIndexSizeOpaque  = 0;
                                gfxVertexSizeOpaque = 0;
                                vertexSize3D        = 0;
                                indexSize3D         = 0;
                                render3DEnabled     = false;
                            }
                            ProcessStage();
                            break;

                        case ENGINE_INITDEVMENU:
                            LoadGameConfig("Data/Game/GameConfig.bin");
                            InitDevMenu();
                            ResetCurrentStageFolder();
                            break;

                        case ENGINE_EXITGAME: running = false; break;

                        case ENGINE_SCRIPTERROR:
                            LoadGameConfig("Data/Game/GameConfig.bin");
                            InitErrorMessage();
                            ResetCurrentStageFolder();
                            break;

                        case ENGINE_ENTER_HIRESMODE:
                            gameMode    = ENGINE_MAINGAME;
                            highResMode = true;
                            PrintLog("Callback: HiRes Mode Enabled");
                            break;

                        case ENGINE_EXIT_HIRESMODE:
                            gameMode    = ENGINE_MAINGAME;
                            highResMode = false;
                            PrintLog("Callback: HiRes Mode Disabled");
                            break;

                        case ENGINE_PAUSE: break;
                        case ENGINE_WAIT: break;

                        case ENGINE_VIDEOWAIT:
                            if (ProcessVideo() == 1)
                                gameMode = ENGINE_MAINGAME;
                            break;

                        default: break;
                    }
                }
            }
        }

        FlipScreen();

#if RETRO_USING_OPENGL && RETRO_USING_SDL2
        SDL_GL_SwapWindow(Engine.window);
#endif
        frameStep      = false;
        Engine.message = MESSAGE_NONE;

#if RETRO_USE_HAPTICS
        int hapticID = GetHapticEffectNum();
        if (hapticID >= 0) {
            // PlayHaptics(hapticID);
        }
        else if (hapticID == HAPTIC_STOP) {
            // StopHaptics();
        }
#endif
    }

    ReleaseAudioDevice();
    StopVideoPlayback();
    ReleaseRenderDevice();
    WriteSettings();
#if RETRO_USE_MOD_LOADER
    SaveMods();
#endif

#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_Quit();
#endif
}

#if RETRO_USE_MOD_LOADER
const tinyxml2::XMLElement *FirstXMLChildElement(tinyxml2::XMLDocument *doc, const tinyxml2::XMLElement *elementPtr, const char *name)
{
    if (doc) {
        if (!elementPtr)
            return doc->FirstChildElement(name);
        else
            return elementPtr->FirstChildElement(name);
    }
    return NULL;
}

const tinyxml2::XMLElement *NextXMLSiblingElement(tinyxml2::XMLDocument *doc, const tinyxml2::XMLElement *elementPtr, const char *name)
{
    if (doc) {
        if (!elementPtr)
            return doc->NextSiblingElement(name);
        else
            return elementPtr->NextSiblingElement(name);
    }
    return NULL;
}

const tinyxml2::XMLAttribute *FindXMLAttribute(const tinyxml2::XMLElement *elementPtr, const char *name) { return elementPtr->FindAttribute(name); }
const char *GetXMLAttributeName(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->Name(); }
int GetXMLAttributeValueInt(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->IntValue(); }
bool GetXMLAttributeValueBool(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->BoolValue(); }
const char *GetXMLAttributeValueString(const tinyxml2::XMLAttribute *attributePtr) { return attributePtr->Value(); }

void RetroEngine::LoadXMLWindowText()
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement  = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *titleElement = FirstXMLChildElement(doc, gameElement, "title");
                if (titleElement) {
                    const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(titleElement, "name");
                    if (nameAttr)
                        StrCopy(gameWindowText, GetXMLAttributeValueString(nameAttr));
                }
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLVariables()
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement      = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *variablesElement = FirstXMLChildElement(doc, gameElement, "variables");
                if (variablesElement) {
                    const tinyxml2::XMLElement *varElement = FirstXMLChildElement(doc, variablesElement, "variable");
                    if (varElement) {
                        do {
                            const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(varElement, "name");
                            const char *varName                    = "unknownVariable";
                            if (nameAttr)
                                varName = GetXMLAttributeValueString(nameAttr);

                            const tinyxml2::XMLAttribute *valAttr = FindXMLAttribute(varElement, "value");
                            int varValue                          = 0;
                            if (valAttr)
                                varValue = GetXMLAttributeValueInt(valAttr);

                            StrCopy(globalVariableNames[globalVariablesCount], varName);
                            globalVariables[globalVariablesCount] = varValue;
                            globalVariablesCount++;

                        } while ((varElement = NextXMLSiblingElement(doc, varElement, "variable")));
                    }
                }
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLPalettes()
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement    = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *paletteElement = FirstXMLChildElement(doc, gameElement, "palette");
                if (paletteElement) {
                    for (const tinyxml2::XMLElement *clrElement = paletteElement->FirstChildElement("color"); clrElement;
                         clrElement                             = clrElement->NextSiblingElement("color")) {
                        const tinyxml2::XMLAttribute *bankAttr = clrElement->FindAttribute("bank");
                        int clrBank                            = 0;
                        if (bankAttr)
                            clrBank = bankAttr->IntValue();

                        const tinyxml2::XMLAttribute *indAttr = clrElement->FindAttribute("index");
                        int clrInd                            = 0;
                        if (indAttr)
                            clrInd = indAttr->IntValue();

                        const tinyxml2::XMLAttribute *rAttr = clrElement->FindAttribute("r");
                        int clrR                            = 0;
                        if (rAttr)
                            clrR = rAttr->IntValue();

                        const tinyxml2::XMLAttribute *gAttr = clrElement->FindAttribute("g");
                        int clrG                            = 0;
                        if (gAttr)
                            clrG = gAttr->IntValue();

                        const tinyxml2::XMLAttribute *bAttr = clrElement->FindAttribute("b");
                        int clrB                            = 0;
                        if (bAttr)
                            clrB = bAttr->IntValue();

                        SetPaletteEntry(clrBank, clrInd, clrR, clrG, clrB);
                    }

                    for (const tinyxml2::XMLElement *clrsElement = paletteElement->FirstChildElement("colors"); clrsElement;
                         clrsElement                             = clrsElement->NextSiblingElement("colors")) {
                        const tinyxml2::XMLAttribute *bankAttr = clrsElement->FindAttribute("bank");
                        int bank                               = 0;
                        if (bankAttr)
                            bank = bankAttr->IntValue();

                        const tinyxml2::XMLAttribute *indAttr = clrsElement->FindAttribute("start");
                        int index                             = 0;
                        if (indAttr)
                            index = indAttr->IntValue();

                        std::string text = clrsElement->GetText();
                        // working: AABBFF #FFaaFF (12, 32, 34) (145 53 234)
                        std::regex search(R"((?:#?([0-9A-F]{2})([0-9A-F]{2})([0-9A-F]{2}))|(?:\((\d+),?\s*(\d+),?\s*(\d+)\)))",
                                          std::regex_constants::icase | std::regex_constants::ECMAScript);
                        std::smatch match;
                        while (std::regex_search(text, match, search)) {
                            int r, g, b;
                            int base, start;
                            if (match[1].matched) {
                                // we have hex
                                base  = 16;
                                start = 1;
                            }
                            else {
                                // triplet
                                base  = 10;
                                start = 4;
                            }

                            r = std::stoi(match[start + 0].str(), nullptr, base);
                            g = std::stoi(match[start + 1].str(), nullptr, base);
                            b = std::stoi(match[start + 2].str(), nullptr, base);

                            SetPaletteEntry(bank, index++, r, g, b);
                            text = match.suffix();
                        }
                    }
                }
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLObjects()
{
    FileInfo info;
    modObjCount = 0;

    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement    = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *objectsElement = FirstXMLChildElement(doc, gameElement, "objects");
                if (objectsElement) {
                    const tinyxml2::XMLElement *objElement = FirstXMLChildElement(doc, objectsElement, "object");
                    if (objElement) {
                        do {
                            const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(objElement, "name");
                            const char *objName                    = "unknownObject";
                            if (nameAttr)
                                objName = GetXMLAttributeValueString(nameAttr);

                            const tinyxml2::XMLAttribute *scrAttr = FindXMLAttribute(objElement, "script");
                            const char *objScript                 = "unknownObject.txt";
                            if (scrAttr)
                                objScript = GetXMLAttributeValueString(scrAttr);

                            byte flags = 0;

                            // forces the object to be loaded, this means the object doesn't have to be and *SHOULD NOT* be in the stage object list
                            // if it is, it'll cause issues!!!!
                            const tinyxml2::XMLAttribute *loadAttr = FindXMLAttribute(objElement, "forceLoad");
                            int objForceLoad                       = false;
                            if (loadAttr)
                                objForceLoad = GetXMLAttributeValueBool(loadAttr);

                            flags |= (objForceLoad & 1);

                            StrCopy(modTypeNames[modObjCount], objName);
                            StrCopy(modScriptPaths[modObjCount], objScript);
                            modScriptFlags[modObjCount] = flags;
                            modObjCount++;

                        } while ((objElement = NextXMLSiblingElement(doc, objElement, "object")));
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLSoundFX()
{
    FileInfo info;
    FileInfo infoStore;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement   = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *soundsElement = FirstXMLChildElement(doc, gameElement, "sounds");
                if (soundsElement) {
                    const tinyxml2::XMLElement *sfxElement = FirstXMLChildElement(doc, soundsElement, "soundfx");
                    if (sfxElement) {
                        do {
                            const tinyxml2::XMLAttribute *pathAttr = FindXMLAttribute(sfxElement, "path");
                            const char *sfxPath                    = "unknownSFX.wav";
                            if (pathAttr)
                                sfxPath = GetXMLAttributeValueString(pathAttr);

                            SetSfxName(sfxPath, globalSFXCount, true);

                            GetFileInfo(&infoStore);
                            CloseFile();
                            LoadSfx((char *)sfxPath, globalSFXCount);
                            SetFileInfo(&infoStore);
                            globalSFXCount++;

                        } while ((sfxElement = NextXMLSiblingElement(doc, sfxElement, "soundfx")));
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLPlayers(TextMenu *menu)
{
    FileInfo info;

    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const tinyxml2::XMLElement *gameElement   = FirstXMLChildElement(doc, nullptr, "game");
                const tinyxml2::XMLElement *playerElement = FirstXMLChildElement(doc, gameElement, "players");
                if (playerElement) {
                    const tinyxml2::XMLElement *plrElement = FirstXMLChildElement(doc, playerElement, "player");
                    if (plrElement) {
                        do {
                            const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(plrElement, "name");
                            const char *plrName                    = "unknownPlayer";
                            if (nameAttr)
                                plrName = GetXMLAttributeValueString(nameAttr);

                            if (menu)
                                AddTextMenuEntry(menu, plrName);
                            else
                                StrCopy(playerNames[playerCount++], plrName);

                        } while ((plrElement = NextXMLSiblingElement(doc, plrElement, "player")));
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
void RetroEngine::LoadXMLStages(TextMenu *menu, int listNo)
{
    FileInfo info;
    for (int m = 0; m < (int)modList.size(); ++m) {
        if (!modList[m].active)
            continue;

        SetActiveMod(m);
        if (LoadFile("Data/Game/Game.xml", &info)) {
            tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;

            char *xmlData = new char[info.fileSize + 1];
            FileRead(xmlData, info.fileSize);
            xmlData[info.fileSize] = 0;

            bool success = doc->Parse(xmlData) == tinyxml2::XML_SUCCESS;

            if (success) {
                const char *elementNames[] = { "presentationStages", "regularStages", "bonusStages", "specialStages" };

                const tinyxml2::XMLElement *gameElement = FirstXMLChildElement(doc, nullptr, "game");
                for (int l = 0; l < STAGELIST_MAX; ++l) {
                    const tinyxml2::XMLElement *listElement = FirstXMLChildElement(doc, gameElement, elementNames[l]);
                    if (listElement) {
                        const tinyxml2::XMLElement *stgElement = FirstXMLChildElement(doc, listElement, "stage");
                        if (stgElement) {
                            do {
                                const tinyxml2::XMLAttribute *nameAttr = FindXMLAttribute(stgElement, "name");
                                const char *stgName                    = "unknownStage";
                                if (nameAttr)
                                    stgName = GetXMLAttributeValueString(nameAttr);

                                const tinyxml2::XMLAttribute *folderAttr = FindXMLAttribute(stgElement, "folder");
                                const char *stgFolder                    = "unknownStageFolder";
                                if (folderAttr)
                                    stgFolder = GetXMLAttributeValueString(folderAttr);

                                const tinyxml2::XMLAttribute *idAttr = FindXMLAttribute(stgElement, "id");
                                const char *stgID                    = "unknownStageID";
                                if (idAttr)
                                    stgID = GetXMLAttributeValueString(idAttr);

                                const tinyxml2::XMLAttribute *highlightAttr = FindXMLAttribute(stgElement, "highlight");
                                bool stgHighlighted                         = false;
                                if (highlightAttr)
                                    stgHighlighted = GetXMLAttributeValueBool(highlightAttr);

                                if (menu) {
                                    if (listNo == 3 || listNo == 4) {
                                        if ((listNo == 4 && l == 2) || (listNo == 3 && l == 3)) {
                                            AddTextMenuEntry(menu, stgName);
                                            menu->entryHighlight[menu->rowCount - 1] = stgHighlighted;
                                        }
                                    }
                                    else if (listNo == l + 1) {
                                        AddTextMenuEntry(menu, stgName);
                                        menu->entryHighlight[menu->rowCount - 1] = stgHighlighted;
                                    }
                                }
                                else {

                                    StrCopy(stageList[l][stageListCount[l]].name, stgName);
                                    StrCopy(stageList[l][stageListCount[l]].folder, stgFolder);
                                    StrCopy(stageList[l][stageListCount[l]].id, stgID);
                                    stageList[l][stageListCount[l]].highlighted = stgHighlighted;

                                    stageListCount[l]++;
                                }

                            } while ((stgElement = NextXMLSiblingElement(doc, stgElement, "stage")));
                        }
                    }
                }
            }
            else {
                PrintLog("Failed to parse game.xml File!");
            }

            delete[] xmlData;
            delete doc;

            CloseFile();
        }
    }
    SetActiveMod(-1);
}
#endif

bool RetroEngine::LoadGameConfig(const char *filePath)
{
    FileInfo info;
    byte fileBuffer  = 0;
    byte fileBuffer2 = 0;
    char data[0x40];
    char strBuffer[0x40];
    StrCopy(gameWindowText, "Retro-Engine"); // this is the default window name

    globalVariablesCount = 0;
#if RETRO_USE_MOD_LOADER
    playerCount = 0;
#endif

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

        // Read Object Names
        byte objectCount = 0;
        FileRead(&objectCount, 1);
        for (byte o = 0; o < objectCount; ++o) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        // Read Script Paths
        for (byte s = 0; s < objectCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        byte varCount = 0;
        FileRead(&varCount, 1);
        globalVariablesCount = varCount;
        for (byte v = 0; v < varCount; ++v) {
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
        }
        
        SetGlobalVariableByName("Options.DevMenuFlag", devMenu ? 1 : 0);
        SetGlobalVariableByName("Engine.PlatformId", RETRO_GAMEPLATFORMID);
        SetGlobalVariableByName("Engine.DeviceType", RETRO_GAMEPLATFORM);

        // Read SFX
        byte sfxCount = 0;
        FileRead(&sfxCount, 1);
        for (byte s = 0; s < sfxCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
        }

        // Read Player Names
        byte plrCount = 0;
        FileRead(&plrCount, 1);
        for (byte p = 0; p < plrCount; ++p) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
#if RETRO_USE_MOD_LOADER
            strBuffer[fileBuffer] = 0;
            StrCopy(playerNames[p], strBuffer);
            playerCount++;
#endif
        }

        for (int c = 0; c < 4; ++c) {
            // Special Stages are stored as cat 2 in file, but cat 3 in game :(
            int cat = c;
            if (c == 2)
                cat = 3;
            else if (c == 3)
                cat = 2;
            FileRead(&fileBuffer, 1);
            stageListCount[cat] = fileBuffer;
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

#if !RETRO_USE_ORIGINAL_CODE
#if RETRO_USE_MOD_LOADER
        if (!disableSaveIniOverride) {
#endif
            if (controlMode >= 0) {
                if (ReadSaveRAMData()) {
                    saveRAM[35] = controlMode;
                    WriteSaveRAMData();
                }
                SetGlobalVariableByName("Options.OriginalControls", controlMode);
            }
#if RETRO_USE_MOD_LOADER
        }
#endif
#endif

        CloseFile();

#if RETRO_USE_MOD_LOADER
        LoadXMLWindowText();
        LoadXMLVariables();
        LoadXMLPalettes();
        LoadXMLObjects();
        LoadXMLPlayers(NULL);
        LoadXMLStages(NULL, 0);

        SetGlobalVariableByName("Engine.Standalone", 1);
#endif

        SetGlobalVariableByName("game.hasPlusDLC", !RSDK_AUTOBUILD);

#if !RETRO_USE_ORIGINAL_CODE
        if (strlen(Engine.startSceneFolder) && strlen(Engine.startSceneID)) {
            SceneInfo *scene = &stageList[STAGELIST_BONUS][0xFE]; // slot 0xFF is used for "none" startStage
            strcpy(scene->name, "_RSDK_SCENE");
            strcpy(scene->folder, Engine.startSceneFolder);
            strcpy(scene->id, Engine.startSceneID);
            startList_Game  = STAGELIST_BONUS;
            startStage_Game = 0xFE;
        }
#endif

        return true;
    }

    return false;
}

void RetroEngine::Callback(int callbackID)
{
    // Sonic Origins Params
    int notifyParam1 = GetGlobalVariableByName("game.callbackParam0");
    int notifyParam2 = GetGlobalVariableByName("game.callbackParam1");
    int notifyParam3 = GetGlobalVariableByName("game.callbackParam2");

    switch (callbackID) {
        default: PrintLog("Callback: Unknown (%d)", callbackID); break;
        case CALLBACK_DISPLAYLOGOS: // Display Logos, Called immediately
            /*if (ActiveStageList) {
                callbackMessage = 1;
                GameMode        = 7;
            }
            else {
                callbackMessage = 10;
            }*/
            PrintLog("Callback: Display Logos");
            break;
        case CALLBACK_PRESS_START: // Called when "Press Start" is activated, PC = NONE
            /*if (ActiveStageList) {
                callbackMessage = 2;
                GameMode        = 7;
            }
            else {
                callbackMessage = 10;
            }*/
            PrintLog("Callback: Press Start");
            break;
        case CALLBACK_TIMEATTACK_NOTIFY_ENTER: PrintLog("Callback: Time Attack Notify Enter"); break;
        case CALLBACK_TIMEATTACK_NOTIFY_EXIT: PrintLog("Callback: Time Attack Notify Exit"); break;
        case CALLBACK_FINISHGAME_NOTIFY: // PC = NONE
            PrintLog("Callback: Finish Game Notify");
            break;
        case CALLBACK_RETURNSTORE_SELECTED:
            gameMode = ENGINE_EXITGAME;
            PrintLog("Callback: Return To Store Selected");
            break;
        case CALLBACK_RESTART_SELECTED:
            PrintLog("Callback: Restart Selected");
            stageMode = STAGEMODE_LOAD;
            break;
        case CALLBACK_EXIT_SELECTED:
            // gameMode = ENGINE_EXITGAME;
            PrintLog("Callback: Exit Selected");
            if (bytecodeMode == BYTECODE_PC) {
                running = false;
            }
            else {
                activeStageList   = 0;
                stageListPosition = 0;
                stageMode         = STAGEMODE_LOAD;
            }
            break;
        case CALLBACK_BUY_FULL_GAME_SELECTED: //, Mobile = Buy Full Game Selected (Trial Mode Only)
            gameMode = ENGINE_EXITGAME;
            PrintLog("Callback: Buy Full Game Selected");
            break;
        case CALLBACK_TERMS_SELECTED: // PC = How to play, Mobile = Full Game Only Screen
            // PC doesn't have hi res mode
            /*if (bytecodeMode == BYTECODE_PC) {
                for (int s = 0; s < stageListCount[STAGELIST_PRESENTATION]; ++s) {
                    if (StrComp("HELP", stageList[STAGELIST_PRESENTATION][s].name)) {
                        activeStageList   = STAGELIST_PRESENTATION;
                        stageListPosition = s;
                        stageMode         = STAGEMODE_LOAD;
                    }
                }
            }*/
            PrintLog("Callback: PC = How to play Menu, Mobile = Terms & Conditions Screen");
            break;
        case CALLBACK_PRIVACY_SELECTED: // PC = Controls, Mobile = Full Game Only Screen
            PrintLog("Callback: PC = Controls Menu, Mobile = Privacy Screen");
            break;
        case CALLBACK_TRIAL_ENDED:
            if (bytecodeMode == BYTECODE_PC) {
                PrintLog("Callback: ???");
            }
            else {
                if (Engine.trialMode) {
                    PrintLog("Callback: Trial Ended Screen Requested");
                }
                else {
                    // Go to this URL http://www.sega.com
                    PrintLog("Callback: Sega Website Requested");
                }
            }
            break;                       // PC = ???, Mobile = Trial Ended Screen
        case CALLBACK_SETTINGS_SELECTED: // PC = Settings, Mobile = Full Game Only Screen (Trial Mode Only)
            if (bytecodeMode == BYTECODE_PC) {
                PrintLog("Callback: Settings Requested");
            }
            else {
                if (Engine.trialMode) {
                    PrintLog("Callback: Full Game Only Requested");
                }
                else {
                    // Go to this URL http://www.sega.com/legal/terms_mobile.php
                    PrintLog("Callback: Terms Requested");
                }
            }
            break;
        case CALLBACK_PAUSE_REQUESTED: // PC/Mobile = Pause Requested (Mobile uses in-game menu, PC does as well if devMenu is active)
            // I know this is kinda lazy and a copout, buuuuuuut the in-game menu is so much better than the janky PC one
            stageMode = STAGEMODE_PAUSED;
            PauseSound();
            for (int o = 0; o < OBJECT_COUNT; ++o) {
                if (StrComp("PauseMenu", typeNames[o])) {
                    objectEntityList[9].type      = o;
                    objectEntityList[9].drawOrder = 6;
                    objectEntityList[9].priority  = PRIORITY_ALWAYS;
                    if (activeStageList == STAGELIST_SPECIAL)
                        stageLayouts[0].type = LAYER_3DFLOOR;
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
            PrintLog("Callback: Pause Menu Requested");
            break;
        case CALLBACK_FULL_VERSION_ONLY: PrintLog("Callback: Full Version Only Notify"); break; // PC = ???, Mobile = Full Game Only Screen
        case CALLBACK_STAFF_CREDITS: // PC = Staff Credits, Mobile = Privacy
            if (bytecodeMode == BYTECODE_PC) {
                for (int s = 0; s < stageListCount[STAGELIST_PRESENTATION]; ++s) {
                    if (StrComp("CREDITS", stageList[STAGELIST_PRESENTATION][s].name)) {
                        activeStageList   = STAGELIST_PRESENTATION;
                        stageListPosition = s;
                        stageMode         = STAGEMODE_LOAD;
                    }
                }
                PrintLog("Callback: Staff Credits Requested");
            }
            else {
                // Go to this URL http://www.sega.com/legal/privacy_mobile.php
                PrintLog("Callback: Privacy Requested");
            }
            break;
        case CALLBACK_MOREGAMES: //, PC = ??? (only when online), Mobile = Show More Games
            PrintLog("Callback: Show More Games");
            break;

        case CALLBACK_SHOWREMOVEADS: //, PC = ??? (only when online), Mobile = Remove Ads
            PrintLog("Callback: Show Remove Ads");
            break;

        case CALLBACK_AGEGATE:
            PrintLog("Callback: Age Gate");
            // Newer versions of the game wont continue without this
            // Thanks to Sappharad for pointing this out
            SetGlobalVariableByName("HaveLoadAllGDPRValue", 1);
            break;

        // Sonic Origins
        case NOTIFY_DEATH_EVENT: PrintLog("NOTIFY: DeathEvent() -> %d", notifyParam1); break;
        case NOTIFY_TOUCH_SIGNPOST: PrintLog("NOTIFY: TouchSignPost() -> %d", notifyParam1); break;
        case NOTIFY_HUD_ENABLE: PrintLog("NOTIFY: HUDEnable() -> %d", notifyParam1); break;
        case NOTIFY_ADD_COIN:
            PrintLog("NOTIFY: AddCoin() -> %d", notifyParam1);
            SetGlobalVariableByName("game.coinCount", GetGlobalVariableByName("game.coinCount") + notifyParam1);
            break;
        case NOTIFY_KILL_ENEMY: PrintLog("NOTIFY: KillEnemy() -> %d", notifyParam1); break;
        case NOTIFY_SAVESLOT_SELECT: PrintLog("NOTIFY: SaveSlotSelect() -> %d", notifyParam1); break;
        case NOTIFY_FUTURE_PAST:
            PrintLog("NOTIFY: FuturePast() -> %d", notifyParam1);
            objectEntityList[objectLoop].state++;
            break;
        case NOTIFY_GOTO_FUTURE_PAST: PrintLog("NOTIFY: GotoFuturePast() -> %d", notifyParam1); break;
        case NOTIFY_BOSS_END: PrintLog("NOTIFY: BossEnd() -> %d", notifyParam1); break;
        case NOTIFY_SPECIAL_END: PrintLog("NOTIFY: SpecialEnd() -> %d", notifyParam1); break;
        case NOTIFY_DEBUGPRINT: PrintLog("NOTIFY: DebugPrint() -> %d, %d, %d", notifyParam1, notifyParam2, notifyParam3); break;
        case NOTIFY_KILL_BOSS: PrintLog("NOTIFY: KillBoss() -> %d", notifyParam1); break;
        case NOTIFY_TOUCH_EMERALD: PrintLog("NOTIFY: TouchEmerald() -> %d", notifyParam1); break;
        case NOTIFY_STATS_ENEMY: PrintLog("NOTIFY: StatsEnemy() -> %d, %d, %d", notifyParam1, notifyParam2, notifyParam3); break;
        case NOTIFY_STATS_CHARA_ACTION: PrintLog("NOTIFY: StatsCharaAction() -> %d, %d, %d", notifyParam1, notifyParam2, notifyParam3); break;
        case NOTIFY_STATS_RING: PrintLog("NOTIFY: StatsRing() -> %d", notifyParam1); break;
        case NOTIFY_STATS_MOVIE:
            PrintLog("NOTIFY: StatsMovie() -> %d", notifyParam1);
            ClearGraphicsData();
            ClearAnimationData();
            LoadPalette("MasterPalette.act", 0, 0, 0, 256);
#if RETRO_USE_MOD_LOADER
            Engine.LoadXMLPalettes();
#endif
            activeStageList   = 0;
            stageMode         = STAGEMODE_LOAD;
            Engine.gameMode   = ENGINE_MAINGAME;
            stageListPosition = 0;
            break;
        case NOTIFY_STATS_PARAM_1: PrintLog("NOTIFY: StatsParam1() -> %d, %d, %d", notifyParam1, notifyParam2, notifyParam3); break;
        case NOTIFY_STATS_PARAM_2: PrintLog("NOTIFY: StatsParam2() -> %d", notifyParam1); break;
        case NOTIFY_CHARACTER_SELECT:
            PrintLog("NOTIFY: CharacterSelect() -> %d", notifyParam1);
            SetGlobalVariableByName("game.callbackResult", 1);
            SetGlobalVariableByName("game.continueFlag", 0);
            break;
        case NOTIFY_SPECIAL_RETRY:
            PrintLog("NOTIFY: SpecialRetry() -> %d, %d, %d", notifyParam1, notifyParam2, notifyParam3);
            SetGlobalVariableByName("game.callbackResult", 1);
            break;
        case NOTIFY_TOUCH_CHECKPOINT: PrintLog("NOTIFY: TouchCheckpoint() -> %d", notifyParam1); break;
        case NOTIFY_ACT_FINISH: PrintLog("NOTIFY: ActFinish() -> %d", notifyParam1); break;
        case NOTIFY_1P_VS_SELECT: PrintLog("NOTIFY: 1PVSSelect() -> %d", notifyParam1); break;
        case NOTIFY_CONTROLLER_SUPPORT:
            PrintLog("NOTIFY: ControllerSupport() -> %d", notifyParam1);
            SetGlobalVariableByName("game.callbackResult", 1);
            break;
        case NOTIFY_STAGE_RETRY: PrintLog("NOTIFY: StageRetry() -> %d, %d, %d", notifyParam1, notifyParam2, notifyParam3); break;
        case NOTIFY_SOUND_TRACK: PrintLog("NOTIFY: SoundTrack() -> %d", notifyParam1); break;
        case NOTIFY_GOOD_ENDING: PrintLog("NOTIFY: GoodEnding() -> %d", notifyParam1); break;
        case NOTIFY_BACK_TO_MAINMENU: PrintLog("NOTIFY: BackToMainMenu() -> %d", notifyParam1); break;
        case NOTIFY_LEVEL_SELECT_MENU: PrintLog("NOTIFY: LevelSelectMenu() -> %d", notifyParam1); break;
        case NOTIFY_PLAYER_SET: PrintLog("NOTIFY: PlayerSet() -> %d", notifyParam1); break;
        case NOTIFY_EXTRAS_MODE: PrintLog("NOTIFY: ExtrasMode() -> %d", notifyParam1); break;
        case NOTIFY_SPIN_DASH_TYPE: PrintLog("NOTIFY: SpindashType() -> %d", notifyParam1); break;
        case NOTIFY_TIME_OVER: PrintLog("NOTIFY: TimeOver() -> %d", notifyParam1); break;
        case NOTIFY_TIMEATTACK_MODE: PrintLog("NOTIFY: TimeAttackMode() -> %d", notifyParam1); break;
        case NOTIFY_STATS_BREAK_OBJECT: PrintLog("NOTIFY: StatsBreakObject() -> %d, %d", notifyParam1, notifyParam2); break;
        case NOTIFY_STATS_SAVE_FUTURE: PrintLog("NOTIFY: StatsSaveFuture() -> %d", notifyParam1); break;
        case NOTIFY_STATS_CHARA_ACTION2: PrintLog("NOTIFY: StatsCharaAction2() -> %d, %d, %d", notifyParam1, notifyParam2, notifyParam3); break;

        // Sega Forever stuff
        case CALLBACK_SHOWMENU_2: PrintLog("Callback: showMenu(2)"); break;
        case CALLBACK_SHOWHELPCENTER: PrintLog("Callback: Show Help Center"); break;
        case CALLBACK_CHANGEADSTYPE: PrintLog("Callback: Change Ads Type"); break;
        case CALLBACK_ONSHOWINTERSTITIAL: PrintLog("Callback: onShowInterstitial(2, 0) - Pause_Duration"); break;
        case CALLBACK_ONSHOWBANNER: PrintLog("Callback: onShowBanner()"); break;
        case CALLBACK_ONSHOWBANNER_PAUSESTART: PrintLog("Callback: onShowBanner() - Pause_Start"); break;
        case CALLBACK_ONHIDEBANNER: PrintLog("Callback: onHideBanner()"); break;
        case CALLBACK_REMOVEADSBUTTON_FADEOUT: PrintLog("Callback: RemoveAdsButton_FadeOut()"); break;
        case CALLBACK_REMOVEADSBUTTON_FADEIN: PrintLog("Callback: RemoveAdsButton_FadeIn()"); break;
        case CALLBACK_ONSHOWINTERSTITIAL_2:
        case CALLBACK_ONSHOWINTERSTITIAL_3:
        case CALLBACK_ONSHOWINTERSTITIAL_5: PrintLog("Callback: onShowInterstitial(0, 0)"); break;
        case CALLBACK_ONSHOWINTERSTITIAL_4: PrintLog("Callback: onShowInterstitial(1, 0)"); break;
        case CALLBACK_ONVISIBLEGRIDBTN_1: PrintLog("Callback: onVisibleGridBtn(1)"); break;
        case CALLBACK_ONVISIBLEGRIDBTN_0:
            PrintLog("Callback: onVisibleGridBtn(0)");

            // small hack here since the game enables hiRes and forgets to disable it????
            if (Engine.highResMode)
                Engine.highResMode = false;
            break;
        case CALLBACK_ONSHOWINTERSTITIAL_PAUSEDURATION: PrintLog("Callback: onShowInterstitial(0, 0) - Pause_Duration"); break;
        case CALLBACK_SHOWCOUNTDOWNMENU: PrintLog("Callback: showCountDownMenu(0)"); break;
        case CALLBACK_ONVISIBLEMAINMENU_1: PrintLog("Callback: onVisibleMainMenu(1)"); break;
        case CALLBACK_ONVISIBLEMAINMENU_0: PrintLog("Callback: OnVisibleMainMenu(0)"); break;
        case CALLBACK_ONSHOWREWARDADS:
            PrintLog("Callback: onShowRewardAds(0)");

            // small hack to prevent a softlock
            SetGlobalVariableByName("RewardAdCallback", 1);
            break;
        case CALLBACK_ONSHOWBANNER_2: PrintLog("Callback: onShowBanner(4, 0)"); break;

            // Mod loader Only
#if RETRO_USE_MOD_LOADER
        case CALLBACK_SET1P: activePlayerCount = 1; break;
        case CALLBACK_SET2P: activePlayerCount = 2; break;
#endif
    }
}
