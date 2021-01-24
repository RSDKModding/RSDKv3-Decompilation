#include "RetroEngine.hpp"
#include <string>

char rsdkName[0x400];

static const char encryptionStringA[] = { "4RaS9D7KaEbxcp2o5r6t" };
static const char encryptionStringB[] = { "3tRaUxLmEaSn" };

File cFile;
File cFileStream;

bool CheckRSDKFile(const char *filePath)
{
    FileInfo info;

    Engine.usingDataFile = false;
    Engine.usingBytecode = false;

    cFile.handle = fOpen(filePath, "rb");
    if (cFile.handle) {
        Engine.usingDataFile = true;
        StrCopy(rsdkName, filePath);
        fClose(cFile.handle);
    }
    else {
        Engine.usingDataFile = false;
    }

    cFile.handle = NULL;
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
    return Engine.usingDataFile;
}

bool LoadFile(const char *filePath, FileInfo *fileInfo)
{
    MEM_ZEROP(fileInfo);
    StrCopy(fileInfo->fileName, filePath);
    StrCopy(cFile.info.fileName, filePath);

    if (cFile.handle)
        fClose(cFile.handle);

    cFile.handle = NULL;

    if (Engine.usingDataFile) {
        cFile.handle = fOpen(rsdkName, "rb");
        fSeek(cFile.handle, 0, SEEK_END);
        cFile.info.fileSize       = (int)fTell(cFile.handle);
        cFile.info.bufferPosition = 0;
        cFile.readSize       = 0;
        cFile.info.readPos        = 0;
        if (!ParseVirtualFileSystem(fileInfo)) {
            fClose(cFile.handle);
            cFile.handle = NULL;
            printLog("Couldn't load file '%s'", filePath);
            return false;
        }
        fileInfo->readPos           = cFile.info.readPos;
        fileInfo->fileSize          = cFile.vFileSize;
        fileInfo->virtualFileOffset = cFile.info.virtualFileOffset;
        fileInfo->eStringNo         = cFile.info.eStringNo;
        fileInfo->eStringPosB       = cFile.info.eStringPosB;
        fileInfo->eStringPosA       = cFile.info.eStringPosA;
        fileInfo->eNybbleSwap       = cFile.info.eNybbleSwap;
        fileInfo->bufferPosition    = cFile.info.bufferPosition;
    }
    else {
        cFile.handle = fOpen(fileInfo->fileName, "rb");
        if (!cFile.handle) {
            printLog("Couldn't load file '%s'", filePath);
            return false;
        }
        cFile.info.virtualFileOffset = 0;
        fSeek(cFile.handle, 0, SEEK_END);
        fileInfo->fileSize = (int)fTell(cFile.handle);
        cFile.info.fileSize           = fileInfo->fileSize;
        fSeek(cFile.handle, 0, SEEK_SET);
        cFile.info.readPos = 0;
        fileInfo->readPos           = cFile.info.readPos;
        fileInfo->virtualFileOffset = 0;
        fileInfo->eStringNo         = 0;
        fileInfo->eStringPosB       = 0;
        fileInfo->eStringPosA       = 0;
        fileInfo->eNybbleSwap       = 0;
        fileInfo->bufferPosition    = 0;
    }
    cFile.info.bufferPosition = 0;
    cFile.readSize       = 0;

    printLog("Loaded File '%s'", filePath);

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
    cFile.info.virtualFileOffset = 0;
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

    fSeek(cFile.handle, 0, SEEK_SET);
    Engine.usingDataFile = false;
    cFile.info.bufferPosition       = 0;
    cFile.readSize             = 0;
    cFile.info.readPos              = 0;

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

    i          = 0;
    fileOffset = 0;
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

            //Grab info for next dir to know when we've found an error
            //Ignore dir name we dont care
            if (i == dirCount - 1) {
                nextFileOffset = cFile.info.fileSize - headerSize; //There is no next dir, so just make this the EOF
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
        fSeek(cFile.handle, fileOffset + headerSize, SEEK_SET);
        cFile.info.bufferPosition    = 0;
        cFile.readSize          = 0;
        cFile.info.readPos           = 0;
        cFile.info.virtualFileOffset = fileOffset + headerSize;
        i                 = 0;
        while (i < 1) {
            FileRead(&fileBuffer, 1);
            ++cFile.info.virtualFileOffset;
            j = 0;
            while (j < fileBuffer) {
                FileRead(&stringBuffer[j], 1);
                stringBuffer[j] = ~stringBuffer[j];
                ++j;
                ++cFile.info.virtualFileOffset;
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
                cFile.info.virtualFileOffset += 4;
                cFile.vFileSize = j;
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
                cFile.info.virtualFileOffset += 4;
                cFile.info.virtualFileOffset += j;
            }

            //No File has been found (next file would be in a new dir)
            if (cFile.info.virtualFileOffset >= nextFileOffset + headerSize) {
                Engine.usingDataFile = true;
                return false;
            }
            fSeek(cFile.handle, cFile.info.virtualFileOffset, SEEK_SET);
            cFile.info.bufferPosition = 0;
            cFile.readSize       = 0;
            cFile.info.readPos        = cFile.info.virtualFileOffset;
        }
        cFile.info.eStringNo            = (cFile.vFileSize & 0x1FCu) >> 2;
        cFile.info.eStringPosB          = (cFile.info.eStringNo % 9) + 1;
        cFile.info.eStringPosA          = (cFile.info.eStringNo % cFile.info.eStringPosB) + 1;
        cFile.info.eNybbleSwap          = false;
        Engine.usingDataFile = true;
        return true;
    }
    //Engine.usingDataFile = true;
    return false;
}

