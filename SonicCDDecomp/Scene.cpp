#include "RetroEngine.h"

void Scene::InitFirstStage(void) {
	xScrollOffset = 0;
	yScrollOffset = 0;
	//StopMusic();
	//StopAllSFX();
	//ReleaseStageSFX();
	Palette::FadeMode = 0;
	Player::PlayerListPos = 0;
	//ClearGraphicsData();
	//ClearAnimationData();
	Palette::ActivePalette = Palette::FullPalette[0];
	Palette::LoadPalette("MasterPalette.act", 0, 0, 0, 256);
	ActiveStageList = 0;
	StageMode = 0;
	Engine.GameMode = 1;
	StageListPosition = 0;
}

void Scene::ProcessStage(void) {

	switch (StageMode)
	{
	case 0: //Startup
		Palette::FadeMode = 0;
		Palette::SetActivePalette(0, 0, 256);

		CameraEnabled = 1;
		CameraTarget = -1;
		CameraAdjustY = 0;
		xScrollOffset = 0;
		yScrollOffset = 0;
		yScrollA = 0;
		yScrollB = 240;
		xScrollA = 0;
		xScrollB = 320;
		yScrollMove = 0;
		CameraShakeX = 0;
		CameraShakeY = 0;

		Drawing3D::VertexCount = 0;
		Drawing3D::FaceCount = 0;
		for (int i = 0; i < PLAYER_COUNT; i++) {
			MEM_ZERO(Player::Players[i]);
			Player::Players[i].Visible = 1;
			Player::Players[i].Gravity = 1; //Air
			Player::Players[i].TileCollisions = true;
			Player::Players[i].ObjectInteractions = true; 
		}
		PauseEnabled = false;
		TimeEnabled = false;
		StageMilliseconds = 0;
		StageSeconds = 0;
		StageMinutes = 0;
		ResetBackgroundSettings();
		LoadStageFiles();
		StageMode = 1;

		break;
	case 1: //Regular
		if (Palette::FadeMode > 0) {
			Palette::FadeMode--;
		}

		if (Palette::PaletteMode > 0) {
			Palette::PaletteMode = 0;
			Palette::SetActivePalette(0, 0, 256);
		}

		lastXSize = -1;
		lastYSize = -1;
		Input::CheckKeyDown(&Input::KeyDown, 0xFF);
		Input::CheckKeyPress(&Input::KeyPress, 0xFF);
		if (PauseEnabled && Input::KeyPress.Start) {
			StageMode = 2;
			Audio::PauseSound();
		}

		if (TimeEnabled) {
			if (++StageTimer == 60) {
				StageTimer = 0;
				if (++StageSeconds > 59) {
					StageSeconds = 0;
					if (++StageMinutes > 59)
						StageMinutes = 0;
				}
			}
			StageMilliseconds = 100 * StageTimer / 60;
		}

		Object::ProcessObjects();
		if (CameraTarget > -1) {
			if (CameraEnabled) {
				switch (CameraStyle)
				{
				case 0:
					Player::SetPlayerScreenPosition(&Player::Players[CameraTarget]);
					break;
				case 1:
					Player::SetPlayerScreenPositionCDStyle(&Player::Players[CameraTarget]);
					break;
				case 2:
					Player::SetPlayerScreenPositionCDStyle(&Player::Players[CameraTarget]);
					break;
				case 3:
					Player::SetPlayerScreenPositionCDStyle(&Player::Players[CameraTarget]);
					break;
				case 4:
					Player::SetPlayerHLockedScreenPosition(&Player::Players[CameraTarget]);
					break;
				default:
					break;
				}
			}
			else {
				Player::SetPlayerLockedScreenPosition(&Player::Players[CameraTarget]);
			}
		}

		Drawing::DrawStageGFX();
		break;
	case 2: //Paused
		if (Palette::FadeMode > 0) {
			Palette::FadeMode--;
		}

		if (Palette::PaletteMode > 0) {
			Palette::PaletteMode = 0;
			Palette::SetActivePalette(0, 0, 256);
		}
		lastXSize = -1;
		lastYSize = -1;
		Input::CheckKeyDown(&Input::KeyDown, 0xFF);
		Input::CheckKeyPress(&Input::KeyPress, 0xFF);
		Object::ProcessPausedObjects();
		Drawing::DrawObjectList(0);
		Drawing::DrawObjectList(1);
		Drawing::DrawObjectList(2);
		Drawing::DrawObjectList(3);
		Drawing::DrawObjectList(4);
		Drawing::DrawObjectList(5);
		Drawing::DrawObjectList(6);
		if (PauseEnabled && Input::KeyPress.Start) {
			StageMode = 1;
			Audio::ResumeSound();
		}
		break;
	}

}

