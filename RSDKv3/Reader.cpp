#include "RetroEngine.hpp"
#include <string>

char rsdkName[0x400];

char fileName[0x100];
byte fileBuffer[0x2000];
int fileSize;
int vFileSize;
int readPos;
int readSize;
int bufferPosition;
int virtualFileOffset;
byte eStringPosA;
byte eStringPosB;
byte eStringNo;
byte eNybbleSwap;
char encryptionStringA[] = { "4RaS9D7KaEbxcp2o5r6t" };
char encryptionStringB[] = { "3tRaUxLmEaSn" };
#if RETRO_USE_MOD_LOADER
byte isModdedFile = false;
#endif

FileIO *cFileHandle = nullptr;

bool CheckRSDKFile(const char *filePath)
{
    FileInfo info;

    char filePathBuffer[0x100];
#if RETRO_PLATFORM == RETRO_OSX
    sprintf(filePathBuffer, "%s/%s", gamePath, filePath);
#else
    sprintf(filePathBuffer, "%s", filePath);
#endif

    Engine.usingDataFile = false;
#if !RETRO_USE_ORIGINAL_CODE
    Engine.usingDataFile_Config = false;
    Engine.usingDataFileStore   = false;
#endif
    Engine.usingBytecode = false;

    cFileHandle = fOpen(filePathBuffer, "rb");
    if (cFileHandle) {
        Engine.usingDataFile = true;
#if !RETRO_USE_ORIGINAL_CODE
        Engine.usingDataFile_Config = true;
#endif

        StrCopy(rsdkName, filePathBuffer);
        fClose(cFileHandle);
        cFileHandle = NULL;
        if (LoadFile("Data/Scripts/ByteCode/GlobalCode.bin", &info)) {
            Engine.usingBytecode = true;
            Engine.bytecodeMode  = BYTECODE_MOBILE;
            CloseFile();
        }
        else if (LoadFile("Data/Scripts/ByteCode/GS000.bin", &info)) {
            Engine.usingBytecode = true;
            Engine.bytecodeMode  = BYTECODE_PC;
            CloseFile();
        }
        return true;
    }
    else {
        Engine.usingDataFile = false;
#if !RETRO_USE_ORIGINAL_CODE
        Engine.usingDataFile_Config = false;
#endif

        cFileHandle = NULL;
        if (LoadFile("Data/Scripts/ByteCode/GlobalCode.bin", &info)) {
            Engine.usingBytecode = true;
            Engine.bytecodeMode  = BYTECODE_MOBILE;
            CloseFile();
        }
        else if (LoadFile("Data/Scripts/ByteCode/GS000.bin", &info)) {
            Engine.usingBytecode = true;
            Engine.bytecodeMode  = BYTECODE_PC;
            CloseFile();
        }
        return false;
    }

    return false;
}

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool LoadFile(const char *filePath, FileInfo *fileInfo)
{
    MEM_ZEROP(fileInfo);

    if (cFileHandle)
        fClose(cFileHandle);

    cFileHandle = NULL;

    char filePathBuf[0x100];
    StrCopy(filePathBuf, filePath);

    if (Engine.forceFolder)
        Engine.usingDataFile = Engine.usingDataFileStore;
    Engine.forceFolder = false;

    Engine.usingDataFileStore = Engine.usingDataFile;

#if RETRO_USE_MOD_LOADER
    fileInfo->isMod = false;
    isModdedFile    = false;
#endif
    bool addPath = true;
    // Fixes ".ani" ".Ani" bug and any other case differences
    char pathLower[0x100];
    memset(pathLower, 0, sizeof(char) * 0x100);
    for (int c = 0; c < strlen(filePathBuf); ++c) {
        pathLower[c] = tolower(filePathBuf[c]);
    }

#if RETRO_USE_MOD_LOADER
    int m = activeMod != -1 ? activeMod : 0;
 
    for (; m < modList.size(); ++m) {
        if (modList[m].active) {
            std::map<std::string, std::string>::const_iterator iter = modList[m].fileMap.find(pathLower);
            if (iter != modList[m].fileMap.cend()) {
                StrCopy(filePathBuf, iter->second.c_str());
                Engine.forceFolder   = true;
                Engine.usingDataFile = false;
                fileInfo->isMod      = true;
                isModdedFile         = true;
                addPath              = false;
                break;
            }
        }
        if (activeMod != -1)
            break;
    }

    if (forceUseScripts && !Engine.forceFolder) {
        if (std::string(filePathBuf).rfind("Data/Scripts/", 0) == 0 && ends_with(std::string(filePathBuf), "txt")) {
            // is a script, since those dont exist normally, load them from "scripts/"
            Engine.forceFolder   = true;
            Engine.usingDataFile = false;
            fileInfo->isMod      = true;
            isModdedFile         = true;
            addPath              = true;
            std::string fStr     = std::string(filePathBuf);
            fStr.erase(fStr.begin(), fStr.begin() + 5); // remove "Data/"
            StrCopy(filePathBuf, fStr.c_str());
        }
    }
#endif

#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    if (addPath) {
        char pathBuf[0x100];
        sprintf(pathBuf, "%s/%s", gamePath, filePathBuf);
        sprintf(filePathBuf, "%s", pathBuf);
    }
