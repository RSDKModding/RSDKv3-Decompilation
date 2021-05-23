#include "RetroEngine.hpp"
#include <stdlib.h>

int stageListCount[STAGELIST_MAX];
char stageListNames[STAGELIST_MAX][0x20] = {
    "Presentation Stages",
    "Regular Stages",
    "Bonus Stages",
    "Special Stages",
};
SceneInfo stageList[STAGELIST_MAX][0x100];

int stageMode = STAGEMODE_LOAD;

int cameraTarget   = -1;
int cameraStyle    = 0;
int cameraEnabled  = 0;
int cameraAdjustY  = 0;
int xScrollOffset  = 0;
int yScrollOffset  = 0;
int yScrollA       = 0;
int yScrollB       = SCREEN_YSIZE;
int xScrollA       = 0;
int xScrollB       = SCREEN_XSIZE;
int yScrollMove    = 0;
int cameraShakeX   = 0;
int cameraShakeY   = 0;
int cameraLag      = 0;
int cameraLagStyle = 0;

int xBoundary1    = 0;
int newXBoundary1 = 0;
int yBoundary1    = 0;
int newYBoundary1 = 0;
int xBoundary2    = 0;
int yBoundary2    = 0;
int waterLevel    = 0;
int waterDrawPos  = 0;
int newXBoundary2 = 0;
int newYBoundary2 = 0;

int SCREEN_SCROLL_LEFT  = SCREEN_CENTERX - 8;
int SCREEN_SCROLL_RIGHT = SCREEN_CENTERX + 8;

int lastYSize = -1;
int lastXSize = -1;

bool pauseEnabled     = true;
bool timeEnabled      = true;
bool debugMode        = false;
int frameCounter        = 0;
int stageMilliseconds = 0;
int stageSeconds      = 0;
int stageMinutes      = 0;

// Category and Scene IDs
int activeStageList   = 0;
int stageListPosition = 0;
char currentStageFolder[0x100];
int actID = 0;

char titleCardText[0x100];
byte titleCardWord2 = 0;

byte activeTileLayers[4];
byte tLayerMidPoint;
TileLayer stageLayouts[LAYER_COUNT];

int bgDeformationData0[DEFORM_COUNT];
int bgDeformationData1[DEFORM_COUNT];
int bgDeformationData2[DEFORM_COUNT];
int bgDeformationData3[DEFORM_COUNT];

LineScroll hParallax;
LineScroll vParallax;

Tiles128x128 tiles128x128;
CollisionMasks collisionMasks[2];

byte tilesetGFXData[TILESET_SIZE];

ushort tile3DFloorBuffer[0x13334];
bool drawStageGFXHQ = false;

void InitFirstStage()
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode     = 0;
    activePlayer = 0;
    ClearGraphicsData();
    ClearAnimationData();
    activePalette = fullPalette[0];
    LoadPalette("MasterPalette.act", 0, 0, 0, 256);
    stageMode         = STAGEMODE_LOAD;
    Engine.gameMode   = ENGINE_MAINGAME;
    //activeStageList   = 0;
    //stageListPosition = 0;
    activeStageList   = Engine.startList;
    stageListPosition = Engine.startStage;
}

void ProcessStage(void)
{
    int updateMax = 0; 
    switch (stageMode) {
        case STAGEMODE_LOAD: // Startup
            fadeMode = 0;
            SetActivePalette(0, 0, 256);

            cameraEnabled = 1;
            cameraTarget  = -1;
            cameraAdjustY = 0;
            xScrollOffset = 0;
            yScrollOffset = 0;
            yScrollA      = 0;
            yScrollB      = SCREEN_YSIZE;
            xScrollA      = 0;
            xScrollB      = SCREEN_XSIZE;
            yScrollMove   = 0;
            cameraShakeX  = 0;
            cameraShakeY  = 0;

            vertexCount = 0;
            faceCount   = 0;
            for (int i = 0; i < PLAYER_COUNT; ++i) {
                MEM_ZERO(playerList[i]);
                playerList[i].visible            = 1;
                playerList[i].gravity            = 1; // Air
                playerList[i].tileCollisions     = true;
                playerList[i].objectInteractions = true;
            }
            pauseEnabled      = false;
            timeEnabled       = false;
            frameCounter      = 0;
            stageMilliseconds = 0;
            stageSeconds      = 0;
            stageMinutes      = 0;
            Engine.frameCount = 0;
            stageMode         = STAGEMODE_NORMAL;
            ResetBackgroundSettings();
            LoadStageFiles();

#if RETRO_HARDWARE_RENDER
            texBufferMode = 0;
            for (int i = 0; i < LAYER_COUNT; i++) {
                if (stageLayouts[i].type == LAYER_3DSKY)
                    texBufferMode = 1;
            }
            for (int i = 0; i < hParallax.entryCount; i++) {
                if (hParallax.deform[i]) 
                    texBufferMode = 1;
            }

            if (tilesetGFXData[0x32002] > 0)
                texBufferMode = 0;

            if (texBufferMode) {
                for (int i = 0; i < TILEUV_SIZE; i += 4) {
                    tileUVArray[i + 0] = (i >> 2) % 28 * 18 + 1;
                    tileUVArray[i + 1] = (i >> 2) / 28 * 18 + 1;
                    tileUVArray[i + 2] = tileUVArray[i + 0] + 16;
                    tileUVArray[i + 3] = tileUVArray[i + 1] + 16;
                }
                tileUVArray[TILEUV_SIZE - 4] = 487.0f;
                tileUVArray[TILEUV_SIZE - 3] = 487.0f;
                tileUVArray[TILEUV_SIZE - 2] = 503.0f;
                tileUVArray[TILEUV_SIZE - 1] = 503.0f;
            }
            else {
                for (int i = 0; i < TILEUV_SIZE; i += 4) {
                    tileUVArray[i + 0] = (i >> 2 & 31) * 16;
                    tileUVArray[i + 1] = (i >> 2 >> 5) * 16;
                    tileUVArray[i + 2] = tileUVArray[i + 0] + 16;
                    tileUVArray[i + 3] = tileUVArray[i + 1] + 16;
                }
            }

            UpdateHardwareTextures();
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif

            break;
        case STAGEMODE_NORMAL:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            if (paletteMode > 0) {
                paletteMode = 0;
                SetActivePalette(0, 0, 256);
            }

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_PAUSED;
                PauseSound();
            }

            if (timeEnabled) {
                if (++frameCounter == Engine.refreshRate) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        if (++stageMinutes > 59)
                            stageMinutes = 0;
                    }
                }
                stageMilliseconds = 100 * frameCounter / Engine.refreshRate;
            }

            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                ProcessObjects();
            }

            if (cameraTarget > -1) {
                if (cameraEnabled == 1) {
                    switch (cameraStyle) {
                        case 0: SetPlayerScreenPosition(&playerList[cameraTarget]); break;
                        case 1: SetPlayerScreenPositionCDStyle(&playerList[cameraTarget]); break;
                        case 2: SetPlayerScreenPositionCDStyle(&playerList[cameraTarget]); break;
                        case 3: SetPlayerScreenPositionCDStyle(&playerList[cameraTarget]); break;
                        case 4: SetPlayerHLockedScreenPosition(&playerList[cameraTarget]); break;
                        default: break;
                    }
                }
                else {
                    SetPlayerLockedScreenPosition(&playerList[cameraTarget]);
                }
            }

            DrawStageGFX();
            break;
        case STAGEMODE_PAUSED:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode--;

            if (paletteMode > 0) {
                paletteMode = 0;
                SetActivePalette(0, 0, 256);
            }
            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(&keyDown, 0xFF);
            CheckKeyPress(&keyPress, 0xFF);
            
            updateMax = 1;
            /*updateMax = Engine.renderFrameIndex;
            if (Engine.refreshRate >= Engine.targetRefreshRate) {
                updateMax = 0;
                if (Engine.frameCount % Engine.skipFrameIndex < Engine.renderFrameIndex)
                    updateMax = 1;
            }*/

            // Update
            for (int i = 0; i < updateMax; ++i) {
                ProcessPausedObjects();
            }