void FileRead(void *dest, int size)
{
    byte *data = (byte *)dest;

    if (cFile.info.readPos <= cFile.info.fileSize) {
        if (Engine.usingDataFile) {
            while (size > 0) {
                if (cFile.info.bufferPosition == cFile.readSize)
                    FillFileBuffer();

                *data = encryptionStringB[cFile.info.eStringPosB] ^ cFile.info.eStringNo ^ cFile.fileBuffer[cFile.info.bufferPosition++];
                if (cFile.info.eNybbleSwap)
                    *data = 16 * (*data & 0xF) + ((signed int)*data >> 4);
                *data ^= encryptionStringA[cFile.info.eStringPosA++];
                ++cFile.info.eStringPosB;
                if (cFile.info.eStringPosA <= 19 || cFile.info.eStringPosB <= 11) {
                    if (cFile.info.eStringPosA > 19) {
                        cFile.info.eStringPosA = 1;
                        cFile.info.eNybbleSwap ^= 1u;
                    }
                    if (cFile.info.eStringPosB > 11) {
                        cFile.info.eStringPosB = 1;
                        cFile.info.eNybbleSwap ^= 1u;
                    }
                }
                else {
                    ++cFile.info.eStringNo;
                    cFile.info.eStringNo &= 0x7Fu;
                    if (cFile.info.eNybbleSwap) {
                        cFile.info.eNybbleSwap = 0;
                        cFile.info.eStringPosA = (cFile.info.eStringNo % 12) + 6;
                        cFile.info.eStringPosB = (cFile.info.eStringNo % 5) + 4;
                    }
                    else {
                        cFile.info.eNybbleSwap = 1;
                        cFile.info.eStringPosA = (cFile.info.eStringNo % 15) + 3;
                        cFile.info.eStringPosB = (cFile.info.eStringNo % 7) + 1;
                    }
                }
                ++data;
                --size;
            }
        }
        else {
            while (size > 0) {
                if (cFile.info.bufferPosition == cFile.readSize)
                    FillFileBuffer();

                *data++ = cFile.fileBuffer[cFile.info.bufferPosition++];
                size--;
            }
        }
    }
}

