#include "RetroEngine.h"
#include <string>

using namespace Reader;

void Reader::CopyFilePath(char* Dest, const char* Src) {

    strcpy(Dest, Src);
    for (int i = 0; ; i++) {
        if (i >= strlen(Dest)) {
            break;
        }

        if (Dest[i] == '/') {
            Dest[i] = '\\';
        }
    }
}

bool Reader::CheckRSDKFile(const char* filename) {
    char result;
    char rsdkName; 
    FileInfo FileInfo;

    Engine.usingDataFile = false;
    Engine.usingBytecode = false;
    result = 0;

    //CopyFilePath(filename, &rsdkName);
    cFileHandle = fopen(filename, "rb");
    if (cFileHandle) {
        Engine.usingDataFile = true;
        StringUtils::StrCopy(RSDKName, (char*)filename);
        fclose(cFileHandle);
        cFileHandle = 0;
        if (LoadFile("Data/Scripts/ByteCode/GlobalCode.bin", &FileInfo)) {
            Engine.usingBytecode = true;
            CloseFile();
        }
        result = 1;
    }

    return result;
}

int Reader::LoadFile(const char* Filename, FileInfo* FileInfo) {
    
    sprintf(FileInfo->FileName, "%s", Filename);

    if (cFileHandle) {
        fclose(cFileHandle);
    }
    cFileHandle = NULL;

    if (Engine.usingDataFile) {
        cFileHandle = fopen(RSDKName, "rb");
        fseek(cFileHandle, 0, 2);
        FileSize = ftell(cFileHandle);
        BufferPosition = 0;
        ReadSize = 0;
        ReadPos = 0;
        if (!ParseVirtualFileSystem(FileInfo)) {
            fclose(cFileHandle);
            cFileHandle = NULL;
#if RSDK_DEBUG
            printf("Couldn't load file '%s'\n", Filename);
#endif
            return 0;
        }
        FileInfo->FileSize = vFileSize;
        FileInfo->VirtualFileOffset = VirtualFileOffset;
    }
    else {
        cFileHandle = fopen(FileInfo->FileName, "rb");
        if (!cFileHandle) {
#if RSDK_DEBUG
            printf("Couldn't load file '%s'\n", Filename);
#endif
            return 0;
        }
        VirtualFileOffset = 0;
        fseek(cFileHandle, 0, SEEK_END);
        FileInfo->FileSize = ftell(cFileHandle);
        FileSize = ftell(cFileHandle);
        fseek(cFileHandle, 0, SEEK_SET);
        ReadPos = 0;
    }
    BufferPosition = 0;
    ReadSize = 0;

#if RSDK_DEBUG
    printf("Loaded File '%s'\n", Filename);
#endif

    return 1;
}

