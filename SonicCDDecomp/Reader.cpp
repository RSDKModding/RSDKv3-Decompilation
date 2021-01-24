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

bool LoadFile(const char *filePath, FileInfo *fileInfo, File *file)
{
    MEM_ZEROP(fileInfo);
    StrCopy(fileInfo->fileName, filePath);
    StrCopy(file->info.fileName, filePath);

    if (file->handle)
        fClose(file->handle);

    file->handle = NULL;

    if (Engine.usingDataFile) {
        file->handle = fOpen(rsdkName, "rb");
        fSeek(file->handle, 0, SEEK_END);
        file->info.fileSize       = (int)fTell(file->handle);
        file->info.bufferPosition = 0;
        file->readSize       = 0;
        file->info.readPos        = 0;
        if (!ParseVirtualFileSystem(fileInfo)) {
            fClose(file->handle);
            file->handle = NULL;
            printLog("Couldn't load file '%s'", filePath);
            return false;
        }
        fileInfo->readPos           = file->info.readPos;
        fileInfo->fileSize          = file->vFileSize;
        fileInfo->virtualFileOffset = file->info.virtualFileOffset;
        fileInfo->eStringNo         = file->info.eStringNo;
        fileInfo->eStringPosB       = file->info.eStringPosB;
        fileInfo->eStringPosA       = file->info.eStringPosA;
        fileInfo->eNybbleSwap       = file->info.eNybbleSwap;
        fileInfo->bufferPosition    = file->info.bufferPosition;
    }
    else {
        file->handle = fOpen(fileInfo->fileName, "rb");
        if (!file->handle) {
            printLog("Couldn't load file '%s'", filePath);
            return false;
        }
        file->info.virtualFileOffset = 0;
        fSeek(file->handle, 0, SEEK_END);
        fileInfo->fileSize = (int)fTell(file->handle);
        file->info.fileSize           = fileInfo->fileSize;
        fSeek(file->handle, 0, SEEK_SET);
        file->info.readPos = 0;
        fileInfo->readPos           = file->info.readPos;
        fileInfo->virtualFileOffset = 0;
        fileInfo->eStringNo         = 0;
        fileInfo->eStringPosB       = 0;
        fileInfo->eStringPosA       = 0;
        fileInfo->eNybbleSwap       = 0;
        fileInfo->bufferPosition    = 0;
    }
    file->info.bufferPosition = 0;
    file->readSize       = 0;

    printLog("Loaded File '%s'", filePath);

    return true;
}

bool ParseVirtualFileSystem(FileInfo *fileInfo, File *file)
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
    file->info.virtualFileOffset = 0;
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

    fSeek(file->handle, 0, SEEK_SET);
    Engine.usingDataFile = false;
    file->info.bufferPosition       = 0;
    file->readSize             = 0;
    file->info.readPos              = 0;

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
                nextFileOffset = file->info.fileSize - headerSize; //There is no next dir, so just make this the EOF
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
        fSeek(file->handle, fileOffset + headerSize, SEEK_SET);
        file->info.bufferPosition    = 0;
        file->readSize          = 0;
        file->info.readPos           = 0;
        file->info.virtualFileOffset = fileOffset + headerSize;
        i                 = 0;
        while (i < 1) {
            FileRead(&fileBuffer, 1);
            ++file->info.virtualFileOffset;
            j = 0;
            while (j < fileBuffer) {
                FileRead(&stringBuffer[j], 1);
                stringBuffer[j] = ~stringBuffer[j];
                ++j;
                ++file->info.virtualFileOffset;
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
                file->info.virtualFileOffset += 4;
                file->vFileSize = j;
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
                file->info.virtualFileOffset += 4;
                file->info.virtualFileOffset += j;
            }

            //No File has been found (next file would be in a new dir)
            if (file->info.virtualFileOffset >= nextFileOffset + headerSize) {
                Engine.usingDataFile = true;
                return false;
            }
            fSeek(file->handle, file->info.virtualFileOffset, SEEK_SET);
            file->info.bufferPosition = 0;
            file->readSize       = 0;
            file->info.readPos        = file->info.virtualFileOffset;
        }
        file->info.eStringNo            = (file->vFileSize & 0x1FCu) >> 2;
        file->info.eStringPosB          = (file->info.eStringNo % 9) + 1;
        file->info.eStringPosA          = (file->info.eStringNo % file->info.eStringPosB) + 1;
        file->info.eNybbleSwap          = false;
        Engine.usingDataFile = true;
        return true;
    }
    //Engine.usingDataFile = true;
    return false;
}

