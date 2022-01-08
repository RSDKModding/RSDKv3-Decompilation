#ifndef READER_H
#define READER_H

#ifdef FORCE_CASE_INSENSITIVE

#include "fcaseopen.h"
#define FileIO                                          FILE
#define fOpen(path, mode)                               fcaseopen(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  fread(buffer, elementSize, elementCount, file)
#define fSeek(file, offset, whence)                     fseek(file, offset, whence)
#define fTell(file)                                     ftell(file)
#define fClose(file)                                    fclose(file)
#define fWrite(buffer, elementSize, elementCount, file) fwrite(buffer, elementSize, elementCount, file)

#else

#if RETRO_USING_SDL2
#define FileIO                                          SDL_RWops
#define fOpen(path, mode)                               SDL_RWFromFile(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  SDL_RWread(file, buffer, elementSize, elementCount)
#define fSeek(file, offset, whence)                     SDL_RWseek(file, offset, whence)
#define fTell(file)                                     SDL_RWtell(file)
#define fClose(file)                                    SDL_RWclose(file)
#define fWrite(buffer, elementSize, elementCount, file) SDL_RWwrite(file, buffer, elementSize, elementCount)
#else
#define FileIO                                          FILE
#define fOpen(path, mode)                               fopen(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  fread(buffer, elementSize, elementCount, file)
#define fSeek(file, offset, whence)                     fseek(file, offset, whence)
#define fTell(file)                                     ftell(file)
#define fClose(file)                                    fclose(file)
#define fWrite(buffer, elementSize, elementCount, file) fwrite(buffer, elementSize, elementCount, file)
#endif

#endif

struct FileInfo {
    char fileName[0x100];
    int fileSize;
    int vFileSize;
    int readPos;
    int bufferPosition;
    int virtualFileOffset;
    byte eStringPosA;
    byte eStringPosB;
    byte eStringNo;
    byte eNybbleSwap;
    FileIO *cFileHandle;
    byte *fileBuffer;
#if RETRO_USE_MOD_LOADER
    byte isMod;
#endif
};

extern char rsdkName[0x400];

extern char fileName[0x100];
extern byte fileBuffer[0x2000];
extern int fileSize;
extern int vFileSize;
extern int readPos;
extern int readSize;
extern int bufferPosition;
extern int virtualFileOffset;
extern byte eStringPosA;
extern byte eStringPosB;
extern byte eStringNo;
extern byte eNybbleSwap;
extern char encryptionStringA[21];
extern char encryptionStringB[13];
#if RETRO_USE_MOD_LOADER
extern byte isModdedFile;
#endif

extern FileIO *cFileHandle;

inline void CopyFilePath(char *dest, const char *src)
{
    strcpy(dest, src);
    for (int i = 0;; ++i) {
        if (i >= strlen(dest)) {
            break;
        }

        if (dest[i] == '/')
            dest[i] = '\\';
    }
}
bool CheckRSDKFile(const char *filePath);

bool LoadFile(const char *filePath, FileInfo *fileInfo);
inline bool CloseFile()
{
    int result = 0;
    if (cFileHandle)
        result = fClose(cFileHandle);

    cFileHandle = NULL;
    return result;
}

void FileRead(void *dest, int size);

bool ParseVirtualFileSystem(FileInfo *fileInfo);

inline size_t FillFileBuffer()
{
    if (readPos + 0x2000 <= fileSize)
        readSize = 0x2000;
    else
        readSize = fileSize - readPos;

    size_t result = fRead(fileBuffer, 1, readSize, cFileHandle);
    readPos += readSize;
    bufferPosition = 0;
    return result;
}

inline void GetFileInfo(FileInfo *fileInfo)
{
    StrCopy(fileInfo->fileName, fileName);
    fileInfo->bufferPosition    = bufferPosition;
    fileInfo->readPos           = readPos - readSize;
    fileInfo->fileSize          = fileSize;
    fileInfo->vFileSize         = vFileSize;
    fileInfo->virtualFileOffset = virtualFileOffset;
    fileInfo->eStringPosA       = eStringPosA;
    fileInfo->eStringPosB       = eStringPosB;
    fileInfo->eStringNo         = eStringNo;
    fileInfo->eNybbleSwap       = eNybbleSwap;
#if RETRO_USE_MOD_LOADER
    fileInfo->isMod = isModdedFile;
#endif
}
void SetFileInfo(FileInfo *fileInfo);
size_t GetFilePosition();
void SetFilePosition(int newPos);
bool ReachedEndOfFile();

#endif // !READER_H
