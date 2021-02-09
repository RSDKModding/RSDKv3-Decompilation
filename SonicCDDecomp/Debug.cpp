#include "RetroEngine.hpp"

int touchTimer = 0;

void initDevMenu()
{
    drawStageGFXHQ = false;
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode        = 0;
    playerListPos   = 0;
    Engine.gameMode = ENGINE_DEVMENU;
    ClearGraphicsData();
    ClearAnimationData();
    LoadPalette("MasterPalette.act", 0, 0, 0, 256);
    SetActivePalette(0, 0, 256);
    textMenuSurfaceNo = 0;
    LoadGIFFile("Data/Game/SystemText.gif", 0);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);
    stageMode = DEVMENU_MAIN;
    SetupTextMenu(&gameMenu[0], 0);
    AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
    AddTextMenuEntry(&gameMenu[0], " ");
    char version[0x80];
    StrCopy(version, Engine.gameWindowText);
    StrAdd(version, " Version");
    AddTextMenuEntry(&gameMenu[0], version);
    AddTextMenuEntry(&gameMenu[0], Engine.gameVersion);
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], "START GAME");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], "MODS");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], "EXIT GAME");
    gameMenu[0].alignment        = 2;
    gameMenu[0].selectionCount   = 2;
    gameMenu[0].selection1       = 0;
    gameMenu[0].selection2       = 9;
    gameMenu[1].visibleRowCount  = 0;
    gameMenu[1].visibleRowOffset = 0;
#if RETRO_HARDWARE_RENDER
    Engine.highResMode = false;
    render3DEnabled    = false;
    UpdateHardwareTextures();
