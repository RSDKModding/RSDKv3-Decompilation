#include "RetroEngine.hpp"

int touchFlags = 0;

void InitDevMenu()
{
#if RETRO_USE_MOD_LOADER
    for (int m = 0; m < modList.size(); ++m) ScanModFolder(&modList[m]);
#endif
    drawStageGFXHQ = false;
    xScrollOffset  = 0;
    yScrollOffset  = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode        = 0;
    playerListPos   = 0;
    Engine.gameMode = ENGINE_DEVMENU;
    ClearGraphicsData();
    ClearAnimationData();
    LoadPalette("MasterPalette.act", 0, 0, 0, 256);
#if RETRO_USE_MOD_LOADER
    Engine.LoadXMLPalettes();
#endif
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
#ifdef RETRO_DEV_EXTRA
    AddTextMenuEntry(&gameMenu[0], RETRO_DEV_EXTRA);
#else
    AddTextMenuEntry(&gameMenu[0], " ");
#endif
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], "START GAME");
    AddTextMenuEntry(&gameMenu[0], " ");
    AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
    AddTextMenuEntry(&gameMenu[0], " ");
#if RETRO_USE_MOD_LOADER
    AddTextMenuEntry(&gameMenu[0], "MODS");
    AddTextMenuEntry(&gameMenu[0], " ");
#endif
    AddTextMenuEntry(&gameMenu[0], "EXIT GAME");
    gameMenu[0].alignment        = 2;
    gameMenu[0].selectionCount   = 2;
    gameMenu[0].selection1       = 0;
    gameMenu[0].selection2       = 9;
    gameMenu[1].visibleRowCount  = 0;
    gameMenu[1].visibleRowOffset = 0;
    if (renderType == RENDER_HW) {
        Engine.highResMode = false;
        render3DEnabled    = false;
        UpdateHardwareTextures();
    }
}
void InitErrorMessage()
{
    drawStageGFXHQ = false;
    xScrollOffset  = 0;
    yScrollOffset  = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode        = 0;
    Engine.gameMode = ENGINE_DEVMENU;
    ClearGraphicsData();
    ClearAnimationData();
    LoadPalette("MasterPalette.act", 0, 0, 0, 256);
#if RETRO_USE_MOD_LOADER
    Engine.LoadXMLPalettes();
#endif
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
    if (renderType == RENDER_HW) {
        Engine.highResMode = false;
        render3DEnabled    = false;
        UpdateHardwareTextures();
    }
}
void ProcessStageSelect()
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
        int tFlags = touchFlags;
        touchFlags = 0;

        for (int t = 0; t < touches; ++t) {
            if (touchDown[t]) {
                if (touchX[t] < SCREEN_CENTERX) {
                    if (touchY[t] >= SCREEN_CENTERY) {
                        if (!(tFlags & 2))
                            keyPress.down = true;
                        else
                            touchFlags |= 1 << 1;
                    }
                    else {
                        if (!(tFlags & 1))
                            keyPress.up = true;
                        else
                            touchFlags |= 1 << 0;
                    }
                }
                else if (touchX[t] > SCREEN_CENTERX) {
                    if (touchY[t] > SCREEN_CENTERY) {
                        if (!(tFlags & 4))
                            keyPress.start = true;
                        else
                            touchFlags |= 1 << 2;
                    }
                    else {
                        if (!(tFlags & 8))
                            keyPress.B = true;
                        else
                            touchFlags |= 1 << 3;
                    }
                }
            }
        }

        touchFlags |= (int)keyPress.up << 0;
        touchFlags |= (int)keyPress.down << 1;
        touchFlags |= (int)keyPress.start << 2;
        touchFlags |= (int)keyPress.B << 3;
    }

    switch (stageMode) {
        case DEVMENU_MAIN: // Main Menu
        {
            if (keyPress.down)
                gameMenu[0].selection2 += 2;

            if (keyPress.up)
                gameMenu[0].selection2 -= 2;

            int count = 13;
#if RETRO_USE_MOD_LOADER
            count += 2;
#endif

            if (gameMenu[0].selection2 > count)
                gameMenu[0].selection2 = 9;
            if (gameMenu[0].selection2 < 9)
                gameMenu[0].selection2 = count;

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX, 72);
            if (keyPress.start || keyPress.A) {
                if (gameMenu[0].selection2 == 9) {
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
#if RETRO_USE_MOD_LOADER
                else if (gameMenu[0].selection2 == 13) {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "MOD LIST");
                    SetupTextMenu(&gameMenu[1], 0);
                    InitMods();

                    char buffer[0x100];
                    for (int m = 0; m < modList.size(); ++m) {
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
                    stageMode                    = DEVMENU_MODMENU;
                }
#endif
                else {
                    Engine.running = false;
                }
            }
            else if (keyPress.B) {
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
#ifdef RETRO_DEV_EXTRA
                AddTextMenuEntry(&gameMenu[0], RETRO_DEV_EXTRA);
#else
                AddTextMenuEntry(&gameMenu[0], " ");
#endif
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "START GAME");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
                AddTextMenuEntry(&gameMenu[0], " ");
#if RETRO_USE_MOD_LOADER
                AddTextMenuEntry(&gameMenu[0], "MODS");
                AddTextMenuEntry(&gameMenu[0], " ");
#endif
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
                case 3: // Presentation
                    if (stageListCount[0] > 0)
                        nextMenu = true;
                    activeStageList = 0;
                    break;
                case 5: // Regular
                    if (stageListCount[1] > 0)
                        nextMenu = true;
                    activeStageList = 1;
                    break;
                case 7: // Special
                    if (stageListCount[3] > 0)
                        nextMenu = true;
                    activeStageList = 3;
                    break;
                case 9: // Bonus
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
                stageMode                    = DEVMENU_STAGESEL;
            }
            else if (keyPress.B) {
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "SELECT A PLAYER");
                SetupTextMenu(&gameMenu[1], 0);
                LoadConfigListText(&gameMenu[1], 0);
                gameMenu[0].alignment       = 2;
                gameMenu[1].alignment       = 0;
                gameMenu[1].selectionCount  = 1;
                gameMenu[1].visibleRowCount = 0;
                gameMenu[1].selection1      = playerListPos;
                stageMode                   = DEVMENU_PLAYERSEL;
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
#ifdef RETRO_DEV_EXTRA
                AddTextMenuEntry(&gameMenu[0], RETRO_DEV_EXTRA);
#else
                AddTextMenuEntry(&gameMenu[0], " ");
#endif
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "START GAME");
                AddTextMenuEntry(&gameMenu[0], " ");
                AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
                AddTextMenuEntry(&gameMenu[0], " ");