int Reader::ParseVirtualFileSystem(FileInfo* File) {
    char result;
    char Filename[71];
    byte DirCount;
    char StringBuffer[68];
    int j;
    int FileOffset;
    int FNamePos = 0;
    int headerSize;
    char FullFilename[68];
    int i;
    byte FileBuffer;

    j = 0;
    VirtualFileOffset = 0;
    for (int i = 0; File->FileName[i]; i++) {
        if (File->FileName[i] == '/') {
            FNamePos = i;
            j = 0;
        }
        else {
            ++j;
        }
        FullFilename[i] = File->FileName[i];
    }
    ++FNamePos;
    for (i = 0; i < j; ++i) {
        Filename[i] = File->FileName[i + FNamePos];
    }
    Filename[j] = 0;
    FullFilename[FNamePos] = 0;

    fseek(cFileHandle, 0, SEEK_SET);
    Engine.usingDataFile = 0;
    BufferPosition = 0;
    ReadSize = 0;
    ReadPos = 0;

    FileRead(&FileBuffer, 1);
    headerSize = FileBuffer;
    FileRead(&FileBuffer, 1);
    headerSize += FileBuffer << 8;
    FileRead(&FileBuffer, 1);
    headerSize += FileBuffer << 16;
    FileRead(&FileBuffer, 1);
    headerSize += FileBuffer << 24;

    FileRead(&FileBuffer, 1);
    DirCount = FileBuffer;
    FileRead(&FileBuffer, 1);
    DirCount += FileBuffer << 8;

    i = 0;
    FileOffset = 0;
    while (i < DirCount) {
        FileRead(&FileBuffer, 1);
        for (j = 0; j < FileBuffer; ++j) {
            FileRead(&StringBuffer[j], 1);
            StringBuffer[j] ^= -1 - FileBuffer;
        }
        StringBuffer[j] = 0;

        FileBuffer = StringUtils::StrComp(FullFilename, StringBuffer);
        if (FileBuffer == 1) {
            i = DirCount;
            FileRead(&FileBuffer, 1);
            FileOffset = FileBuffer;
            FileRead(&FileBuffer, 1);
            FileOffset += FileBuffer << 8;
            FileRead(&FileBuffer, 1);
            FileOffset += FileBuffer << 16;
            FileRead(&FileBuffer, 1);
            FileOffset += FileBuffer << 24;
        }
        else {
            FileOffset = -1;
            FileRead(&FileBuffer, 1);
            FileRead(&FileBuffer, 1);
            FileRead(&FileBuffer, 1);
            FileRead(&FileBuffer, 1);
            ++i;
        }
    }

    if (FileOffset == -1) {
        Engine.usingDataFile = 1;
        result = 0;
    }
    else {
        fseek(cFileHandle, FileOffset + headerSize, 0);
        BufferPosition = 0;
        ReadSize = 0;
        ReadPos = 0;
        VirtualFileOffset = FileOffset + headerSize;
        i = 0;
        while (i < 1) {
            FileRead(&FileBuffer, 1);
            ++VirtualFileOffset;
            j = 0;
            while (j < FileBuffer) {
                FileRead(&StringBuffer[j], 1);
                StringBuffer[j] = ~StringBuffer[j];
                ++j;
                ++VirtualFileOffset;
            }
            StringBuffer[j] = 0;

            if (StringUtils::StrComp(Filename, StringBuffer)) {
                i = 1;
                FileRead(&FileBuffer, 1);
                j = FileBuffer;
                FileRead(&FileBuffer, 1);
                j += FileBuffer << 8;
                FileRead(&FileBuffer, 1);
                j += FileBuffer << 16;
                FileRead(&FileBuffer, 1);
                j += FileBuffer << 24;
                VirtualFileOffset += 4;
                vFileSize = j;
            }
            else {
                FileRead(&FileBuffer, 1);
                j = FileBuffer;
                FileRead(&FileBuffer, 1);
                j += FileBuffer << 8;
                FileRead(&FileBuffer, 1);
                j += FileBuffer << 16;
                FileRead(&FileBuffer, 1);
                j += FileBuffer << 24;
                VirtualFileOffset += 4;
                VirtualFileOffset += j;
            }
            fseek(cFileHandle, VirtualFileOffset, 0);
            BufferPosition = 0;
            ReadSize = 0;
            ReadPos = VirtualFileOffset;
        }
        eStringNo = (vFileSize & 0x1FCu) >> 2;
        eStringPosB = (eStringNo % 9) + 1;
        eStringPosA = (eStringNo % eStringPosB) + 1;
        eNybbleSwap = 0;
        Engine.usingDataFile = true;
        result = 1;
    }
    return result;
}

void Reader::FileRead(void* Dest, int Size) {
    byte* data = (byte*)Dest;
    byte eStrPosB; // dl
    if (ReadPos <= FileSize) {
        if (Engine.usingDataFile) {
            while (Size > 0) {
                if (BufferPosition == ReadSize) {
                    FillFileBuffer();
                }

                *data = encryptionStringB[eStringPosB] ^ eStringNo ^ fileBuffer[BufferPosition];
                if (eNybbleSwap == 1) {
                    *data = 16 * (*data & 0xF) + ((signed int)*data >> 4);
                }
                *data ^= encryptionStringA[eStringPosA++];
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
                        eNybbleSwap = 0;
                        eStringPosA = (eStringNo % 12) + 6;
                        eStrPosB = (eStringNo % 5) + 4;
                    }
                    else {
                        eNybbleSwap = 1;
                        eStringPosA = (eStringNo % 15) + 3;
                        eStrPosB = (eStringNo % 7) + 1;
                    }
                    eStringPosB = eStrPosB;
                }
                ++data;
                --Size;
            }
        }
        else {
            while (Size > 0) {
                if (BufferPosition == ReadSize) {
                    FillFileBuffer();
                }

                *data++ = fileBuffer[BufferPosition++];
                Size--;
            }
        }
    }
}