#endif
}
void initErrorMessage()
{
    drawStageGFXHQ = false;
    xScrollOffset = 0;
    yScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode        = 0;
    playerListPos   = 0;
    Engine.gameMode = ENGINE_DEVMENU;
    ClearGraphicsData();
    ClearAnimationData();
    LoadPalette("MasterPalette.act", 0, 0, 0, 256);
    SetActivePalette(0, 0, 256);
    textMenuSurfaceNo = 0;
    LoadGIFFile("Data/Game/SystemText.gif", 0);
    SetPaletteEntry(-1, 0xF0, 0x00, 0x00, 0x00);
    SetPaletteEntry(-1, 0xFF, 0xFF, 0xFF, 0xFF);
    gameMenu[0].alignment        = 2;
    gameMenu[0].selectionCount   = 1;
    gameMenu[0].selection1       = 0;
    gameMenu[1].visibleRowCount  = 0;
    gameMenu[1].visibleRowOffset = 0;
    stageMode                    = DEVMENU_SCRIPTERROR;
    touchTimer                   = 0;
#if RETRO_HARDWARE_RENDER
    Engine.highResMode = false;
    render3DEnabled    = false;
    UpdateHardwareTextures();
#endif
}
void processStageSelect()
{
    ClearScreen(0xF0);
    keyDown.start = false;
    keyDown.B     = false;
    keyDown.up    = false;
    keyDown.down  = false;
    CheckKeyDown(&keyDown, 0xFF);
    CheckKeyPress(&keyPress, 0xFF);

//#if defined RETRO_USING_MOUSE || defined RETRO_USING_TOUCH
    DrawSprite(32, 0x42, 16, 16, 78, 240, textMenuSurfaceNo);
    DrawSprite(32, 0xB2, 16, 16, 95, 240, textMenuSurfaceNo);
    DrawSprite(SCREEN_XSIZE - 32, SCREEN_YSIZE - 32, 16, 16, 112, 240, textMenuSurfaceNo);
//#endif

    if (!keyDown.start && !keyDown.up && !keyDown.down) {
        if (touches > 0) {
            if (touchDown[0] && !(touchTimer % 8)) {
                if (touchX[0] < SCREEN_CENTERX) {
                    if (touchY[0] >= SCREEN_CENTERY) {
                        if (!keyDown.down)
                            keyPress.down = true;
                        keyDown.down = true;
                    }
                    else {
                        if (!keyDown.up)
                            keyPress.up = true;
                        keyDown.up = true;
                    }
                }
                else if (touchX[0] > SCREEN_CENTERX) {
                    if (touchY[0] > SCREEN_CENTERY) {
                        if (!keyDown.start)
                            keyPress.start = true;
                        keyDown.start = true;
                    }
                    else {
                        if (!keyDown.B)
                            keyPress.B = true;
                        keyDown.B = true;
                    }
                }
            }
        }
    }

    touchTimer++;

    switch (stageMode) {
        case DEVMENU_MAIN: // Main Menu
        {
            if (keyPress.down)
                gameMenu[0].selection2 += 2;

            if (keyPress.up)
                gameMenu[0].selection2 -= 2;

            if (gameMenu[0].selection2 > 15)
                gameMenu[0].selection2 = 9;
            if (gameMenu[0].selection2 < 9)
                gameMenu[0].selection2 = 15;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX, 72);
            if (keyPress.start || keyPress.A) {
                if (gameMenu[0].selection2 == 9) {
                    ClearGraphicsData();
                    ClearAnimationData();
                    LoadPalette("MasterPalette.act", 0, 0, 0, 256);
                    activeStageList   = 0;
                    stageMode         = STAGEMODE_LOAD;
                    Engine.gameMode   = ENGINE_MAINGAME;
                    stageListPosition = 0;
                }
                else if (gameMenu[0].selection2 == 11) {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                    SetupTextMenu(&gameMenu[1], 0);
                    LoadConfigListText(&gameMenu[1], 0);
                    gameMenu[1].alignment      = 0;
                    gameMenu[1].selectionCount = 1;
                    gameMenu[1].selection1     = 0;
                    stageMode                  = DEVMENU_PLAYERSEL;
                }
                else if (gameMenu[0].selection2 == 13) {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "MOD LIST");
                    SetupTextMenu(&gameMenu[1], 0);

                    char buffer[0x100];
                    for (int m = 0; m < modCount; ++m) {
                        StrCopy(buffer, modList[m].name.c_str());
                        StrAdd(buffer, ": ");
                        StrAdd(buffer, modList[m].active ? "  Active" : "Inactive");
                        AddTextMenuEntry(&gameMenu[1], buffer);
                    }

                    gameMenu[1].alignment      = 1;
                    gameMenu[1].selectionCount = 3;
                    gameMenu[1].selection1     = 0;
                    if (gameMenu[1].rowCount > 18)
                        gameMenu[1].visibleRowCount = 18;
                    else
                        gameMenu[1].visibleRowCount = 0;

                    gameMenu[0].alignment        = 2;
                    gameMenu[0].selectionCount   = 1;
                    gameMenu[1].timer            = 0;
                    gameMenu[1].visibleRowOffset = 0;
                    stageMode                  = DEVMENU_MODMENU;
                }
                else if (gameMenu[0].selection2 == 15) {
                    Engine.running = false;
                }
            }
            else if (keyPress.B) {
                ClearGraphicsData();
                ClearAnimationData();
                LoadPalette("MasterPalette.act", 0, 0, 0, 256);
                activeStageList   = 0;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
                stageListPosition = 0;
            }
            break;
        }
        case DEVMENU_PLAYERSEL: // Selecting Player
        {
            if (keyPress.down)
                ++gameMenu[1].selection1;
            if (keyPress.up)
                --gameMenu[1].selection1;
            if (gameMenu[1].selection1 == gameMenu[1].rowCount)
                gameMenu[1].selection1 = 0;

            if (gameMenu[1].selection1 < 0)
                gameMenu[1].selection1 = gameMenu[1].rowCount - 1;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX - 40, 96);
            if (keyPress.start || keyPress.A) {
                playerListPos = gameMenu[1].selection1;
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE LIST");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   PRESENTATION");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   REGULAR");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   SPECIAL");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   BONUS");
                gameMenu[0].alignment  = 0;
                gameMenu[0].selection2 = 3;
                stageMode              = DEVMENU_STAGELISTSEL;
            }
            else if (keyPress.B) {
                stageMode = DEVMENU_MAIN;
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
                AddTextMenuEntry(&gameMenu[0], " ");
                char version[0x80];
                StrCopy(version, Engine.gameWindowText);
                StrAdd(version, " Version");
                AddTextMenuEntry(&gameMenu[0], version);
                AddTextMenuEntry(&gameMenu[0], Engine.gameVersion);
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "START GAME");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "MODS");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "EXIT GAME");
                gameMenu[0].alignment        = 2;
                gameMenu[0].selectionCount   = 2;
                gameMenu[0].selection1       = 0;
                gameMenu[0].selection2       = 9;
                gameMenu[1].visibleRowCount  = 0;
                gameMenu[1].visibleRowOffset = 0;
            }
            break;
        }
        case DEVMENU_STAGELISTSEL: // Selecting Category
        {
            if (keyPress.down)
                gameMenu[0].selection2 += 2;
            if (keyPress.up)
                gameMenu[0].selection2 -= 2;

            if (gameMenu[0].selection2 > 9)
                gameMenu[0].selection2 = 3;

            if (gameMenu[0].selection2 < 3)
                gameMenu[0].selection2 = 9;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 80, 72);
            bool nextMenu = false;
            switch (gameMenu[0].selection2) {
                case 3: //Presentation
                    if (stageListCount[0] > 0)
                        nextMenu = true;
                    activeStageList = 0;
                    break;
                case 5: //Regular
                    if (stageListCount[1] > 0)
                        nextMenu = true;
                    activeStageList = 1;
                    break;
                case 7: //Special
                    if (stageListCount[3] > 0)
                        nextMenu = true;
                    activeStageList = 3;
                    break;
                case 9: //Bonus
                    if (stageListCount[2] > 0)
                        nextMenu = true;
                    activeStageList = 2;
                    break;
                default: break;
            }

            if ((keyPress.start || keyPress.A) && nextMenu) {
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE");
                SetupTextMenu(&gameMenu[1], 0);
                LoadConfigListText(&gameMenu[1], ((gameMenu[0].selection2 - 3) >> 1) + 1);
                gameMenu[1].alignment      = 1;
                gameMenu[1].selectionCount = 3;
                gameMenu[1].selection1     = 0;
                if (gameMenu[1].rowCount > 18)
                    gameMenu[1].visibleRowCount = 18;
                else
                    gameMenu[1].visibleRowCount = 0;

                gameMenu[0].alignment        = 2;
                gameMenu[0].selectionCount   = 1;
                gameMenu[1].timer            = 0;
                gameMenu[1].visibleRowOffset = 0;
                stageMode                  = DEVMENU_STAGESEL;
            }
            else if (keyPress.B) {
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                SetupTextMenu(&gameMenu[1], 0);
                LoadConfigListText(&gameMenu[1], 0);
                gameMenu[0].alignment      = 2;
                gameMenu[1].alignment      = 0;
                gameMenu[1].selectionCount  = 1;
                gameMenu[1].visibleRowCount = 0;
                gameMenu[1].selection1     = playerListPos;
                stageMode                  = DEVMENU_PLAYERSEL;
            }
            break;
        }
        case DEVMENU_STAGESEL: // Selecting Stage
        {
            if (keyDown.down) {
                gameMenu[1].timer += 1;
                if (gameMenu[1].timer > 8) {
                    gameMenu[1].timer = 0;
                    keyPress.down     = true;
                }
            }
            else {
                if (keyDown.up) {
                    gameMenu[1].timer -= 1;
                    if (gameMenu[1].timer < -8) {
                        gameMenu[1].timer = 0;
                        keyPress.up       = true;
                    }
                }
                else {
                    gameMenu[1].timer = 0;
                }
            }
            if (keyPress.down) {
                gameMenu[1].selection1++;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset >= gameMenu[1].visibleRowCount) {
                    gameMenu[1].visibleRowOffset += 1;
                }
            }
            if (keyPress.up) {
                gameMenu[1].selection1--;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset < 0) {
                    gameMenu[1].visibleRowOffset -= 1;
                }
            }
            if (gameMenu[1].selection1 == gameMenu[1].rowCount) {
                gameMenu[1].selection1       = 0;
                gameMenu[1].visibleRowOffset = 0;
            }
            if (gameMenu[1].selection1 < 0) {
                gameMenu[1].selection1       = gameMenu[1].rowCount - 1;
                gameMenu[1].visibleRowOffset = gameMenu[1].rowCount - gameMenu[1].visibleRowCount;
            }

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX + 100, 64);
            if (keyPress.start || keyPress.A) {
                debugMode         = keyDown.A;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
                stageListPosition = gameMenu[1].selection1;
            }
            else if (keyPress.B) {
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE LIST");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   PRESENTATION");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   REGULAR");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   SPECIAL");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "   BONUS");
                gameMenu[0].alignment      = 0;
                gameMenu[0].selection2     = (activeStageList << 1) + 3;
                gameMenu[0].selection2     = gameMenu[0].selection2 == 7 ? 9 : gameMenu[0].selection2 == 9 ? 7 : gameMenu[0].selection2;
                gameMenu[0].selectionCount = 2;
                stageMode                  = DEVMENU_STAGELISTSEL;
            }
            break;
        }
        case DEVMENU_SCRIPTERROR: // Script Error
        {
            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX, 72);
            if (keyPress.start || keyPress.A) {
                stageMode = DEVMENU_MAIN;
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
                AddTextMenuEntry(&gameMenu[0], " ");
                char version[0x80];
                StrCopy(version, Engine.gameWindowText);
                StrAdd(version, " Version");
                AddTextMenuEntry(&gameMenu[0], version);
                AddTextMenuEntry(&gameMenu[0], Engine.gameVersion);
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "START GAME");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "MODS");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "EXIT GAME");
                gameMenu[0].alignment        = 2;
                gameMenu[0].selectionCount   = 2;
                gameMenu[0].selection1       = 0;
                gameMenu[0].selection2       = 9;
                gameMenu[1].visibleRowCount  = 0;
                gameMenu[1].visibleRowOffset = 0;
            }
            else if (keyPress.B) {
                ClearGraphicsData();
                ClearAnimationData();
                LoadPalette("MasterPalette.act", 0, 0, 0, 256);
                activeStageList   = 0;
                stageMode         = DEVMENU_MAIN;
                Engine.gameMode   = ENGINE_MAINGAME;
                stageListPosition = 0;
            }
            else if (keyPress.C) {
                ClearGraphicsData();
                ClearAnimationData();
                stageMode       = STAGEMODE_LOAD;
                Engine.gameMode = ENGINE_MAINGAME;
            }
            break;
        }
        case DEVMENU_MODMENU: // Mod Menu
        {
            if (keyDown.down) {
                gameMenu[1].timer += 1;
                if (gameMenu[1].timer > 8) {
                    gameMenu[1].timer = 0;
                    keyPress.down     = true;
                }
            }
            else {
                if (keyDown.up) {
                    gameMenu[1].timer -= 1;
                    if (gameMenu[1].timer < -8) {
                        gameMenu[1].timer = 0;
                        keyPress.up       = true;
                    }
                }
                else {
                    gameMenu[1].timer = 0;
                }
            }
            if (keyPress.down) {
                gameMenu[1].selection1++;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset >= gameMenu[1].visibleRowCount) {
                    gameMenu[1].visibleRowOffset += 1;
                }
            }
            if (keyPress.up) {
                gameMenu[1].selection1--;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset < 0) {
                    gameMenu[1].visibleRowOffset -= 1;
                }
            }
            if (gameMenu[1].selection1 >= gameMenu[1].rowCount) {
                gameMenu[1].selection1       = 0;
                gameMenu[1].visibleRowOffset = 0;
            }
            if (gameMenu[1].selection1 < 0) {
                gameMenu[1].selection1       = gameMenu[1].rowCount - 1;
                gameMenu[1].visibleRowOffset = gameMenu[1].rowCount - gameMenu[1].visibleRowCount;
            }

            char buffer[0x100];
            if (keyPress.A || keyPress.start || keyPress.left || keyPress.right) {
                modList[gameMenu[1].selection1].active ^= 1; 
                StrCopy(buffer, modList[gameMenu[1].selection1].name.c_str());
                StrAdd(buffer, ": ");
                StrAdd(buffer, (modList[gameMenu[1].selection1].active ? "  Active" : "Inactive"));
                EditTextMenuEntry(&gameMenu[1], buffer, gameMenu[1].selection1);
            }

            if (keyPress.B) {
                stageMode = DEVMENU_MAIN;
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
                AddTextMenuEntry(&gameMenu[0], " ");
                char version[0x80];
                StrCopy(version, Engine.gameWindowText);
                StrAdd(version, " Version");
                AddTextMenuEntry(&gameMenu[0], version);
                AddTextMenuEntry(&gameMenu[0], Engine.gameVersion);
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "START GAME");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "MODS");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "EXIT GAME");
                gameMenu[0].alignment        = 2;
                gameMenu[0].selectionCount   = 2;
                gameMenu[0].selection1       = 0;
                gameMenu[0].selection2       = 9;
                gameMenu[1].visibleRowCount  = 0;
                gameMenu[1].visibleRowOffset = 0;

                //Reload entire engine
                Engine.LoadGameConfig("Data/Game/GameConfig.bin");

                ReleaseGlobalSfx();
                LoadGlobalSfx();

                forceUseScripts = false;
                for (int m = 0; m < modCount; ++m) {
                    if (modList[m].useScripts && modList[m].active)
                        forceUseScripts = true;
                }
                saveMods();
            }

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX + 100, 64);
        }
        default: break;
    }

#if RETRO_HARDWARE_RENDER
    gfxIndexSizeOpaque  = gfxIndexSize;
    gfxVertexSizeOpaque = gfxVertexSize;
#endif
}