void Scene::LoadStageFiles(void) {

}

int Scene::LoadActFile(const char* Extention, int StageID, Reader::FileInfo* FileInfo) {
	char dest[0x40];

	StringUtils::StrCopy(dest, (char*)"Data/Stages/");
	StringUtils::StrAdd(dest, StageList[ActiveStageList][StageID].Folder);
	StringUtils::StrAdd(dest, (char*)"/Act");
	StringUtils::StrAdd(dest, StageList[ActiveStageList][StageID].ID);
	StringUtils::StrAdd(dest, (char*)Extention);
	return LoadFile(dest, FileInfo);
}

int Scene::LoadStageFile(const char* FileName, int StageID, Reader::FileInfo* FileInfo) {
	char dest[0x40];

	StringUtils::StrCopy(dest, (char*)"Data/Stages/");
	StringUtils::StrAdd(dest, StageList[ActiveStageList][StageID].Folder);
	StringUtils::StrAdd(dest, (char*)"/");
	StringUtils::StrAdd(dest, (char*)FileName);
	return LoadFile(dest, FileInfo);
}

void Scene::LoadActLayout(void) {

}

void Scene::LoadStageBackground(void) {

}

void Scene::LoadStageChunks(void) {
	Reader::FileInfo FileInfo;
	byte MappingEntry[3];

	if (LoadStageFile("128x128Tiles.bin", StageListPosition, &FileInfo)) {
		for (int i = 0; i < 0x8000; i++) {
			Reader::FileRead(&MappingEntry, 3);
			MappingEntry[0] -= (byte)((MappingEntry[0] >> 6) << 6);

			Tile128x128.VisualPlane[i] = (byte)(MappingEntry[0] >> 4);
			MappingEntry[0] -= 16 * (MappingEntry[0] >> 4);

			Tile128x128.Direction[i] = (byte)(MappingEntry[0] >> 2);
			MappingEntry[0] -= 4 * (MappingEntry[0] >> 2);

			Tile128x128.TileIndex[i] = MappingEntry[1] + (MappingEntry[0] << 8);
			Tile128x128.gfxDataPos[i] = Tile128x128.TileIndex[i] << 8;

			Tile128x128.CollisionFlags[i] = MappingEntry[2] >> 4;
			Tile128x128.CollisionFlagsB[i] = MappingEntry[2] - 16 * (MappingEntry[2] >> 4);
		}
		Reader::CloseFile();
	}

}

void Scene::LoadStageCollisions(void) {

}

void Scene::LoadStageGIFFile(int StageID) {

}

void Scene::LoadStageGFXFile(int StageID) {

}

void Scene::ResetBackgroundSettings(void) {
	for (int i = 0; i < LAYER_COUNT; ++i) {
		StageLayouts[i].DeformationOffset = 0;
		StageLayouts[i].DeformationOffsetW = 0;
		StageLayouts[i].ScrollPosition = 0;
	}

	for (int i = 0; i < 256; i++) {
		hParallax.ScrollPos[i] = 0;
		vParallax.ScrollPos[i] = 0;
	}

	for (int i = 0; i < DEFORM_COUNT; i++) {
		bgDeformationData0[i] = 0;
		bgDeformationData1[i] = 0;
		bgDeformationData2[i] = 0;
		bgDeformationData3[i] = 0;
	}
}