#endif

    StrCopy(fileInfo->fileName, "");
    StrCopy(fileName, "");

    if (Engine.usingDataFile && !Engine.forceFolder) {
        cFileHandle = fOpen(rsdkName, "rb");
        fSeek(cFileHandle, 0, SEEK_END);
        fileSize       = (int)fTell(cFileHandle);
        vFileSize      = fileSize;
        bufferPosition = 0;
        readSize       = 0;
        readPos        = 0;

        StrCopy(fileInfo->fileName, filePath);
        StrCopy(fileName, filePath);
        if (!ParseVirtualFileSystem(fileInfo)) {
            fClose(cFileHandle);
            cFileHandle = NULL;
            PrintLog("Couldn't load file '%s'", filePath);
            return false;
        }
        fileInfo->readPos           = readPos;
        fileInfo->fileSize          = vFileSize;
        fileInfo->vFileSize         = vFileSize;
        fileInfo->virtualFileOffset = virtualFileOffset;
        fileInfo->eStringNo         = eStringNo;
        fileInfo->eStringPosB       = eStringPosB;
        fileInfo->eStringPosA       = eStringPosA;
        fileInfo->eNybbleSwap       = eNybbleSwap;
        fileInfo->bufferPosition    = bufferPosition;
    }
    else {
        StrCopy(fileInfo->fileName, filePathBuf);
        StrCopy(fileName, filePathBuf);
        cFileHandle = fOpen(fileInfo->fileName, "rb");
        if (!cFileHandle) {
            PrintLog("Couldn't load file '%s'", filePathBuf);
            return false;
        }

        StrCopy(fileInfo->fileName, filePathBuf);
        StrCopy(fileName, filePathBuf);
        virtualFileOffset = 0;
        fSeek(cFileHandle, 0, SEEK_END);
        fileInfo->fileSize  = (int)fTell(cFileHandle);
        fileInfo->vFileSize = fileInfo->fileSize;
        fileSize            = fileInfo->fileSize;
        vFileSize           = fileInfo->fileSize;
        fSeek(cFileHandle, 0, SEEK_SET);
        readPos                     = 0;
        fileInfo->readPos           = readPos;
        fileInfo->virtualFileOffset = 0;
        fileInfo->eStringNo         = 0;
        fileInfo->eStringPosB       = 0;
        fileInfo->eStringPosA       = 0;
        fileInfo->eNybbleSwap       = 0;
        fileInfo->bufferPosition    = 0;
    }
    bufferPosition = 0;
    readSize       = 0;

    PrintLog("Loaded File '%s'", filePathBuf);

    return true;
}

