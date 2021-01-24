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
    int vFileSize;
    int readSize;
};

extern char rsdkName[0x400];

extern File cFile;
extern File cFileStream;

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
    if (cFile.handle)
        result = fClose(cFile.handle);

    cFile.handle = NULL;
    return result;
}

void FileRead(void *dest, int size);

bool ParseVirtualFileSystem(FileInfo *fileInfo);

inline size_t FillFileBuffer()
{
    if (cFile.info.readPos + sizeof(cFile.fileBuffer) <= cFile.info.fileSize)
        cFile.readSize = sizeof(cFile.fileBuffer);
    else 
        cFile.readSize = cFile.info.fileSize - cFile.info.readPos;

    size_t result = fRead(cFile.fileBuffer, 1u, cFile.readSize, cFile.handle);
    cFile.info.readPos += cFile.readSize;
    cFile.info.bufferPosition = 0;
    return result;
}

inline void GetFileInfo(FileInfo *fileInfo)
{
    StrCopy(fileInfo->fileName, cFile.info.fileName);
    fileInfo->bufferPosition = cFile.info.bufferPosition;
    fileInfo->readPos        = cFile.info.readPos - cFile.readSize;
    fileInfo->fileSize       = cFile.info.fileSize;
    fileInfo->virtualFileOffset = cFile.info.virtualFileOffset;
    fileInfo->eStringPosA    = cFile.info.eStringPosA;
    fileInfo->eStringPosB    = cFile.info.eStringPosB;
    fileInfo->eStringNo      = cFile.info.eStringNo;
    fileInfo->eNybbleSwap    = cFile.info.eNybbleSwap;
}
void SetFileInfo(FileInfo *fileInfo);
size_t GetFilePosition();
void SetFilePosition(int newPos);
bool ReachedEndOfFile();


size_t FileRead2(FileInfo *info, void *dest, int size); // For Music Streaming
inline bool CloseFile2()
{
    int result = 0;
    if (cFileStream.handle)
        result = fClose(cFileStream.handle);

    cFileStream.handle = NULL;
    return result;
}
size_t GetFilePosition2(FileInfo *info);
void SetFilePosition2(FileInfo *info, int newPos);

#endif // !READER_H
