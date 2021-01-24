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
#define FileIO                                            FILE
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
    int readPos;
    int bufferPosition;
    int virtualFileOffset;
    byte eStringPosA;
    byte eStringPosB;
    byte eStringNo;
    byte eNybbleSwap;
};

struct File {
    FileIO *handle;
    FileInfo info;
    byte fileBuffer[0x2000];
    int actualFileSize;
    int readSize;
};

extern char rsdkName[0x400];

extern File cFile;

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

bool LoadFile(const char *filePath, FileInfo *fileInfo, File *file = &cFile);
inline bool CloseFile(File *file = &cFile)
{
    int result = 0;
    if (file->handle)
        result = fClose(file->handle);

    file->handle = NULL;
    return result;
}

size_t FileRead(void *dest, int size, File *file = &cFile);

bool ParseVirtualFileSystem(FileInfo *fileInfo, File *file = &cFile);

inline size_t FillFileBuffer(File *file = &cFile)
{
    if (file->info.readPos + sizeof(file->fileBuffer) <= file->actualFileSize)
        file->readSize = sizeof(file->fileBuffer);
    else 
        file->readSize = file->actualFileSize - file->info.readPos;

    size_t result = fRead(file->fileBuffer, 1u, file->readSize, file->handle);
    file->info.readPos += file->readSize;
    file->info.bufferPosition = 0;
    return result;
}

inline void GetFileInfo(FileInfo *fileInfo, File *file = &cFile)
{
    StrCopy(fileInfo->fileName, file->info.fileName);
    fileInfo->bufferPosition = file->info.bufferPosition;
    fileInfo->readPos        = file->info.readPos - file->readSize;
    fileInfo->fileSize       = file->actualFileSize;
    fileInfo->virtualFileOffset = file->info.virtualFileOffset;
    fileInfo->eStringPosA    = file->info.eStringPosA;
    fileInfo->eStringPosB    = file->info.eStringPosB;
    fileInfo->eStringNo      = file->info.eStringNo;
    fileInfo->eNybbleSwap    = file->info.eNybbleSwap;
}
void SetFileInfo(FileInfo *fileInfo, File *file = &cFile);
size_t GetFilePosition(File *file = &cFile);
void SetFilePosition(int newPos, File *file = &cFile);
bool ReachedEndOfFile(File *file = &cFile);

#endif // !READER_H