bool ParseVirtualFileSystem(FileInfo *fileInfo)
{
    char filename[0x50];
    char fullFilename[0x50];
    char stringBuffer[0x50];
    ushort dirCount = 0;
    int fileOffset  = 0;
    int fNamePos    = 0;
    int headerSize  = 0;
    int i           = 0;
    byte fileBuffer = 0;

    int j             = 0;
    virtualFileOffset = 0;
    for (int i = 0; fileInfo->fileName[i]; i++) {
        if (fileInfo->fileName[i] == '/') {
            fNamePos = i;
            j        = 0;
        }
        else {
            ++j;
        }
        fullFilename[i] = fileInfo->fileName[i];
    }
    ++fNamePos;
    for (i = 0; i < j; ++i) filename[i] = fileInfo->fileName[i + fNamePos];
    filename[j]            = 0;
    fullFilename[fNamePos] = 0;

    fSeek(cFileHandle, 0, SEEK_SET);
    Engine.usingDataFile = false;
    bufferPosition       = 0;
    readSize             = 0;
    readPos              = 0;

    FileRead(&fileBuffer, 1);
    headerSize = fileBuffer;
    FileRead(&fileBuffer, 1);
    headerSize += fileBuffer << 8;
    FileRead(&fileBuffer, 1);
    headerSize += fileBuffer << 16;
    FileRead(&fileBuffer, 1);
    headerSize += fileBuffer << 24;

    FileRead(&fileBuffer, 1);
    dirCount = fileBuffer;
    FileRead(&fileBuffer, 1);
    dirCount += fileBuffer << 8;

    i                  = 0;
    fileOffset         = 0;
    int nextFileOffset = 0;
    while (i < dirCount) {
        FileRead(&fileBuffer, 1);
        for (j = 0; j < fileBuffer; ++j) {
            FileRead(&stringBuffer[j], 1);
            stringBuffer[j] ^= -1 - fileBuffer;
        }
        stringBuffer[j] = 0;

        if (StrComp(fullFilename, stringBuffer)) {
            FileRead(&fileBuffer, 1);
            fileOffset = fileBuffer;
            FileRead(&fileBuffer, 1);
            fileOffset += fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            fileOffset += fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            fileOffset += fileBuffer << 24;

            // Grab info for next dir to know when we've found an error
            // Ignore dir name we dont care
            if (i == dirCount - 1) {
                nextFileOffset = fileSize - headerSize; // There is no next dir, so just make this the EOF
            }
            else {
                FileRead(&fileBuffer, 1);
                for (j = 0; j < fileBuffer; ++j) {
                    FileRead(&stringBuffer[j], 1);
                    stringBuffer[j] ^= -1 - fileBuffer;
                }
                stringBuffer[j] = 0;

                FileRead(&fileBuffer, 1);
                nextFileOffset = fileBuffer;
                FileRead(&fileBuffer, 1);
                nextFileOffset += fileBuffer << 8;
                FileRead(&fileBuffer, 1);
                nextFileOffset += fileBuffer << 16;
                FileRead(&fileBuffer, 1);
                nextFileOffset += fileBuffer << 24;
            }

            i = dirCount;
        }
        else {
            fileOffset = -1;
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            ++i;
        }
    }

    if (fileOffset == -1) {
        Engine.usingDataFile = true;
        return false;
    }
    else {
        fSeek(cFileHandle, fileOffset + headerSize, SEEK_SET);
        bufferPosition    = 0;
        readSize          = 0;
        readPos           = 0;
        virtualFileOffset = fileOffset + headerSize;
        i                 = 0;
        while (i < 1) {
            FileRead(&fileBuffer, 1);
            ++virtualFileOffset;
            j = 0;
            while (j < fileBuffer) {
                FileRead(&stringBuffer[j], 1);
                stringBuffer[j] = ~stringBuffer[j];
                ++j;
                ++virtualFileOffset;
            }
            stringBuffer[j] = 0;

            if (StrComp(filename, stringBuffer)) {
                i = 1;
                FileRead(&fileBuffer, 1);
                j = fileBuffer;
                FileRead(&fileBuffer, 1);
                j += fileBuffer << 8;
                FileRead(&fileBuffer, 1);
                j += fileBuffer << 16;
                FileRead(&fileBuffer, 1);
                j += fileBuffer << 24;
                virtualFileOffset += 4;
                vFileSize = j;
            }
            else {
                FileRead(&fileBuffer, 1);
                j = fileBuffer;
                FileRead(&fileBuffer, 1);
                j += fileBuffer << 8;
                FileRead(&fileBuffer, 1);
                j += fileBuffer << 16;
                FileRead(&fileBuffer, 1);
                j += fileBuffer << 24;
                virtualFileOffset += 4;
                virtualFileOffset += j;
            }

            // No File has been found (next file would be in a new dir)
            if (virtualFileOffset >= nextFileOffset + headerSize) {
                Engine.usingDataFile = true;
                return false;
            }
            fSeek(cFileHandle, virtualFileOffset, SEEK_SET);
            bufferPosition = 0;
            readSize       = 0;
            readPos        = virtualFileOffset;
        }
        eStringNo            = (vFileSize & 0x1FC) >> 2;
        eStringPosB          = (eStringNo % 9) + 1;
        eStringPosA          = (eStringNo % eStringPosB) + 1;
        eNybbleSwap          = false;
        Engine.usingDataFile = true;
        return true;
    }
    // Engine.usingDataFile = true;
    return false;
}

