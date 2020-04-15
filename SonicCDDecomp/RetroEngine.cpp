#include "RetroEngine.h"


void RetroEngine::Init() {

	Math::CalculateTrigAngles();
	Drawing::GenerateBlendLookupTable();

	Reader::CheckRSDKFile("data.rsdk");

	LoadGameConfig("Data/Game/GameConfig.bin");
    if (Drawing::InitRenderDevice()) {
        if (Audio::InitAudioPlayback()) {
            Scene::InitFirstStage();
            Script::ClearScriptData();
            Initialised = true;
        }
    }

    Running = true;
}

void RetroEngine::Run() {

    printf("Running Game\n");
    while (Running) {

        //printf("GameMode: %d\n", GameMode);

        switch (GameMode)
        {
        case 0:
            //ProcessStageMenu();
            break;
        case 1:
            //Scene::ProcessStage();
            break;
        case 2u:
            LoadGameConfig("Data/Game/GameConfig.bin");
            //InitDevMenu();
            //Scene::ResetStageFolder();
            break;
        case 3:
            Running = false;
            break;
        case 4:
            LoadGameConfig("Data/Game/GameConfig.bin");
            //InitErrorMessage();
            //Scene::ResetStageFolder();
            break;
        case 5:
            GameMode = 1;
            break;
        case 6:
            GameMode = 1;
            break;
        default:
            break;
        }

    }

}

void RetroEngine::LoadGameConfig(const char* Filepath) {
    Reader::FileInfo FileInfo;
    int FileBuffer = 0;
    int FileBuffer2 = 0;
    char Data[0x40];
    char GlobalVarName[32];

    if (Reader::LoadFile(Filepath, &FileInfo)) {
        Reader::FileRead(&FileBuffer, 1);
        Reader::FileRead(GameWindowText, FileBuffer);
        GameWindowText[FileBuffer] = 0;

        Reader::FileRead(&FileBuffer, 1);
        Reader::FileRead(&Data, FileBuffer);                      // Load 'Data'
        Data[FileBuffer] = 0;

        Reader::FileRead(&FileBuffer, 1);
        Reader::FileRead(GameDescriptionText, FileBuffer);
        GameDescriptionText[FileBuffer] = 0;

        //Read Obect Names
        int ObjectCount = 0;
        Reader::FileRead(&ObjectCount, 1);
        for (int j = 0; j < ObjectCount; ++j) {
            Reader::FileRead(&FileBuffer, 1);
            for (int i = 0; i < FileBuffer; ++i) {
                Reader::FileRead(&FileBuffer2, 1);
            }
        }

        //Read Script Paths
        for (int k = 0; k < ObjectCount; ++k) {
            Reader::FileRead(&FileBuffer, 1);
            for (int i = 0; i < FileBuffer; ++i)
                Reader::FileRead(&FileBuffer2, 1);
        }

        int GlobVarCnt = 0;
        Reader::FileRead(&GlobVarCnt, 1);
        GlobalVariablesCount = 0;
        for (int l = 0; l < GlobVarCnt; ++l) {
            ++GlobalVariablesCount;
            //Read Variable Name
            int i = 0;
            Reader::FileRead(&FileBuffer, 1);
            for (; i < FileBuffer; ++i) {
                Reader::FileRead(&FileBuffer2, 1);
                GlobalVarName[i] = FileBuffer2;
            }
            GlobalVarName[i] = 0;
            StringUtils::StrCopy(GlobalVariableNames[l], GlobalVarName);

            //Read Variable Value
            Reader::FileRead(&FileBuffer2, 1);
            GlobalVariables[l] = FileBuffer2 << 24;
            Reader::FileRead(&FileBuffer2, 1);
            GlobalVariables[l] += FileBuffer2 << 16;
            Reader::FileRead(&FileBuffer2, 1);
            GlobalVariables[l] += FileBuffer2 << 8;
            Reader::FileRead(&FileBuffer2, 1);
            GlobalVariables[l] += FileBuffer2;
        }

        //Read SFX
        int SFXCnt = 0;
        Reader::FileRead(&SFXCnt, 1);
        for (int m = 0; m < SFXCnt; ++m) {
            int i = 0;
            Reader::FileRead(&FileBuffer, 1);
            for (; i < FileBuffer; ++i) {
                Reader::FileRead(&FileBuffer2, 1);
            }
        }

        //Read Player Names
        int PlayerCount = 0;
        Reader::FileRead(&PlayerCount, 1);
        for (int n = 0; n < PlayerCount; ++n) {
            Reader::FileRead(&FileBuffer, 1);
            for (int i = 0; i < FileBuffer; ++i) {
                Reader::FileRead(&FileBuffer2, 1);
            }
        }

        for (int i = 0; i < 4; i++) {
            Scene::StageListCount[i] = 0;
            Reader::FileRead(&Scene::StageListCount[i], 1);
            for (int ii = 0; ii < Scene::StageListCount[i]; ii++) {
                int c = 0;

                //Read Stage Folder
                Reader::FileRead(&FileBuffer, 1);
                for (c = 0; c < FileBuffer; c++) {
                    Reader::FileRead(&FileBuffer2, 1);
                    Scene::StageList[i][ii].Folder[c] = FileBuffer2;
                }
                Scene::StageList[i][ii].Folder[c] = 0;

                //Read Stage ID
                Reader::FileRead(&FileBuffer, 1);
                for (c = 0; c < FileBuffer; c++) {
                    Reader::FileRead(&FileBuffer2, 1);
                    Scene::StageList[i][ii].ID[c] = FileBuffer2;
                }
                Scene::StageList[i][ii].ID[c] = 0;

                //Read Stage Name
                Reader::FileRead(&FileBuffer, 1);
                for (c = 0; c < FileBuffer; c++) {
                    Reader::FileRead(&FileBuffer2, 1);
                    Scene::StageList[i][ii].Name[c] = FileBuffer2;
                }
                Scene::StageList[i][ii].Name[c] = 0;

                //Read Stage Mode
                Reader::FileRead(&FileBuffer, 1);
                Scene::StageList[i][ii].Mode = FileBuffer;
            }
        }

        Reader::CloseFile();
    }
}