#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif

            DrawObjectList(0);
            DrawObjectList(1);
            DrawObjectList(2);
            DrawObjectList(3);
            DrawObjectList(4);
            DrawObjectList(5);
            DrawObjectList(6);
            if (pauseEnabled && keyPress.start) {
                stageMode = STAGEMODE_NORMAL;
                ResumeSound();
            }
            break;
    }
    Engine.frameCount++;
}

void LoadStageFiles(void)
{
    StopAllSfx();
    FileInfo infoStore;
    FileInfo info;
    byte fileBuffer  = 0;
    byte fileBuffer2 = 0;
    int scriptID    = 1;
    char strBuffer[0x100];

    //For modders, when you try to reload the same Special Stage level, Sonic CD hangs because of imcomplete data initialisation of the level.
    //So to avoid this, we will check if we are on the SPECIAL STAGE category (activeStageList=3) : if yes, we will do a complete data initialisation.
    if (!CheckCurrentStageFolder(stageListPosition) || activeStageList==3 ) {
        printLog("Loading Scene %s - %s", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
        ReleaseStageSfx();
        LoadPalette("MasterPalette.act", 0, 0, 0, 256);
        ClearScriptData();
        for (int i = SPRITESHEETS_MAX; i > 0; i--) RemoveGraphicsFile((char *)"", i - 1);

        bool loadGlobals = false;
        if (LoadStageFile("StageConfig.bin", stageListPosition, &info)) {
            FileRead(&loadGlobals, 1);
            CloseFile();
        }
        if (loadGlobals && LoadFile("Data/Game/GameConfig.bin", &info)) {
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);

            byte globalObjectCount = 0;
            FileRead(&globalObjectCount, 1);
            for (byte i = 0; i < globalObjectCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                SetObjectTypeName(strBuffer, i + scriptID);
            }

#if RETRO_USE_MOD_LOADER
            if (Engine.usingBytecode && !forceUseScripts) {
#else
            if (Engine.usingBytecode) {
#endif
                GetFileInfo(&infoStore);
                CloseFile();
                LoadBytecode(4, scriptID);
                scriptID += globalObjectCount;
                SetFileInfo(&infoStore);
            }
            else {
                for (byte i = 0; i < globalObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                    GetFileInfo(&infoStore);
                    CloseFile();
                    ParseScriptFile(strBuffer, scriptID++);
                    SetFileInfo(&infoStore);
                    if (Engine.gameMode == ENGINE_SCRIPTERROR)
                        return;
                }
            }
            CloseFile();
        }

        if (LoadStageFile("StageConfig.bin", stageListPosition, &info)) {
            FileRead(&fileBuffer, 1); // Load Globals
            for (int i = 96; i < 128; ++i) {
                byte clr[3];
                FileRead(&clr, 3);
                SetPaletteEntry(-1, i, clr[0], clr[1], clr[2]);
            }

            byte stageObjectCount = 0;
            FileRead(&stageObjectCount, 1);
            for (byte i = 0; i < stageObjectCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                SetObjectTypeName(strBuffer, scriptID + i);
            }
#if RETRO_USE_MOD_LOADER
            if (Engine.usingBytecode && !forceUseScripts) {
#else
            if (Engine.usingBytecode) {
#endif
                for (byte i = 0; i < stageObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                }
                GetFileInfo(&infoStore);
                CloseFile();
                LoadBytecode(activeStageList, scriptID);
                SetFileInfo(&infoStore);
            }
            else {
                for (byte i = 0; i < stageObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                    GetFileInfo(&infoStore);
                    CloseFile();
                    ParseScriptFile(strBuffer, scriptID + i);
                    SetFileInfo(&infoStore);
                    if (Engine.gameMode == ENGINE_SCRIPTERROR)
                        return;
                }
            }

            FileRead(&fileBuffer2, 1);
            stageSFXCount = fileBuffer2;
            for (int i = 0; i < stageSFXCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                GetFileInfo(&infoStore);
                CloseFile();
                LoadSfx(strBuffer, globalSFXCount + i);
                SetFileInfo(&infoStore);
            }
            CloseFile();
        }
        FileInfo info;
        if (LoadStageFile("16x16Tiles.gif", stageListPosition, &info)) {
            CloseFile();
            LoadStageGIFFile(stageListPosition);
        }
        else {
            LoadStageGFXFile(stageListPosition);
        }
        LoadStageCollisions();
        LoadStageBackground();
    }
    else {
        printLog("Reloading Scene %s - %s", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
    }
    LoadStageChunks();
    for (int i = 0; i < TRACK_COUNT; ++i) SetMusicTrack((char *)"", i, 0, 0);
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        objectEntityList[i].type           = 0;
        objectEntityList[i].direction      = 0;
        objectEntityList[i].animation      = 0;
        objectEntityList[i].prevAnimation  = 0;
        objectEntityList[i].animationSpeed = 0;
        objectEntityList[i].animationTimer = 0;
        objectEntityList[i].frame          = 0;
        objectEntityList[i].priority       = 0;
        objectEntityList[i].direction      = 0;
        objectEntityList[i].rotation       = 0;
        objectEntityList[i].state          = 0;
        objectEntityList[i].propertyValue  = 0;
        objectEntityList[i].XPos           = 0;
        objectEntityList[i].YPos           = 0;
        objectEntityList[i].drawOrder      = 3;
        objectEntityList[i].scale          = 512;
        objectEntityList[i].inkEffect      = 0;
        objectEntityList[i].values[0]      = 0;
        objectEntityList[i].values[1]      = 0;
        objectEntityList[i].values[2]      = 0;
        objectEntityList[i].values[3]      = 0;
        objectEntityList[i].values[4]      = 0;
        objectEntityList[i].values[5]      = 0;
        objectEntityList[i].values[6]      = 0;
        objectEntityList[i].values[7]      = 0;
    }
    LoadActLayout();
    Init3DFloorBuffer(0);
    ProcessStartupObjects();
    xScrollA = (playerList[0].XPos >> 16) - SCREEN_CENTERX;
    xScrollB                 = (playerList[0].XPos >> 16) - SCREEN_CENTERX + SCREEN_XSIZE;
    yScrollA = (playerList[0].YPos >> 16) - SCREEN_SCROLL_UP;
    yScrollB                 = (playerList[0].YPos >> 16) - SCREEN_SCROLL_UP + SCREEN_YSIZE;
}
int LoadActFile(const char *ext, int stageID, FileInfo *info)
{
    char dest[0x40];

    StrCopy(dest, "Data/Stages/");
    StrAdd(dest, stageList[activeStageList][stageID].folder);
    StrAdd(dest, "/Act");
    StrAdd(dest, stageList[activeStageList][stageID].id);
    StrAdd(dest, ext);

    ConvertStringToInteger(stageList[activeStageList][stageID].id, &actID);

    return LoadFile(dest, info);
}
int LoadStageFile(const char *filePath, int stageID, FileInfo *info)
{
    char dest[0x40];

    StrCopy(dest, "Data/Stages/");
    StrAdd(dest, stageList[activeStageList][stageID].folder);
    StrAdd(dest, "/");
    StrAdd(dest, filePath);
    return LoadFile(dest, info);
}
void LoadActLayout()
{
    FileInfo info;
    if (LoadActFile(".bin", stageListPosition, &info)) {
        byte length = 0;
        FileRead(&length, 1);
        titleCardWord2 = (byte)length;
        for (int i = 0; i < length; i++) {
            FileRead(&titleCardText[i], 1);
            if (titleCardText[i] == '-')
                titleCardWord2 = (byte)(i + 1);
        }
        titleCardText[length] = '\0';

        // READ TILELAYER
        FileRead(activeTileLayers, 4);
        FileRead(&tLayerMidPoint, 1);

        FileRead(&stageLayouts[0].width, 1);
        FileRead(&stageLayouts[0].height, 1);
        xBoundary1    = 0;
        newXBoundary1 = 0;
        yBoundary1    = 0;
        newYBoundary1 = 0;
        xBoundary2    = stageLayouts[0].width << 7;
        yBoundary2    = stageLayouts[0].height << 7;
        waterLevel    = yBoundary2 + 128;
        newXBoundary2 = stageLayouts[0].width << 7;
        newYBoundary2 = stageLayouts[0].height << 7;

        for (int i = 0; i < 0x10000; ++i) stageLayouts[0].tiles[i] = 0;

        byte fileBuffer = 0;
        for (int y = 0; y < stageLayouts[0].height; ++y) {
            ushort *tiles = &stageLayouts[0].tiles[(y * 0x100)];
            for (int x = 0; x < stageLayouts[0].width; ++x) {
                FileRead(&fileBuffer, 1);
                tiles[x] = fileBuffer << 8;
                FileRead(&fileBuffer, 1);
                tiles[x] += fileBuffer;
            }
        }

        // READ TYPENAMES
        FileRead(&fileBuffer, 1);
        int typenameCnt = fileBuffer;
        if (fileBuffer) {
            for (int i = 0; i < typenameCnt; ++i) {
                FileRead(&fileBuffer, 1);
                int nameLen = fileBuffer;
                for (int l = 0; l < nameLen; ++l) FileRead(&fileBuffer, 1);
            }
        }

        // READ OBJECTS
        FileRead(&fileBuffer, 1);
        int ObjectCount = fileBuffer;
        FileRead(&fileBuffer, 1);
        ObjectCount = (ObjectCount << 8) + fileBuffer;
        Entity *object = &objectEntityList[32];
        for (int i = 0; i < ObjectCount; ++i) {
            FileRead(&fileBuffer, 1);
            object->type = fileBuffer;

            FileRead(&fileBuffer, 1);
            object->propertyValue = fileBuffer;

            FileRead(&fileBuffer, 1);
            object->XPos = fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            object->XPos += fileBuffer;
            object->XPos <<= 16;

            FileRead(&fileBuffer, 1);
            object->YPos = fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            object->YPos += fileBuffer;
            object->YPos <<= 16;

            ++object;
        }
        stageLayouts[0].type = LAYER_HSCROLL;
        CloseFile();
    }
}
void LoadStageBackground()
{
    for (int i = 0; i < LAYER_COUNT; ++i) {
        stageLayouts[i].type               = LAYER_NOSCROLL;
        stageLayouts[i].deformationOffset  = 0;
        stageLayouts[i].deformationOffsetW = 0;
    }
    for (int i = 0; i < PARALLAX_COUNT; ++i) {
        hParallax.scrollPos[i] = 0;
        vParallax.scrollPos[i] = 0;
    }

    FileInfo info;
    if (LoadStageFile("Backgrounds.bin", stageListPosition, &info)) {
        byte fileBuffer = 0;
        byte layerCount = 0;
        FileRead(&layerCount, 1);
        FileRead(&hParallax.entryCount, 1);
        for (int i = 0; i < hParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] = fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] += fileBuffer;

            FileRead(&fileBuffer, 1);
            hParallax.scrollSpeed[i] = fileBuffer << 10;

            hParallax.scrollPos[i] = 0;

            FileRead(&hParallax.deform[i], 1);
        }

        FileRead(&vParallax.entryCount, 1);
        for (int i = 0; i < vParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] = fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] += fileBuffer;

            FileRead(&fileBuffer, 1);
            vParallax.scrollSpeed[i] = fileBuffer << 10;

            vParallax.scrollPos[i] = 0;

            FileRead(&vParallax.deform[i], 1);
        }

        for (int i = 1; i < layerCount + 1; ++i) {
            FileRead(&fileBuffer, 1);
            stageLayouts[i].width = fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].height = fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].type = fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].parallaxFactor = fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].parallaxFactor += fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].scrollSpeed = fileBuffer << 10;
            stageLayouts[i].scrollPos   = 0;

            
            memset(stageLayouts[i].tiles, 0, TILELAYER_CHUNK_MAX * sizeof(ushort));
            byte *lineScrollPtr = stageLayouts[i].lineScroll;
            memset(stageLayouts[i].lineScroll, 0, 0x7FFF);

            // Read Line Scroll
            byte buf[3];
            while (true) {
                FileRead(&buf[0], 1);
                if (buf[0] == 0xFF) {
                    FileRead(&buf[1], 1);
                    if (buf[1] == 0xFF) {
                        break;
                    }
                    else {
                        FileRead(&buf[2], 1);
                        int val = buf[1];
                        int cnt = buf[2] - 1;
                        for (int c = 0; c < cnt; ++c) *lineScrollPtr++ = val;
                    }
                }
                else {
                    *lineScrollPtr++ = buf[0];
                }
            }

            // Read Layout
            for (int y = 0; y < stageLayouts[i].height; ++y) {
                ushort *chunks = &stageLayouts[i].tiles[y * 0x100];
                for (int x = 0; x < stageLayouts[i].width; ++x) {
                    FileRead(&fileBuffer, 1);
                    *chunks = fileBuffer << 8;
                    FileRead(&fileBuffer, 1);
                    *chunks += fileBuffer;
                    ++chunks;
                }
            }
        }

        CloseFile();
    }
}
void LoadStageChunks()
{
    FileInfo info;
    byte entry[3];

    if (LoadStageFile("128x128Tiles.bin", stageListPosition, &info)) {
        for (int i = 0; i < CHUNKTILE_COUNT; ++i) {
            FileRead(&entry, 3);
            entry[0] -= (byte)((entry[0] >> 6) << 6);

            tiles128x128.visualPlane[i] = (byte)(entry[0] >> 4);
            entry[0] -= 16 * (entry[0] >> 4);

            tiles128x128.direction[i] = (byte)(entry[0] >> 2);
            entry[0] -= 4 * (entry[0] >> 2);

            tiles128x128.tileIndex[i]  = entry[1] + (entry[0] << 8);
#if RETRO_SOFTWARE_RENDER
            tiles128x128.gfxDataPos[i] = tiles128x128.tileIndex[i] << 8;
#endif
#if RETRO_HARDWARE_RENDER
            tiles128x128.gfxDataPos[i] = tiles128x128.tileIndex[i] << 2;
#endif

            tiles128x128.collisionFlags[0][i] = entry[2] >> 4;
            tiles128x128.collisionFlags[1][i] = entry[2] - ((entry[2] >> 4) << 4);
        }
        CloseFile();
    }
}
void LoadStageCollisions()
{
    FileInfo info;
    if (LoadStageFile("CollisionMasks.bin", stageListPosition, &info)) {

        byte fileBuffer = 0;
        int tileIndex  = 0;
        for (int t = 0; t < 1024; ++t) {
            for (int p = 0; p < 2; ++p) {
                FileRead(&fileBuffer, 1);
                bool isCeiling             = fileBuffer >> 4;
                collisionMasks[p].flags[t] = fileBuffer & 0xF;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] = fileBuffer;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] += fileBuffer << 8;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] += fileBuffer << 16;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] += fileBuffer << 24;

                if (isCeiling) // Ceiling Tile
                {
                    for (int c = 0; c < TILE_SIZE; c += 2) {
                        FileRead(&fileBuffer, 1);
                        collisionMasks[p].roofMasks[c + tileIndex]     = fileBuffer >> 4;
                        collisionMasks[p].roofMasks[c + tileIndex + 1] = fileBuffer & 0xF;
                    }

                    // Has Collision (Pt 1)
                    FileRead(&fileBuffer, 1);
                    int id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) {
                        if (fileBuffer & id) {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex + 8]  = -0x40;
                        }
                        id <<= 1;
                    }

                    // Has Collision (Pt 2)
                    FileRead(&fileBuffer, 1);
                    id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) {
                        if (fileBuffer & id) {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex]  = -0x40;
                        }
                        id <<= 1;
                    }

                    // LWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = 0;
                        while (h > -1) {
                            if (h >= TILE_SIZE) {
                                collisionMasks[p].lWallMasks[c + tileIndex] = 0x40;
                                h                                           = -1;
                            }
                            else if (c > collisionMasks[p].roofMasks[h + tileIndex]) {
                                ++h;
                            }
                            else {
                                collisionMasks[p].lWallMasks[c + tileIndex] = h;
                                h                                           = -1;
                            }
                        }
                    }

                    // RWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = TILE_SIZE - 1;
                        while (h < TILE_SIZE) {
                            if (h <= -1) {
                                collisionMasks[p].rWallMasks[c + tileIndex] = -0x40;
                                h                                           = TILE_SIZE;
                            }
                            else if (c > collisionMasks[p].roofMasks[h + tileIndex]) {
                                --h;
                            }
                            else {
                                collisionMasks[p].rWallMasks[c + tileIndex] = h;
                                h                                           = TILE_SIZE;
                            }
                        }
                    }
                }
                else // Regular Tile
                {
                    for (int c = 0; c < TILE_SIZE; c += 2) {
                        FileRead(&fileBuffer, 1);
                        collisionMasks[p].floorMasks[c + tileIndex]     = fileBuffer >> 4;
                        collisionMasks[p].floorMasks[c + tileIndex + 1] = fileBuffer & 0xF;
                    }
                    FileRead(&fileBuffer, 1);
                    int id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) // HasCollision
                    {
                        if (fileBuffer & id) {
                            collisionMasks[p].roofMasks[c + tileIndex + 8] = 0xF;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex + 8]  = -0x40;
                        }
                        id <<= 1;
                    }

                    FileRead(&fileBuffer, 1);
                    id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) // HasCollision (pt 2)
                    {
                        if (fileBuffer & id) {
                            collisionMasks[p].roofMasks[c + tileIndex] = 0xF;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex]  = -0x40;
                        }
                        id <<= 1;
                    }

                    //LWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = 0;
                        while (h > -1) {
                            if (h >= TILE_SIZE) {
                                collisionMasks[p].lWallMasks[c + tileIndex] = 0x40;
                                h                                           = -1;
                            }
                            else if (c < collisionMasks[p].floorMasks[h + tileIndex]) {
                                ++h;
                            }
                            else {
                                collisionMasks[p].lWallMasks[c + tileIndex] = h;
                                h                                           = -1;
                            }
                        }
                    }

                    //RWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = TILE_SIZE - 1;
                        while (h < TILE_SIZE) {
                            if (h <= -1) {
                                collisionMasks[p].rWallMasks[c + tileIndex] = -0x40;
                                h                                           = TILE_SIZE;
                            }
                            else if (c < collisionMasks[p].floorMasks[h + tileIndex]) {
                                --h;
                            }
                            else {
                                collisionMasks[p].rWallMasks[c + tileIndex] = h;
                                h                                           = TILE_SIZE;
                            }
                        }
                    }
                }
            }
            tileIndex += 16;
        }
        CloseFile();
    }
}
void LoadStageGIFFile(int stageID)
{
    FileInfo info;
    if (LoadStageFile("16x16Tiles.gif", stageID, &info)) {
        byte fileBuffer = 0;
        int fileBuffer2 = 0;

        SetFilePosition(6); // GIF89a
        FileRead(&fileBuffer, 1);
        int width = fileBuffer;
        FileRead(&fileBuffer, 1);
        width += (fileBuffer << 8);
        FileRead(&fileBuffer, 1);
        int height = fileBuffer;
        FileRead(&fileBuffer, 1);
        height += (fileBuffer << 8);

        FileRead(&fileBuffer, 1); // Palette Size
        //int has_pallete = (fileBuffer & 0x80) >> 7;
        //int colors = ((fileBuffer & 0x70) >> 4) + 1;
        int palette_size = (fileBuffer & 0x7) + 1;
        if (palette_size > 0)
            palette_size = 1 << palette_size;

        FileRead(&fileBuffer, 1); // BG Colour index (thrown away)
        FileRead(&fileBuffer, 1); // Pixel aspect ratio (thrown away)

        if (palette_size == 256)
        {
            byte clr[3];

            for (int c = 0; c < 0x80; ++c) FileRead(clr, 3);
            for (int c = 0x80; c < 0x100; ++c) {
                FileRead(clr, 3);
                SetPaletteEntry(-1, c, clr[0], clr[1], clr[2]);
            }
        }

        FileRead(&fileBuffer, 1);
        while (fileBuffer != ',') FileRead(&fileBuffer, 1); // gif image start identifier

        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer, 1);
        bool interlaced = (fileBuffer & 0x40) >> 6;
        if ((unsigned int)fileBuffer >> 7 == 1) {
            int c = 128;
            do {
                ++c;
                FileRead(&fileBuffer2, 3);
            } while (c != 256);
        }

        ReadGifPictureData(width, height, interlaced, tilesetGFXData, 0);

        byte transparent = tilesetGFXData[0];
        for (int i = 0; i < 0x40000; ++i) {
            if (tilesetGFXData[i] == transparent)
                tilesetGFXData[i] = 0;
        }

        CloseFile();
    }
}
void LoadStageGFXFile(int stageID)
{
    FileInfo info;
    if (LoadStageFile("16x16Tiles.gfx", stageID, &info)) {
        byte fileBuffer = 0;
        FileRead(&fileBuffer, 1);
        int width = fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        width += fileBuffer;
        FileRead(&fileBuffer, 1);
        int height = fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        height += fileBuffer;

        byte clr[3];
        for (int i = 0; i < 0x80; ++i) FileRead(&clr, 3); // Palette
        for (int c = 0x80; c < 0x100; ++c) {
            FileRead(clr, 3);
            SetPaletteEntry(-1, c, clr[0], clr[1], clr[2]);
        }

        byte *gfxData = tilesetGFXData;
        byte buf[3];
        while (true) {
            FileRead(&buf[0], 1);
            if (buf[0] == 0xFF) {
                FileRead(&buf[1], 1);
                if (buf[1] == 0xFF) {
                    break;
                }
                else {
                    FileRead(&buf[2], 1);
                    for (int i = 0; i < buf[2]; ++i) *gfxData++ = buf[1];
                }
            }
            else {
                *gfxData++ = buf[0];
            }
        }

        byte transparent = tilesetGFXData[0];
        for (int i = 0; i < 0x40000; ++i) {
            if (tilesetGFXData[i] == transparent)
                tilesetGFXData[i] = 0;
        }

        CloseFile();
    }
}