void SetFileInfo(FileInfo *fileInfo)
{
    if (Engine.usingDataFile) {
        cFile.handle       = fOpen(rsdkName, "rb");
        cFile.info.virtualFileOffset = fileInfo->virtualFileOffset;
        cFile.vFileSize         = fileInfo->fileSize;
        fSeek(cFile.handle, 0, SEEK_END);
        cFile.info.fileSize = (int)fTell(cFile.handle);
        cFile.info.readPos  = fileInfo->readPos;
        fSeek(cFile.handle, cFile.info.readPos, SEEK_SET);
        FillFileBuffer();
        cFile.info.bufferPosition = fileInfo->bufferPosition;
        cFile.info.eStringPosA    = fileInfo->eStringPosA;
        cFile.info.eStringPosB    = fileInfo->eStringPosB;
        cFile.info.eStringNo      = fileInfo->eStringNo;
        cFile.info.eNybbleSwap    = fileInfo->eNybbleSwap;
    }
    else {
        StrCopy(cFile.info.fileName, fileInfo->fileName);
        cFile.handle       = fOpen(fileInfo->fileName, "rb");
        cFile.info.virtualFileOffset = 0;
        cFile.info.fileSize          = fileInfo->fileSize;
        cFile.info.readPos           = fileInfo->readPos;
        fSeek(cFile.handle, cFile.info.readPos, SEEK_SET);
        FillFileBuffer();
        cFile.info.bufferPosition = fileInfo->bufferPosition;
        cFile.info.eStringPosA    = 0;
        cFile.info.eStringPosB    = 0;
        cFile.info.eStringNo      = 0;
        cFile.info.eNybbleSwap    = 0;
    }
}

size_t GetFilePosition()
{
    if (Engine.usingDataFile)
        return cFile.info.bufferPosition + cFile.info.readPos - cFile.readSize - cFile.info.virtualFileOffset;
    else
        return cFile.info.bufferPosition + cFile.info.readPos - cFile.readSize;
}

void SetFilePosition(int newPos)
{
    if (Engine.usingDataFile) {
        cFile.info.readPos     = cFile.info.virtualFileOffset + newPos;
        cFile.info.eStringNo   = (cFile.vFileSize & 0x1FCu) >> 2;
        cFile.info.eStringPosB = (cFile.info.eStringNo % 9) + 1;
        cFile.info.eStringPosA = (cFile.info.eStringNo % cFile.info.eStringPosB) + 1;
        cFile.info.eNybbleSwap = false;
        while (newPos) {
            ++cFile.info.eStringPosA;
            ++cFile.info.eStringPosB;
            if (cFile.info.eStringPosA <= 19 || cFile.info.eStringPosB <= 11) {
                if (cFile.info.eStringPosA > 19) {
                    cFile.info.eStringPosA = 1;
                    cFile.info.eNybbleSwap ^= 1u;
                }
                if (cFile.info.eStringPosB > 11) {
                    cFile.info.eStringPosB = 1;
                    cFile.info.eNybbleSwap ^= 1u;
                }
            }
            else {
                ++cFile.info.eStringNo;
                cFile.info.eStringNo &= 0x7Fu;
                if (cFile.info.eNybbleSwap) {
                    cFile.info.eNybbleSwap = false;
                    cFile.info.eStringPosA = (cFile.info.eStringNo % 12) + 6;
                    cFile.info.eStringPosB = (cFile.info.eStringNo % 5) + 4;
                }
                else {
                    cFile.info.eNybbleSwap = true;
                    cFile.info.eStringPosA = (cFile.info.eStringNo % 15) + 3;
                    cFile.info.eStringPosB = (cFile.info.eStringNo % 7) + 1;
                }
            }
            --newPos;
        }
    }
    else {
        cFile.info.readPos = newPos;
    }
    fSeek(cFile.handle, cFile.info.readPos, SEEK_SET);
    FillFileBuffer();
}

bool ReachedEndOfFile()
{
    if (Engine.usingDataFile)
        return cFile.info.bufferPosition + cFile.info.readPos - cFile.readSize - cFile.info.virtualFileOffset >= cFile.vFileSize;
    else
        return cFile.info.bufferPosition + cFile.info.readPos - cFile.readSize >= cFile.info.fileSize;
}

