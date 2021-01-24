#include "RetroEngine.hpp"
#include <string>

char rsdkName[0x400];

static const char encryptionStringA[] = { "4RaS9D7KaEbxcp2o5r6t" };
static const char encryptionStringB[] = { "3tRaUxLmEaSn" };

File cFile;

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
        file->actualFileSize       = (int)fTell(file->handle);
        file->info.bufferPosition = 0;
        file->readSize       = 0;
        file->info.readPos        = 0;
        if (!ParseVirtualFileSystem(fileInfo->fileName, file)) {
            fClose(file->handle);
            file->handle = NULL;
            printLog("Couldn't load file '%s'", filePath);
            return false;
        }
        fileInfo->readPos           = file->info.readPos;
        fileInfo->fileSize          = file->info.fileSize;
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
        file->actualFileSize           = fileInfo->fileSize;
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

bool ParseVirtualFileSystem(const char *filePath, File *file)
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
    for (int i = 0; filePath[i]; i++) {
        if (filePath[i] == '/') {
            fNamePos = i;
            j        = 0;
        }
        else {
            ++j;
        }
        fullFilename[i] = filePath[i];
    }
    ++fNamePos;
    for (i = 0; i < j; ++i) filename[i] = filePath[i + fNamePos];
    filename[j]            = 0;
    fullFilename[fNamePos] = 0;

    fSeek(file->handle, 0, SEEK_SET);
    Engine.usingDataFile = false;
    file->info.bufferPosition       = 0;
    file->readSize             = 0;
    file->info.readPos              = 0;

    FileRead(&fileBuffer, 1, file);
    headerSize = fileBuffer;
    FileRead(&fileBuffer, 1, file);
    headerSize += fileBuffer << 8;
    FileRead(&fileBuffer, 1, file);
    headerSize += fileBuffer << 16;
    FileRead(&fileBuffer, 1, file);
    headerSize += fileBuffer << 24;

    FileRead(&fileBuffer, 1, file);
    dirCount = fileBuffer;
    FileRead(&fileBuffer, 1, file);
    dirCount += fileBuffer << 8;

    i          = 0;
    fileOffset = 0;
    int nextFileOffset = 0;
    while (i < dirCount) {
        FileRead(&fileBuffer, 1, file);
        for (j = 0; j < fileBuffer; ++j) {
            FileRead(&stringBuffer[j], 1, file);
            stringBuffer[j] ^= -1 - fileBuffer;
        }
        stringBuffer[j] = 0;

        if (StrComp(fullFilename, stringBuffer)) {
            FileRead(&fileBuffer, 1, file);
            fileOffset = fileBuffer;
            FileRead(&fileBuffer, 1, file);
            fileOffset += fileBuffer << 8;
            FileRead(&fileBuffer, 1, file);
            fileOffset += fileBuffer << 16;
            FileRead(&fileBuffer, 1, file);
            fileOffset += fileBuffer << 24;

            //Grab info for next dir to know when we've found an error
            //Ignore dir name we dont care
            if (i == dirCount - 1) {
                nextFileOffset = file->actualFileSize - headerSize; //There is no next dir, so just make this the EOF
            }
            else {
                FileRead(&fileBuffer, 1, file);
                for (j = 0; j < fileBuffer; ++j) {
                    FileRead(&stringBuffer[j], 1, file);
                    stringBuffer[j] ^= -1 - fileBuffer;
                }
                stringBuffer[j] = 0;

                FileRead(&fileBuffer, 1, file);
                nextFileOffset = fileBuffer;
                FileRead(&fileBuffer, 1, file);
                nextFileOffset += fileBuffer << 8;
                FileRead(&fileBuffer, 1, file);
                nextFileOffset += fileBuffer << 16;
                FileRead(&fileBuffer, 1, file);
                nextFileOffset += fileBuffer << 24;
            }

            i = dirCount;
        }
        else {
            fileOffset = -1;
            FileRead(&fileBuffer, 1, file);
            FileRead(&fileBuffer, 1, file);
            FileRead(&fileBuffer, 1, file);
            FileRead(&fileBuffer, 1, file);
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
            FileRead(&fileBuffer, 1, file);
            ++file->info.virtualFileOffset;
            j = 0;
            while (j < fileBuffer) {
                FileRead(&stringBuffer[j], 1, file);
                stringBuffer[j] = ~stringBuffer[j];
                ++j;
                ++file->info.virtualFileOffset;
            }
            stringBuffer[j] = 0;

            if (StrComp(filename, stringBuffer)) {
                i = 1;
                FileRead(&fileBuffer, 1, file);
                j = fileBuffer;
                FileRead(&fileBuffer, 1, file);
                j += fileBuffer << 8;
                FileRead(&fileBuffer, 1, file);
                j += fileBuffer << 16;
                FileRead(&fileBuffer, 1, file);
                j += fileBuffer << 24;
                file->info.virtualFileOffset += 4;
                file->info.fileSize = j;
            }
            else {
                FileRead(&fileBuffer, 1, file);
                j = fileBuffer;
                FileRead(&fileBuffer, 1, file);
                j += fileBuffer << 8;
                FileRead(&fileBuffer, 1, file);
                j += fileBuffer << 16;
                FileRead(&fileBuffer, 1, file);
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
        file->info.eStringNo            = (file->info.fileSize & 0x1FCu) >> 2;
        file->info.eStringPosB          = (file->info.eStringNo % 9) + 1;
        file->info.eStringPosA          = (file->info.eStringNo % file->info.eStringPosB) + 1;
        file->info.eNybbleSwap          = false;
        Engine.usingDataFile = true;
        return true;
    }
    //Engine.usingDataFile = true;
    return false;
}

size_t FileRead(void *dest, int size, File *file)
{
    size_t bytes_read = 0;
    byte *data = (byte *)dest;

    if (file->info.readPos <= file->actualFileSize) {
        if (Engine.usingDataFile) {
            while (bytes_read != size) {
                if (ReachedEndOfFile(file))
                    break;

                if (file->info.bufferPosition == file->readSize)
                    FillFileBuffer(file);

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
                ++bytes_read;
            }
        }
        else {
            while (bytes_read != size) {
                if (ReachedEndOfFile(file))
                    break;

                if (file->info.bufferPosition == file->readSize)
                    FillFileBuffer(file);

                *data++ = file->fileBuffer[file->info.bufferPosition++];
                ++bytes_read;
            }
        }
    }

    return bytes_read;
}

void SetFileInfo(FileInfo *fileInfo, File *file)
{
    if (Engine.usingDataFile) {
        file->handle       = fOpen(rsdkName, "rb");
        file->info.virtualFileOffset = fileInfo->virtualFileOffset;
        file->info.fileSize         = fileInfo->fileSize;
        fSeek(file->handle, 0, SEEK_END);
        file->actualFileSize = (int)fTell(file->handle);
        file->info.readPos  = fileInfo->readPos;
        fSeek(file->handle, file->info.readPos, SEEK_SET);
        FillFileBuffer(file);
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
        file->actualFileSize          = fileInfo->fileSize;
        file->info.readPos           = fileInfo->readPos;
        fSeek(file->handle, file->info.readPos, SEEK_SET);
        FillFileBuffer(file);
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
        file->info.eStringNo   = (file->info.fileSize & 0x1FCu) >> 2;
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
    FillFileBuffer(file);
}

bool ReachedEndOfFile(File *file)
{
    if (Engine.usingDataFile)
        return file->info.bufferPosition + file->info.readPos - file->readSize - file->info.virtualFileOffset >= file->info.fileSize;
    else
        return file->info.bufferPosition + file->info.readPos - file->readSize >= file->actualFileSize;
}
