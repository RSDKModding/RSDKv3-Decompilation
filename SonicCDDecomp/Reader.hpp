#ifndef READER_H
#define READER_H

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

extern FILE *cFileHandle;
extern FILE *cFileHandleStream;

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
        result = fclose(cFileHandle);

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

    size_t result = fread(fileBuffer, 1u, readSize, cFileHandle);
    readPos += readSize;
    bufferPosition = 0;
    return result;
}

inline void GetFileInfo(FileInfo *fileInfo)
{
    StrCopy(fileInfo->fileName, fileName);
    fileInfo->bufferPosition = bufferPosition;
    fileInfo->readPos        = readPos - readSize;
    fileInfo->fileSize       = fileSize;
    fileInfo->virtualFileOffset = virtualFileOffset;
    fileInfo->eStringPosA    = eStringPosA;
    fileInfo->eStringPosB    = eStringPosB;
    fileInfo->eStringNo      = eStringNo;
    fileInfo->eNybbleSwap    = eNybbleSwap;
}
void SetFileInfo(FileInfo *fileInfo);
size_t GetFilePosition();
void SetFilePosition(int newPos);
bool ReachedEndOfFile();


size_t FileRead2(FileInfo *info, void *dest, int size); // For Music Streaming
inline bool CloseFile2()
{
    int result = 0;
    if (cFileHandleStream)
        result = fclose(cFileHandleStream);

    cFileHandleStream = NULL;
    return result;
}
size_t GetFilePosition2(FileInfo *info);
void SetFilePosition2(FileInfo *info, int newPos);

#endif // !READER_H