size_t FileRead2(FileInfo *info, void *dest, int size)
{
    byte *data = (byte *)dest;
    int rPos   = (int)GetFilePosition2(info);
    memset(data, 0, size);

    if (rPos <= info->fileSize) {
        if (Engine.usingDataFile) {
            int rSize = 0;
            if (rPos + size <= info->fileSize)
                rSize = size;
            else
                rSize = info->fileSize - rPos;

            size_t result = fRead(data, 1u, rSize, cFileStream.handle);
            info->readPos += rSize;
            info->bufferPosition = 0;

            while (size > 0) {
                *data = encryptionStringB[info->eStringPosB] ^ info->eStringNo ^ *data;
                if (info->eNybbleSwap)
                    *data = 16 * (*data & 0xF) + (*data >> 4);
                *data ^= encryptionStringA[info->eStringPosA++];
                ++info->eStringPosB;
                if (info->eStringPosA <= 19 || info->eStringPosB <= 11) {
                    if (info->eStringPosA > 19) {
                        info->eStringPosA = 1;
                        info->eNybbleSwap ^= 1u;
                    }
                    if (info->eStringPosB > 11) {
                        info->eStringPosB = 1;
                        info->eNybbleSwap ^= 1u;
                    }
                }
                else {
                    ++info->eStringNo;
                    info->eStringNo &= 0x7Fu;
                    if (info->eNybbleSwap) {
                        info->eNybbleSwap = false;
                        info->eStringPosA = (info->eStringNo % 12) + 6;
                        info->eStringPosB = (info->eStringNo % 5) + 4;
                    }
                    else {
                        info->eNybbleSwap = true;
                        info->eStringPosA = (info->eStringNo % 15) + 3;
                        info->eStringPosB = (info->eStringNo % 7) + 1;
                    }
                }
                ++data;
                --size;
            }
            return result;
        }
        else {
            int rSize = 0;
            if (rPos + size <= info->fileSize)
                rSize = size;
            else
                rSize = info->fileSize - rPos;

            size_t result = fRead(data, 1u, rSize, cFileStream.handle);
            info->readPos += rSize;
            info->bufferPosition = 0;
            return result;
        }
    }
    return 0;
}

size_t GetFilePosition2(FileInfo* info)
{
    if (Engine.usingDataFile)
        return info->bufferPosition + info->readPos - info->virtualFileOffset;
    else
        return info->bufferPosition + info->readPos;
}

void SetFilePosition2(FileInfo *info, int newPos)
{
    if (Engine.usingDataFile) {
        info->readPos     = info->virtualFileOffset + newPos;
        info->eStringNo   = (info->fileSize & 0x1FCu) >> 2;
        info->eStringPosB = (info->eStringNo % 9) + 1;
        info->eStringPosA = (info->eStringNo % info->eStringPosB) + 1;
        info->eNybbleSwap = false;
        while (newPos) {
            ++info->eStringPosA;
            ++info->eStringPosB;
            if (info->eStringPosA <= 19 || info->eStringPosB <= 11) {
                if (info->eStringPosA > 19) {
                    info->eStringPosA = 1;
                    info->eNybbleSwap ^= 1u;
                }
                if (info->eStringPosB > 11) {
                    info->eStringPosB = 1;
                    info->eNybbleSwap ^= 1u;
                }
            }
            else {
                ++info->eStringNo;
                info->eStringNo &= 0x7Fu;
                if (info->eNybbleSwap) {
                    info->eNybbleSwap = false;
                    info->eStringPosA = (info->eStringNo % 12) + 6;
                    info->eStringPosB = (info->eStringNo % 5) + 4;
                }
                else {
                    info->eNybbleSwap = true;
                    info->eStringPosA = (info->eStringNo % 15) + 3;
                    info->eStringPosB = (info->eStringNo % 7) + 1;
                }
            }
            --newPos;
        }
    }
    else {
        info->readPos = newPos;
    }
    fSeek(cFileStream.handle, info->readPos, SEEK_SET);
}