size_t Reader::FillFileBuffer(void) {
    size_t result;

    if (ReadPos + 0x2000 <= FileSize) {
        ReadSize = 0x2000;
    }
    else {
        ReadSize = FileSize - ReadPos;
    }
    result = fread(fileBuffer, 1u, ReadSize, cFileHandle);
    ReadPos += ReadSize;
    BufferPosition = 0;
    return result;
}

void Reader::GetFileInfo(FileInfo* FileInfo) {
    FileInfo->BufferPosition = BufferPosition;
    FileInfo->ReadPos = ReadPos - ReadSize;
    FileInfo->eStringPosA = eStringPosA;
    FileInfo->eStringPosB = eStringPosB;
    FileInfo->eStringNo = eStringNo;
    FileInfo->eNybbleSwap = eNybbleSwap;
}

void Reader::SetFileInfo(FileInfo* FileInfo) {
    if (Engine.usingDataFile) {
        cFileHandle = fopen(RSDKName, "rb");
        VirtualFileOffset = FileInfo->VirtualFileOffset;
        vFileSize = FileInfo->FileSize;
        fseek(cFileHandle, 0, 2);
        FileSize = ftell(cFileHandle);
        ReadPos = FileInfo->ReadPos;
        fseek(cFileHandle, ReadPos, 0);
        FillFileBuffer();
        BufferPosition = FileInfo->BufferPosition;
        eStringPosA = FileInfo->eStringPosA;
        eStringPosB = FileInfo->eStringPosB;
        eStringNo = FileInfo->eStringNo;
        eNybbleSwap = FileInfo->eNybbleSwap;
    }
    else {
        cFileHandle = fopen(FileInfo->FileName, "rb");
        VirtualFileOffset = 0;
        FileSize = FileInfo->FileSize;
        ReadPos = FileInfo->ReadPos;
        fseek(cFileHandle, ReadPos, 0);
        FillFileBuffer();
        BufferPosition = FileInfo->BufferPosition;
    }
}

size_t Reader::GetFilePosition(void) {
    size_t result; // eax

    if (Engine.usingDataFile == 1) {
        result = BufferPosition + ReadPos - ReadSize - VirtualFileOffset;
    }
    else {
        result = BufferPosition + ReadPos - ReadSize;
    }
    return result;
}

void Reader::SetFilePosition(int NewPos) {
    byte eStrPosB; // dl

    if (Engine.usingDataFile) {
        ReadPos = VirtualFileOffset + NewPos;
        eStringNo = (vFileSize & 0x1FCu) >> 2;
        eStringPosB = (eStringNo % 9) + 1;
        eStringPosA = (eStringNo % eStringPosB) + 1;
        eNybbleSwap = 0;
        while (NewPos)
        {
            ++eStringPosA;
            ++eStringPosB;
            if (eStringPosA <= 19 || eStringPosB <= 11)
            {
                if (eStringPosA > 19)
                {
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
                    eNybbleSwap = 0;
                    eStringPosA = (eStringNo % 12) + 6;
                    eStrPosB = (eStringNo % 5) + 4;
                }
                else {
                    eNybbleSwap = 1;
                    eStringPosA = (eStringNo % 15) + 3;
                    eStrPosB = (eStringNo % 7) + 1;
                }
                eStringPosB = eStrPosB;
            }
            --NewPos;
        }
    }
    else {
        ReadPos = NewPos;
    }
    fseek(cFileHandle, ReadPos, 0);
    FillFileBuffer();
}

bool Reader::ReachedEndOfFile(void) {
    bool result;

    if (Engine.usingDataFile) {
        result = BufferPosition + ReadPos - ReadSize - VirtualFileOffset >= vFileSize;
    }
    else {
        result = BufferPosition + ReadPos - ReadSize >= FileSize;
    }
    return result;
}

int Reader::CloseFile(void) {
    int result;

    if (cFileHandle) {
        result = fclose(cFileHandle);
    }

    cFileHandle = NULL;
    return result;
}