void FileRead(void *dest, int size)
{
    byte *data = (byte *)dest;

    if (readPos <= fileSize) {
        if (Engine.usingDataFile && !Engine.forceFolder) {
            while (size > 0) {
                if (bufferPosition == readSize)
                    FillFileBuffer();

                *data = fileBuffer[bufferPosition++];
                *data ^= encryptionStringB[eStringPosB++] ^ eStringNo;
                if (eNybbleSwap)
                    *data = ((*data & 0xF) << 4) | ((signed int)*data >> 4);
                *data ^= encryptionStringA[eStringPosA++];

                if (eStringPosA <= 19 || eStringPosB <= 11) {
                    if (eStringPosA > 19) {
                        eStringPosA = 1;
                        eNybbleSwap ^= 1;
                    }
                    if (eStringPosB > 11) {
                        eStringPosB = 1;
                        eNybbleSwap ^= 1;
                    }
                }
                else {
                    ++eStringNo;
                    eStringNo &= 0x7F;
                    if (eNybbleSwap) {
                        eNybbleSwap = 0;
                        eStringPosA = (eStringNo % 12) + 6;
                        eStringPosB = (eStringNo % 5) + 4;
                    }
                    else {
                        eNybbleSwap = 1;
                        eStringPosA = (eStringNo % 15) + 3;
                        eStringPosB = (eStringNo % 7) + 1;
                    }
                }
                ++data;
                --size;
            }
        }
        else {
            while (size > 0) {
                if (bufferPosition == readSize)
                    FillFileBuffer();

                *data++ = fileBuffer[bufferPosition++];
                size--;
            }
        }
    }
    else {
        printf("");
    }
}

void SetFileInfo(FileInfo *fileInfo)
{
    Engine.forceFolder = false;
#if RETRO_USE_MOD_LOADER
    if (!fileInfo->isMod) {
#endif
        Engine.usingDataFile = Engine.usingDataFileStore;
#if RETRO_USE_MOD_LOADER
    }
    else {
        Engine.forceFolder = true;
    }
#endif

#if RETRO_USE_MOD_LOADER
    isModdedFile = fileInfo->isMod;
#endif
    if (Engine.usingDataFile && !Engine.forceFolder) {
        cFileHandle       = fOpen(rsdkName, "rb");
        virtualFileOffset = fileInfo->virtualFileOffset;
        vFileSize         = fileInfo->fileSize;
        fSeek(cFileHandle, 0, SEEK_END);
        fileSize = (int)fTell(cFileHandle);
        readPos  = fileInfo->readPos;
        fSeek(cFileHandle, readPos, SEEK_SET);
        FillFileBuffer();
        bufferPosition = fileInfo->bufferPosition;
        eStringPosA    = fileInfo->eStringPosA;
        eStringPosB    = fileInfo->eStringPosB;
        eStringNo      = fileInfo->eStringNo;
        eNybbleSwap    = fileInfo->eNybbleSwap;
    }
    else {
        StrCopy(fileName, fileInfo->fileName);
        cFileHandle       = fOpen(fileInfo->fileName, "rb");
        virtualFileOffset = 0;
        fileSize          = fileInfo->fileSize;
        readPos           = fileInfo->readPos;
        fSeek(cFileHandle, readPos, SEEK_SET);
        FillFileBuffer();
        bufferPosition = fileInfo->bufferPosition;
        eStringPosA    = 0;
        eStringPosB    = 0;
        eStringNo      = 0;
        eNybbleSwap    = 0;
    }
}

size_t GetFilePosition()
{
    if (Engine.usingDataFile)
        return bufferPosition + readPos - readSize - virtualFileOffset;
    else
        return bufferPosition + readPos - readSize;
}

void SetFilePosition(int newPos)
{
    if (Engine.usingDataFile) {
        readPos     = virtualFileOffset + newPos;
        eStringNo   = (vFileSize & 0x1FCu) >> 2;
        eStringPosB = (eStringNo % 9) + 1;
        eStringPosA = (eStringNo % eStringPosB) + 1;
        eNybbleSwap = false;
        while (newPos) {
            ++eStringPosA;
            ++eStringPosB;
            if (eStringPosA <= 19 || eStringPosB <= 11) {
                if (eStringPosA > 19) {
                    eStringPosA = 1;
                    eNybbleSwap ^= 1u;
                }
                if (eStringPosB > 11) {
                    eStringPosB = 1;
                    eNybbleSwap ^= 1u;
                }
            }
            else {
                ++eStringNo;
                eStringNo &= 0x7Fu;
                if (eNybbleSwap) {
                    eNybbleSwap = false;
                    eStringPosA = (eStringNo % 12) + 6;
                    eStringPosB = (eStringNo % 5) + 4;
                }
                else {
                    eNybbleSwap = true;
                    eStringPosA = (eStringNo % 15) + 3;
                    eStringPosB = (eStringNo % 7) + 1;
                }
            }
            --newPos;
        }
    }
    else {
        readPos = newPos;
    }
    fSeek(cFileHandle, readPos, SEEK_SET);
    FillFileBuffer();
}

bool ReachedEndOfFile()
{
    if (Engine.usingDataFile)
        return bufferPosition + readPos - readSize - virtualFileOffset >= vFileSize;
    else
        return bufferPosition + readPos - readSize >= fileSize;
}