void ResetBackgroundSettings()
{
    for (int i = 0; i < LAYER_COUNT; ++i) {
        stageLayouts[i].deformationOffset  = 0;
        stageLayouts[i].deformationOffsetW = 0;
        stageLayouts[i].scrollPos          = 0;
    }

    for (int i = 0; i < PARALLAX_COUNT; ++i) {
        hParallax.scrollPos[i] = 0;
        vParallax.scrollPos[i] = 0;
    }

    for (int i = 0; i < DEFORM_COUNT; ++i) {
        bgDeformationData0[i] = 0;
        bgDeformationData1[i] = 0;
        bgDeformationData2[i] = 0;
        bgDeformationData3[i] = 0;
    }
}

void SetLayerDeformation(int selectedDef, int waveLength, int waveWidth, int waveType, int YPos, int waveSize)
{
    int *deformPtr = nullptr;
    switch (selectedDef) {
        case DEFORM_FG: deformPtr = bgDeformationData0; break;
        case DEFORM_FG_WATER: deformPtr = bgDeformationData1; break;
        case DEFORM_BG: deformPtr = bgDeformationData2; break;
        case DEFORM_BG_WATER: deformPtr = bgDeformationData3; break;
        default: break;
    }

#if RETRO_SOFTWARE_RENDER
    int shift = 9;
#endif

#if RETRO_HARDWARE_RENDER
    int shift = 5;
#endif

    int id = 0;
    if (waveType == 1) {
        id     = YPos;
        for (int i = 0; i < waveSize; ++i) {
            deformPtr[id] = waveWidth * sinVal512[(i << 9) / waveLength & 0x1FF] >> shift;
            ++id;
        }
    }
    else {
        for (int i = 0; i < 0x200 * 0x100; i += 0x200) {
            int val       = waveWidth * sinVal512[i / waveLength & 0x1FF] >> shift;
            deformPtr[id] = val;
#if RETRO_SOFTWARE_RENDER
            if (deformPtr[id] >= waveWidth)
                deformPtr[id] = waveWidth - 1;
#endif
            ++id;
        }
    }

    switch (selectedDef) {
        case DEFORM_FG:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData0[i] = bgDeformationData0[i - DEFORM_STORE];
            break;
        case DEFORM_FG_WATER:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData1[i] = bgDeformationData1[i - DEFORM_STORE];
            break;
        case DEFORM_BG:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData2[i] = bgDeformationData2[i - DEFORM_STORE];
            break;
        case DEFORM_BG_WATER:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData3[i] = bgDeformationData3[i - DEFORM_STORE];
            break;
        default: break;
    }
}

