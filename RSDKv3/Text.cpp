#include "RetroEngine.hpp"

TextMenu gameMenu[TEXTMENU_COUNT];
int textMenuSurfaceNo = 0;

FontCharacter fontCharacterList[FONTCHAR_COUNT];

void LoadFontFile(const char *filePath)
{
    byte fileBuffer = 0;
    int cnt         = 0;
    FileInfo info;
    if (LoadFile(filePath, &info)) {
        while (!ReachedEndOfFile()) {
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id += fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id += fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].id += fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcX = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcX += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcY = fileBuffer;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].srcY += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].width = fileBuffer + 1;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].width += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].height = fileBuffer + 1;
            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].height += fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].pivotX = fileBuffer;
            FileRead(&fileBuffer, 1);
            if (fileBuffer > 0x80) {
                fontCharacterList[cnt].pivotX += (fileBuffer - 0x80) << 8;
                fontCharacterList[cnt].pivotX += -0x8000;
            }
            else {
                fontCharacterList[cnt].pivotX += fileBuffer << 8;
            }

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].pivotY = fileBuffer;
            FileRead(&fileBuffer, 1);
            if (fileBuffer > 0x80) {
                fontCharacterList[cnt].pivotY += (fileBuffer - 0x80) << 8;
                fontCharacterList[cnt].pivotY += -0x8000;
            }
            else {
                fontCharacterList[cnt].pivotY += fileBuffer << 8;
            }

            FileRead(&fileBuffer, 1);
            fontCharacterList[cnt].xAdvance = fileBuffer;
            FileRead(&fileBuffer, 1);
            if (fileBuffer > 0x80) {
                fontCharacterList[cnt].xAdvance += (fileBuffer - 0x80) << 8;
                fontCharacterList[cnt].xAdvance += -0x8000;
            }
            else {
                fontCharacterList[cnt].xAdvance += fileBuffer << 8;
            }

            // Unused
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            cnt++;
        }
        CloseFile();
    }
}
void LoadTextFile(TextMenu *menu, const char *filePath, byte mapCode)
{
    bool finished = false;
    FileInfo info;
    byte fileBuffer = 0;
    if (LoadFile(filePath, &info)) {
        menu->textDataPos                = 0;
        menu->rowCount                   = 0;
        menu->entryStart[menu->rowCount] = menu->textDataPos;
        menu->entrySize[menu->rowCount]  = 0;

        FileRead(&fileBuffer, 1);
        if (fileBuffer == 0xFF) {
            FileRead(&fileBuffer, 1);
            while (!finished) {
                ushort character = 0;
                FileRead(&fileBuffer, 1);
                character = fileBuffer;
                FileRead(&fileBuffer, 1);
                character |= fileBuffer << 8;

                if (character != '\n') {
                    if (character == '\r') {
                        menu->rowCount += 1;
                        if (menu->rowCount > 511) {
                            finished = true;
                        }
                        else {
                            menu->entryStart[menu->rowCount] = menu->textDataPos;
                            menu->entrySize[menu->rowCount]  = 0;
                        }
                    }
                    else {
                        if (mapCode) {
                            int i = 0;
                            while (i < 1024) {
                                if (fontCharacterList[i].id == character) {
                                    character = i;
                                    i         = 1025;
                                }
                                else {
                                    ++i;
                                }
                            }
                            if (i == 1024) {
                                character = 0;
                            }
                        }
                        menu->textData[menu->textDataPos++] = character;
                        menu->entrySize[menu->rowCount]++;
                    }
                }
                if (!finished) {
                    finished = ReachedEndOfFile();
                    if (menu->textDataPos >= TEXTDATA_COUNT)
                        finished = true;
                }
            }
        }
        else {
            ushort character = fileBuffer;
            if (character != '\n') {
                if (character == '\r') {
                    menu->rowCount++;
                    menu->entryStart[menu->rowCount] = menu->textDataPos;
                    menu->entrySize[menu->rowCount]  = 0;
                }
                else {
                    if (mapCode) {
                        int i = 0;
                        while (i < 1024) {
                            if (fontCharacterList[i].id == character) {
                                character = i;
                                i         = 1025;
                            }
                            else {
                                ++i;
                            }
                        }
                        if (i == 1024) {
                            character = 0;
                        }
                    }
                    menu->textData[menu->textDataPos++] = character;
                    menu->entrySize[menu->rowCount]++;
                }
            }

            while (!finished) {
                FileRead(&fileBuffer, 1);
                character = fileBuffer;
                if (character != '\n') {
                    if (character == '\r') {
                        menu->rowCount++;
                        if (menu->rowCount > 511) {
                            finished = true;
                        }
                        else {
                            menu->entryStart[menu->rowCount] = menu->textDataPos;
                            menu->entrySize[menu->rowCount]  = 0;
                        }
                    }
                    else {
                        if (mapCode) {
                            int i = 0;
                            while (i < 1024) {
                                if (fontCharacterList[i].id == character) {
                                    character = i;
                                    i         = 1025;
                                }
                                else {
                                    ++i;
                                }
                            }
                            if (i == 1024)
                                character = 0;
                        }
                        menu->textData[menu->textDataPos++] = character;
                        menu->entrySize[menu->rowCount]++;
                    }
                }
                if (!finished) {
                    finished = ReachedEndOfFile();
                    if (menu->textDataPos >= TEXTDATA_COUNT)
                        finished = true;
                }
            }
        }
        menu->rowCount++;
        CloseFile();
    }
}
void SetupTextMenu(TextMenu *menu, int rowCount)
{
    menu->textDataPos = 0;
    menu->rowCount    = rowCount;
}
void AddTextMenuEntry(TextMenu *menu, const char *text)
{
    menu->entryStart[menu->rowCount]     = menu->textDataPos;
    menu->entrySize[menu->rowCount]      = 0;
    menu->entryHighlight[menu->rowCount] = false;
    for (int i = 0; i < StrLength(text);) {
        if (text[i] != '\0') {
            menu->textData[menu->textDataPos++] = text[i];
            menu->entrySize[menu->rowCount]++;
            ++i;
        }
        else {
            break;
        }
    }
    menu->rowCount++;
}
void AddTextMenuEntryMapped(TextMenu *menu, char *text)
{
    menu->entryStart[menu->rowCount]     = menu->textDataPos;
    menu->entrySize[menu->rowCount]      = 0;
    menu->entryHighlight[menu->rowCount] = false;
    for (int i = 0; i < StrLength(text);) {
        if (text[i] != '\0') {
            ushort num = text[i];
            int j      = 0;
            while (j < 1024) {
                if (fontCharacterList[j].id == (int)num) {
                    num = j;
                    j   = 1025;
                }
                else {
                    ++j;
                }
            }
            if (j == 1024) {
                num = 0;
            }
            menu->textData[menu->textDataPos++] = (char)num;
            menu->entrySize[menu->rowCount]++;
            ++i;
        }
        else {
            break;
        }
    }
    menu->rowCount++;
}
void SetTextMenuEntry(TextMenu *menu, const char *text, int rowID)
{
    menu->entryStart[rowID]              = menu->textDataPos;
    menu->entrySize[rowID]               = 0;
    menu->entryHighlight[menu->rowCount] = false;
    for (int i = 0; i < StrLength(text);) {
        if (text[i] != '\0') {
            menu->textData[menu->textDataPos++] = text[i];
            menu->entrySize[rowID]++;
            ++i;
        }
        else {
            break;
        }
    }
}
void EditTextMenuEntry(TextMenu *menu, const char *text, int rowID)
{
    int entryPos                         = menu->entryStart[rowID];
    menu->entrySize[rowID]               = 0;
    menu->entryHighlight[menu->rowCount] = false;
    for (int i = 0; i < StrLength(text);) {
        if (text[i] != '\0') {
            menu->textData[entryPos++] = text[i];
            menu->entrySize[rowID]++;
            ++i;
        }
        else {
            break;
        }
    }
}
void LoadConfigListText(TextMenu *menu, int listNo)
{
    FileInfo info;
    char strBuf[0x100];
    byte fileBuffer = 0;
    byte count      = 0;
    byte strLen     = 0;
    if (LoadFile("Data/Game/GameConfig.bin", &info)) {
        // Name
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        // 'Data'
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        // About
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        // Object Names
        FileRead(&count, 1);
        for (int o = 0; o < count; ++o) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Script Paths
        for (int s = 0; s < count; ++s) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Variables
        FileRead(&count, 1);
        for (int v = 0; v < count; ++v) {
            // Var Name
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;

            // Var Value
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
        }

        // SFX
        FileRead(&count, 1);
        for (int s = 0; s < count; ++s) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Players
        FileRead(&count, 1);
        for (int p = 0; p < count; ++p) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = '\0';

            if (listNo == 0) // Player List
                AddTextMenuEntry(menu, strBuf);
        }

        // Categories
        for (int c = 1; c <= 4; ++c) {
            byte stageCnt = 0;
            FileRead(&stageCnt, 1);
            for (int s = 0; s < stageCnt; ++s) {
                // Stage Folder
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                // Stage ID
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                // Stage Name
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                // IsHighlighted
                FileRead(&fileBuffer, 1);
                if (listNo == c) {
                    AddTextMenuEntry(menu, strBuf);
                    menu->entryHighlight[menu->rowCount - 1] = fileBuffer;
                }
            }
        }
        CloseFile();

#if RETRO_USE_MOD_LOADER
        if (listNo == 0)
            Engine.LoadXMLPlayers(menu);
        else
            Engine.LoadXMLStages(menu, listNo);
#endif
    }
}