#if RETRO_USE_MOD_LOADER
                AddTextMenuEntry(&gameMenu[0], "MODS");
                AddTextMenuEntry(&gameMenu[0], " ");
#endif
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
#if RETRO_USE_MOD_LOADER
                Engine.LoadXMLPalettes();
#endif
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

#if RETRO_USE_MOD_LOADER
        case DEVMENU_MODMENU: // Mod Menu
        {
            int preOption = gameMenu[1].selection1;
            if (keyDown.down) {
                gameMenu[1].timer++;
                if (gameMenu[1].timer > 8) {
                    gameMenu[1].timer = 0;
                    keyPress.down     = true;
                }
            }
            else {
                if (keyDown.up) {
                    gameMenu[1].timer--;
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
                    gameMenu[1].visibleRowOffset++;
                }
            }

            if (keyPress.up) {
                gameMenu[1].selection1--;
                if (gameMenu[1].selection1 - gameMenu[1].visibleRowOffset < 0 && gameMenu[1].visibleRowOffset > 0) {
                    gameMenu[1].visibleRowOffset--;
                }
            }

            if (gameMenu[1].selection1 >= gameMenu[1].rowCount) {
                if (keyDown.C) {
                    gameMenu[1].selection1--;
                    gameMenu[1].visibleRowOffset--;
                }
                else {
                    gameMenu[1].selection1       = 0;
                    gameMenu[1].visibleRowOffset = 0;
                }
            }

            if (gameMenu[1].selection1 < 0) {
                if (keyDown.C) {
                    gameMenu[1].selection1++;
                }
                else {
                    gameMenu[1].selection1       = gameMenu[1].rowCount - 1;
                    gameMenu[1].visibleRowOffset = gameMenu[1].rowCount - gameMenu[1].visibleRowCount;
                }
            }
            gameMenu[1].selection2 = gameMenu[1].selection1; // its a bug fix LOL

            char buffer[0x100];
            if (gameMenu[1].selection1 < modList.size() && (keyPress.A || keyPress.start || keyPress.left || keyPress.right)) {
                modList[gameMenu[1].selection1].active ^= 1;
                StrCopy(buffer, modList[gameMenu[1].selection1].name.c_str());
                StrAdd(buffer, ": ");
                StrAdd(buffer, (modList[gameMenu[1].selection1].active ? "  Active" : "Inactive"));
                EditTextMenuEntry(&gameMenu[1], buffer, gameMenu[1].selection1);
            }

            if (keyDown.C && gameMenu[1].selection1 != preOption) {
                int visibleOffset  = gameMenu[1].visibleRowOffset;
                int option         = gameMenu[1].selection1;
                ModInfo swap       = modList[preOption];
                modList[preOption] = modList[option];
                modList[option]    = swap;

                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "MOD LIST");
                SetupTextMenu(&gameMenu[1], 0);

                char buffer[0x100];
                for (int m = 0; m < modList.size(); ++m) {
                    StrCopy(buffer, modList[m].name.c_str());
                    StrAdd(buffer, ": ");
                    StrAdd(buffer, modList[m].active ? "  Active" : "Inactive");
                    AddTextMenuEntry(&gameMenu[1], buffer);
                }

                gameMenu[1].alignment      = 1;
                gameMenu[1].selectionCount = 3;
                gameMenu[1].selection1     = option;
                if (gameMenu[1].rowCount > 18)
                    gameMenu[1].visibleRowCount = 18;
                else
                    gameMenu[1].visibleRowCount = 0;

                gameMenu[0].alignment        = 2;
                gameMenu[0].selectionCount   = 1;
                gameMenu[1].timer            = 0;
                gameMenu[1].visibleRowOffset = visibleOffset;
            }
            else if (keyPress.B) {
                RefreshEngine();

                stageMode = DEVMENU_MAIN;
                SetupTextMenu(&gameMenu[0], 0);
                AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
                AddTextMenuEntry(&gameMenu[0], " ");
                char version[0x80];
                StrCopy(version, Engine.gameWindowText);
                StrAdd(version, " Version");
                AddTextMenuEntry(&gameMenu[0], version);
                AddTextMenuEntry(&gameMenu[0], Engine.gameVersion);
#ifdef RETRO_DEV_EXTRA
                AddTextMenuEntry(&gameMenu[0], RETRO_DEV_EXTRA);
#else
                AddTextMenuEntry(&gameMenu[0], " ");
#endif
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

            DrawTextMenu(&gameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&gameMenu[1], SCREEN_CENTERX + 100, 64);
            break;
        }
#endif

        default: break;
    }

    if (renderType == RENDER_HW) {
        gfxIndexSizeOpaque  = gfxIndexSize;
        gfxVertexSizeOpaque = gfxVertexSize;
    }
}
