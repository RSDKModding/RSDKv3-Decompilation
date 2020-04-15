#include "RetroEngine.h"

int Audio::InitAudioPlayback(void) {
    Reader::FileInfo FileInfo;
    Reader::FileInfo FileStore;
    char StrBuffer[0x40];
    int FileBuffer = 0;
    int FileBuffer2 = 0;

    if (Reader::LoadFile("Data/Game/Gameconfig.bin", &FileInfo)) {
        FileStore = FileInfo;

        Reader::FileRead(&FileBuffer, 1);
        Reader::FileRead(StrBuffer, FileBuffer);
        StrBuffer[FileBuffer] = 0;

        Reader::FileRead(&FileBuffer, 1);
        Reader::FileRead(&StrBuffer, FileBuffer);                      // Load 'Data'
        StrBuffer[FileBuffer] = 0;

        Reader::FileRead(&FileBuffer, 1);
        Reader::FileRead(StrBuffer, FileBuffer);
        StrBuffer[FileBuffer] = 0;

        //Read Obect Names
        int ObjectCount = 0;
        Reader::FileRead(&ObjectCount, 1);
        for (int j = 0; j < ObjectCount; ++j) {
            Reader::FileRead(&FileBuffer, 1);
            Reader::FileRead(StrBuffer, FileBuffer);
            StrBuffer[FileBuffer] = 0;
        }

        //Read Script Paths
        for (int k = 0; k < ObjectCount; ++k) {
            Reader::FileRead(&FileBuffer, 1);
            Reader::FileRead(StrBuffer, FileBuffer);
            StrBuffer[FileBuffer] = 0;
        }

        int GlobVarCnt = 0;
        Reader::FileRead(&GlobVarCnt, 1);
        for (int l = 0; l < GlobVarCnt; ++l) {
            Reader::FileRead(&FileBuffer, 1);
            Reader::FileRead(StrBuffer, FileBuffer);
            StrBuffer[FileBuffer] = 0;

            //Read Variable Value
            Reader::FileRead(&FileBuffer2, 4);
        }

        //Read SFX
        int SFXCnt = 0;
        numGlobalSFX = 0;
        Reader::FileRead(&SFXCnt, 1);
        for (int m = 0; m < SFXCnt; ++m) {
            numGlobalSFX++;
            Reader::FileRead(&FileBuffer, 1);
            Reader::FileRead(StrBuffer, FileBuffer);
            StrBuffer[FileBuffer] = 0;

            Reader::GetFileInfo(&FileStore);
            //LoadSFX(StrBuffer,m); //FilePath,SFXID
            Reader::SetFileInfo(&FileStore);
        }

        Reader::CloseFile();
    }

	return 1;
}