void SetPlayerScreenPosition(Player *player)
{
    int playerXPos = player->XPos >> 16;
    int playerYPos = player->YPos >> 16;
    if (newYBoundary1 > yBoundary1) {
        if (yScrollOffset <= newYBoundary1)
            yBoundary1 = yScrollOffset;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < yBoundary1) {
        if (yScrollOffset <= yBoundary1)
            --yBoundary1;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2 || yScrollOffset + SCREEN_YSIZE <= newYBoundary2)
            --yBoundary2;
        else
            yBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2)
            ++yBoundary2;
        else
            yBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > xBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            xBoundary1 = xScrollOffset;
        else
            xBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < xBoundary1) {
        if (xScrollOffset <= xBoundary1) {
            --xBoundary1;
            if (player->XVelocity < 0) {
                xBoundary1 += player->XVelocity >> 16;
                if (xBoundary1 < newXBoundary1)
                    xBoundary1 = newXBoundary1;
            }
        }
        else {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2)
            xBoundary2 = SCREEN_XSIZE + xScrollOffset;
        else
            xBoundary2 = newXBoundary2;
    }
    if (newXBoundary2 > xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2) {
            ++xBoundary2;
            if (player->XVelocity > 0) {
                xBoundary2 += player->XVelocity >> 16;
                if (xBoundary2 > newXBoundary2)
                    xBoundary2 = newXBoundary2;
            }
        }
        else {
            xBoundary2 = newXBoundary2;
        }
    }
    int xscrollA     = xScrollA;
    int xscrollB     = xScrollB;
    int scrollAmount = playerXPos - (SCREEN_CENTERX + xScrollA);
    if (abs(playerXPos - (SCREEN_CENTERX + xScrollA)) >= 25) {
        if (scrollAmount <= 0)
            xscrollA -= 16;
        else
            xscrollA += 16;
        xscrollB = SCREEN_XSIZE + xscrollA;
    }
    else {
        if (playerXPos > SCREEN_SCROLL_RIGHT + xscrollA) {
            xscrollA = playerXPos - SCREEN_SCROLL_RIGHT;
            xscrollB = SCREEN_XSIZE + playerXPos - SCREEN_SCROLL_RIGHT;
        }
        if (playerXPos < SCREEN_SCROLL_LEFT + xscrollA) {
            xscrollA = playerXPos - SCREEN_SCROLL_LEFT;
            xscrollB = SCREEN_XSIZE + playerXPos - SCREEN_SCROLL_LEFT;
        }
    }
    if (xscrollA < xBoundary1) {
        xscrollA = xBoundary1;
        xscrollB = SCREEN_XSIZE + xBoundary1;
    }
    if (xscrollB > xBoundary2) {
        xscrollB = xBoundary2;
        xscrollA = xBoundary2 - SCREEN_XSIZE;
    }

    xScrollA = xscrollA;
    xScrollB = xscrollB;
    if (playerXPos <= SCREEN_CENTERX + xscrollA) {
        player->screenXPos = cameraShakeX + playerXPos - xscrollA;
        xScrollOffset      = xscrollA - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX;
        player->screenXPos = SCREEN_CENTERX - cameraShakeX;
        if (playerXPos > xscrollB - SCREEN_CENTERX) {
            player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xscrollB - SCREEN_CENTERX);
            xScrollOffset      = xscrollB - SCREEN_XSIZE - cameraShakeX;
        }
    }

    int yscrollA     = yScrollA;
    int yscrollB     = yScrollB;
    int adjustYPos   = cameraAdjustY + playerYPos;
    int adjustAmount = player->lookPos + adjustYPos - (yscrollA + SCREEN_SCROLL_UP);
    if (player->trackScroll) {
        yScrollMove = 32;
    }
    else {
        if (yScrollMove == 32) {
            yScrollMove = 2 * ((SCREEN_SCROLL_UP - player->screenYPos - player->lookPos) >> 1);
            if (yScrollMove > 32)
                yScrollMove = 32;
            if (yScrollMove < -32)
                yScrollMove = -32;
        }
        if (yScrollMove > 0)
            yScrollMove -= 6;
        yScrollMove += yScrollMove < 0 ? 6 : 0;
    }

    if (abs(adjustAmount) >= abs(yScrollMove) + 17) {
        if (adjustAmount <= 0)
            yscrollA -= 16;
        else
            yscrollA += 16;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    else if (yScrollMove == 32) {
        if (player->lookPos + adjustYPos > yscrollA + yScrollMove + SCREEN_SCROLL_UP) {
            yscrollA = player->lookPos + adjustYPos - (yScrollMove + SCREEN_SCROLL_UP);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
        if (player->lookPos + adjustYPos < yscrollA + SCREEN_SCROLL_UP - yScrollMove) {
            yscrollA = player->lookPos + adjustYPos - (SCREEN_SCROLL_UP - yScrollMove);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
    }
    else {
        yscrollA = player->lookPos + adjustYPos + yScrollMove - SCREEN_SCROLL_UP;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    if (yscrollA < yBoundary1) {
        yscrollA = yBoundary1;
        yscrollB = yBoundary1 + SCREEN_YSIZE;
    }
    if (yscrollB > yBoundary2) {
        yscrollB = yBoundary2;
        yscrollA = yBoundary2 - SCREEN_YSIZE;
    }
    yScrollA = yscrollA;
    yScrollB = yscrollB;
    if (player->lookPos + adjustYPos <= yScrollA + SCREEN_SCROLL_UP) {
        player->screenYPos = adjustYPos - yScrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yScrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustYPos + player->lookPos - SCREEN_SCROLL_UP;
        player->screenYPos = SCREEN_SCROLL_UP - player->lookPos - cameraShakeY;
        if (player->lookPos + adjustYPos > yScrollB - SCREEN_SCROLL_DOWN) {
            player->screenYPos = adjustYPos - (yScrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yScrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }
    player->screenYPos -= cameraAdjustY;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
void SetPlayerScreenPositionCDStyle(Player *player)
{
    int playerXPos = player->XPos >> 16;
    int playerYPos = player->YPos >> 16;
    if (newYBoundary1 > yBoundary1) {
        if (yScrollOffset <= newYBoundary1)
            yBoundary1 = yScrollOffset;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < yBoundary1) {
        if (yScrollOffset <= yBoundary1)
            --yBoundary1;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2 || yScrollOffset + SCREEN_YSIZE <= newYBoundary2)
            --yBoundary2;
        else
            yBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2)
            ++yBoundary2;
        else
            yBoundary2 = newYBoundary2;
    }
    if (newXBoundary1 > xBoundary1) {
        if (xScrollOffset <= newXBoundary1)
            xBoundary1 = xScrollOffset;
        else
            xBoundary1 = newXBoundary1;
    }
    if (newXBoundary1 < xBoundary1) {
        if (xScrollOffset <= xBoundary1) {
            --xBoundary1;
            if (player->XVelocity < 0) {
                xBoundary1 += player->XVelocity >> 16;
                if (xBoundary1 < newXBoundary1)
                    xBoundary1 = newXBoundary1;
            }
        }
        else {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2)
            xBoundary2 = SCREEN_XSIZE + xScrollOffset;
        else
            xBoundary2 = newXBoundary2;
    }
    if (newXBoundary2 > xBoundary2) {
        if (SCREEN_XSIZE + xScrollOffset >= xBoundary2) {
            ++xBoundary2;
            if (player->XVelocity > 0) {
                xBoundary2 += player->XVelocity >> 16;
                if (xBoundary2 > newXBoundary2)
                    xBoundary2 = newXBoundary2;
            }
        }
        else {
            xBoundary2 = newXBoundary2;
        }
    }
    if (!player->gravity) {
        if (player->boundEntity->direction) {
            if (cameraStyle == 3 || player->speed < -0x5F5C2)
                cameraLagStyle = 2;
            else
                cameraLagStyle = 0;
        }
        else {
            cameraLagStyle = (cameraStyle == 2 || player->speed > 0x5F5C2) != 0;
        }
    }
    if (cameraLagStyle) {
        if (cameraLagStyle == 1) {
            if (cameraLag > -64)
                cameraLag -= 2;
        }
        else if (cameraLagStyle == 2 && cameraLag < 64) {
            cameraLag += 2;
        }
    }
    else {
        cameraLag += cameraLag < 0 ? 2 : 0;
        if (cameraLag > 0)
            cameraLag -= 2;
    }
    if (playerXPos <= cameraLag + SCREEN_CENTERX + xBoundary1) {
        player->screenXPos = cameraShakeX + playerXPos - xBoundary1;
        xScrollOffset      = xBoundary1 - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX - cameraLag;
        player->screenXPos = cameraLag + SCREEN_CENTERX - cameraShakeX;
        if (playerXPos - cameraLag > xBoundary2 - SCREEN_CENTERX) {
            player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xBoundary2 - SCREEN_CENTERX);
            xScrollOffset      = xBoundary2 - SCREEN_XSIZE - cameraShakeX;
        }
    }
    xScrollA         = xScrollOffset;
    xScrollB         = SCREEN_XSIZE + xScrollOffset;
    int yscrollA     = yScrollA;
    int yscrollB     = yScrollB;
    int adjustY      = cameraAdjustY + playerYPos;
    int adjustOffset = player->lookPos + adjustY - (yScrollA + SCREEN_SCROLL_UP);
    if (player->trackScroll == 1) {
        yScrollMove = 32;
    }
    else {
        if (yScrollMove == 32) {
            yScrollMove = 2 * ((SCREEN_SCROLL_UP - player->screenYPos - player->lookPos) >> 1);
            if (yScrollMove > 32)
                yScrollMove = 32;
            if (yScrollMove < -32)
                yScrollMove = -32;
        }
        if (yScrollMove > 0)
            yScrollMove -= 6;
        yScrollMove += yScrollMove < 0 ? 6 : 0;
    }

    int absAdjust = abs(adjustOffset);
    if (absAdjust >= abs(yScrollMove) + 17) {
        if (adjustOffset <= 0)
            yscrollA -= 16;
        else
            yscrollA += 16;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    else if (yScrollMove == 32) {
        if (player->lookPos + adjustY > yscrollA + yScrollMove + SCREEN_SCROLL_UP) {
            yscrollA = player->lookPos + adjustY - (yScrollMove + SCREEN_SCROLL_UP);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
        if (player->lookPos + adjustY < yscrollA + SCREEN_SCROLL_UP - yScrollMove) {
            yscrollA = player->lookPos + adjustY - (SCREEN_SCROLL_UP - yScrollMove);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
    }
    else {
        yscrollA = player->lookPos + adjustY + yScrollMove - SCREEN_SCROLL_UP;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    if (yscrollA < yBoundary1) {
        yscrollA = yBoundary1;
        yscrollB = yBoundary1 + SCREEN_YSIZE;
    }
    if (yscrollB > yBoundary2) {
        yscrollB = yBoundary2;
        yscrollA = yBoundary2 - SCREEN_YSIZE;
    }
    yScrollA = yscrollA;
    yScrollB = yscrollB;
    if (player->lookPos + adjustY <= yscrollA + SCREEN_SCROLL_UP) {
        player->screenYPos = adjustY - yscrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yscrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustY + player->lookPos - SCREEN_SCROLL_UP;
        player->screenYPos = SCREEN_SCROLL_UP - player->lookPos - cameraShakeY;
        if (player->lookPos + adjustY > yscrollB - SCREEN_SCROLL_DOWN) {
            player->screenYPos = adjustY - (yscrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yscrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }
    player->screenYPos -= cameraAdjustY;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
void SetPlayerHLockedScreenPosition(Player *player)
{
    int playerXPos = player->XPos >> 16;
    int playerYPos = player->YPos >> 16;
    if (newYBoundary1 > yBoundary1) {
        if (yScrollOffset <= newYBoundary1)
            yBoundary1 = yScrollOffset;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary1 < yBoundary1) {
        if (yScrollOffset <= yBoundary1)
            --yBoundary1;
        else
            yBoundary1 = newYBoundary1;
    }
    if (newYBoundary2 < yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2 || yScrollOffset + SCREEN_YSIZE <= newYBoundary2)
            --yBoundary2;
        else
            yBoundary2 = yScrollOffset + SCREEN_YSIZE;
    }
    if (newYBoundary2 > yBoundary2) {
        if (yScrollOffset + SCREEN_YSIZE >= yBoundary2)
            ++yBoundary2;
        else
            yBoundary2 = newYBoundary2;
    }

    int xscrollA = xScrollA;
    int xscrollB = xScrollB;
    if (playerXPos <= SCREEN_CENTERX + xScrollA) {
        player->screenXPos = cameraShakeX + playerXPos - xScrollA;
        xScrollOffset      = xscrollA - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX;
        player->screenXPos = SCREEN_CENTERX - cameraShakeX;
        if (playerXPos > xscrollB - SCREEN_CENTERX) {
            player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xscrollB - SCREEN_CENTERX);
            xScrollOffset      = xscrollB - SCREEN_XSIZE - cameraShakeX;
        }
    }

    int yscrollA   = yScrollA;
    int yscrollB   = yScrollB;
    int adjustY    = cameraAdjustY + playerYPos;
    int lookOffset = player->lookPos + adjustY - (yScrollA + SCREEN_SCROLL_UP);
    if (player->trackScroll == 1) {
        yScrollMove = 32;
    }
    else {
        if (yScrollMove == 32) {
            yScrollMove = 2 * ((SCREEN_SCROLL_UP - player->screenYPos - player->lookPos) >> 1);
            if (yScrollMove > 32)
                yScrollMove = 32;
            if (yScrollMove < -32)
                yScrollMove = -32;
        }
        if (yScrollMove > 0)
            yScrollMove -= 6;
        yScrollMove += yScrollMove < 0 ? 6 : 0;
    }

    int absLook = abs(lookOffset);
    if (absLook >= abs(yScrollMove) + 17) {
        if (lookOffset <= 0)
            yscrollA -= 16;
        else
            yscrollA += 16;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    else if (yScrollMove == 32) {
        if (player->lookPos + adjustY > yscrollA + yScrollMove + SCREEN_SCROLL_UP) {
            yscrollA = player->lookPos + adjustY - (yScrollMove + SCREEN_SCROLL_UP);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
        if (player->lookPos + adjustY < yscrollA + SCREEN_SCROLL_UP - yScrollMove) {
            yscrollA = player->lookPos + adjustY - (SCREEN_SCROLL_UP - yScrollMove);
            yscrollB = yscrollA + SCREEN_YSIZE;
        }
    }
    else {
        yscrollA = player->lookPos + adjustY + yScrollMove - SCREEN_SCROLL_UP;
        yscrollB = yscrollA + SCREEN_YSIZE;
    }
    if (yscrollA < yBoundary1) {
        yscrollA = yBoundary1;
        yscrollB = yBoundary1 + SCREEN_YSIZE;
    }
    if (yscrollB > yBoundary2) {
        yscrollB = yBoundary2;
        yscrollA = yBoundary2 - SCREEN_YSIZE;
    }
    yScrollA = yscrollA;
    yScrollB = yscrollB;
    if (player->lookPos + adjustY <= yscrollA + SCREEN_SCROLL_UP) {
        player->screenYPos = adjustY - yscrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yscrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustY + player->lookPos - SCREEN_SCROLL_UP;
        player->screenYPos = SCREEN_SCROLL_UP - player->lookPos - cameraShakeY;
        if (player->lookPos + adjustY > yscrollB - SCREEN_SCROLL_DOWN) {
            player->screenYPos = adjustY - (yscrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yscrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }
    player->screenYPos -= cameraAdjustY;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
void SetPlayerLockedScreenPosition(Player *player)
{
    int playerXPos = player->XPos >> 16;
    int playerYPos = player->YPos >> 16;
    int xscrollA   = xScrollA;
    int xscrollB   = xScrollB;
    if (playerXPos <= SCREEN_CENTERX + xScrollA) {
        player->screenXPos = cameraShakeX + playerXPos - xScrollA;
        xScrollOffset      = xscrollA - cameraShakeX;
    }
    else {
        xScrollOffset      = cameraShakeX + playerXPos - SCREEN_CENTERX;
        player->screenXPos = SCREEN_CENTERX - cameraShakeX;
        if (playerXPos > xscrollB - SCREEN_CENTERX) {
            player->screenXPos = cameraShakeX + SCREEN_CENTERX + playerXPos - (xscrollB - SCREEN_CENTERX);
            xScrollOffset      = xscrollB - SCREEN_XSIZE - cameraShakeX;
        }
    }

    int yscrollA     = yScrollA;
    int yscrollB     = yScrollB;
    int adjustY      = cameraAdjustY + playerYPos;
    //int adjustOffset = player->lookPos + adjustY - (yScrollA + SCREEN_SCROLL_UP);
    if (player->lookPos + adjustY <= yScrollA + SCREEN_SCROLL_UP) {
        player->screenYPos = adjustY - yScrollA - cameraShakeY;
        yScrollOffset      = cameraShakeY + yscrollA;
    }
    else {
        yScrollOffset      = cameraShakeY + adjustY + player->lookPos - SCREEN_SCROLL_UP;
        player->screenYPos = SCREEN_SCROLL_UP - player->lookPos - cameraShakeY;
        if (player->lookPos + adjustY > yscrollB - SCREEN_SCROLL_DOWN) {
            player->screenYPos = adjustY - (yscrollB - SCREEN_SCROLL_DOWN) + cameraShakeY + SCREEN_SCROLL_UP;
            yScrollOffset      = yscrollB - SCREEN_YSIZE - cameraShakeY;
        }
    }
    player->screenYPos -= cameraAdjustY;

    if (cameraShakeX) {
        if (cameraShakeX <= 0) {
            cameraShakeX = ~cameraShakeX;
        }
        else {
            cameraShakeX = -cameraShakeX;
        }
    }

    if (cameraShakeY) {
        if (cameraShakeY <= 0) {
            cameraShakeY = ~cameraShakeY;
        }
        else {
            cameraShakeY = -cameraShakeY;
        }
    }
}