void FileRead(void *dest, int size, File *file)
{
    byte *data = (byte *)dest;

    if (file->info.readPos <= file->info.fileSize) {
        if (Engine.usingDataFile) {
            while (size > 0) {
                if (file->info.bufferPosition == file->readSize)
                    FillFileBuffer();

                *data = encryptionStringB[file->info.eStringPosB] ^ file->info.eStringNo ^ file->fileBuffer[file->info.bufferPosition++];
                if (file->info.eNybbleSwap)
                    *data = 16 * (*data & 0xF) + ((signed int)*data >> 4);
                *data ^= encryptionStringA[file->info.eStringPosA++];
                ++file->info.eStringPosB;
                if (file->info.eStringPosA <= 19 || file->info.eStringPosB <= 11) {
                    if (file->info.eStringPosA > 19) {
                        file->info.eStringPosA = 1;
                        file->info.eNybbleSwap ^= 1u;
                    }
                    if (file->info.eStringPosB > 11) {
                        file->info.eStringPosB = 1;
                        file->info.eNybbleSwap ^= 1u;
                    }
                }
                else {
                    ++file->info.eStringNo;
                    file->info.eStringNo &= 0x7Fu;
                    if (file->info.eNybbleSwap) {
                        file->info.eNybbleSwap = 0;
                        file->info.eStringPosA = (file->info.eStringNo % 12) + 6;
                        file->info.eStringPosB = (file->info.eStringNo % 5) + 4;
                    }
                    else {
                        file->info.eNybbleSwap = 1;
                        file->info.eStringPosA = (file->info.eStringNo % 15) + 3;
                        file->info.eStringPosB = (file->info.eStringNo % 7) + 1;
                    }
                }
                ++data;
                --size;
            }
        }
        else {
            while (size > 0) {
                if (file->info.bufferPosition == file->readSize)
                    FillFileBuffer();

                *data++ = file->fileBuffer[file->info.bufferPosition++];
                size--;
            }
        }
    }
}

void SetFileInfo(FileInfo *fileInfo, File *file)
{
    if (Engine.usingDataFile) {
        file->handle       = fOpen(rsdkName, "rb");
        file->info.virtualFileOffset = fileInfo->virtualFileOffset;
        file->vFileSize         = fileInfo->fileSize;
        fSeek(file->handle, 0, SEEK_END);
        file->info.fileSize = (int)fTell(file->handle);
        file->info.readPos  = fileInfo->readPos;
        fSeek(file->handle, file->info.readPos, SEEK_SET);
        FillFileBuffer();
        file->info.bufferPosition = fileInfo->bufferPosition;
        file->info.eStringPosA    = fileInfo->eStringPosA;
        file->info.eStringPosB    = fileInfo->eStringPosB;
        file->info.eStringNo      = fileInfo->eStringNo;
        file->info.eNybbleSwap    = fileInfo->eNybbleSwap;
    }
    else {
        StrCopy(file->info.fileName, fileInfo->fileName);
        file->handle       = fOpen(fileInfo->fileName, "rb");
        file->info.virtualFileOffset = 0;
        file->info.fileSize          = fileInfo->fileSize;
        file->info.readPos           = fileInfo->readPos;
        fSeek(file->handle, file->info.readPos, SEEK_SET);
        FillFileBuffer();
        file->info.bufferPosition = fileInfo->bufferPosition;
        file->info.eStringPosA    = 0;
        file->info.eStringPosB    = 0;
        file->info.eStringNo      = 0;
        file->info.eNybbleSwap    = 0;
    }
}

size_t GetFilePosition(File *file)
{
    if (Engine.usingDataFile)
        return file->info.bufferPosition + file->info.readPos - file->readSize - file->info.virtualFileOffset;
    else
        return file->info.bufferPosition + file->info.readPos - file->readSize;
}

void SetFilePosition(int newPos, File *file)
{
    if (Engine.usingDataFile) {
        file->info.readPos     = file->info.virtualFileOffset + newPos;
        file->info.eStringNo   = (file->vFileSize & 0x1FCu) >> 2;
        file->info.eStringPosB = (file->info.eStringNo % 9) + 1;
        file->info.eStringPosA = (file->info.eStringNo % file->info.eStringPosB) + 1;
        file->info.eNybbleSwap = false;
        while (newPos) {
            ++file->info.eStringPosA;
            ++file->info.eStringPosB;
            if (file->info.eStringPosA <= 19 || file->info.eStringPosB <= 11) {
                if (file->info.eStringPosA > 19) {
                    file->info.eStringPosA = 1;
                    file->info.eNybbleSwap ^= 1u;
                }
                if (file->info.eStringPosB > 11) {
                    file->info.eStringPosB = 1;
                    file->info.eNybbleSwap ^= 1u;
                }
            }
            else {
                ++file->info.eStringNo;
                file->info.eStringNo &= 0x7Fu;
                if (file->info.eNybbleSwap) {
                    file->info.eNybbleSwap = false;
                    file->info.eStringPosA = (file->info.eStringNo % 12) + 6;
                    file->info.eStringPosB = (file->info.eStringNo % 5) + 4;
                }
                else {
                    file->info.eNybbleSwap = true;
                    file->info.eStringPosA = (file->info.eStringNo % 15) + 3;
                    file->info.eStringPosB = (file->info.eStringNo % 7) + 1;
                }
            }
            --newPos;
        }
    }
    else {
        file->info.readPos = newPos;
    }
    fSeek(file->handle, file->info.readPos, SEEK_SET);
    FillFileBuffer();
}

bool ReachedEndOfFile(File *file)
{
    if (Engine.usingDataFile)
        return file->info.bufferPosition + file->info.readPos - file->readSize - file->info.virtualFileOffset >= file->vFileSize;
    else
        return file->info.bufferPosition + file->info.readPos - file->readSize >= file->info.fileSize;
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
