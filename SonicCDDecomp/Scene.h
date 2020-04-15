#ifndef SCENE_H
#define SCENE_H

#define LAYER_COUNT (9)
#define DEFORM_COUNT (576)

namespace Scene {

	class SceneInfo {
	public:
		char Name[0x40];
		char Folder[0x40];
		char ID[0x40];
		byte Mode;
	};

	class CollisionMasks {
	public:
		byte FloorMasks[0x4000];
		byte LWallMasks[0x4000];
		byte RWallMasks[0x4000];
		byte RoofMasks[0x4000];
		int Angles[0x400];
		byte Flags[0x400];
	};

	class TileLayer {
	public:
		ushort Tiles[0x10000];
		byte LineScroll[0x8000];
		int ParallaxFactor;
		int ScrollSpeed;
		int ScrollPosition;
		int Angle;
		int XPos;
		int YPos;
		int ZPos;
		int DeformationOffset;
		int DeformationOffsetW;
		byte Type;
		byte Width;
		byte Height;
		byte field_28027;
	};

	class LineScroll {
	public:
		int ParallaxFactor[0x100];
		int ScrollSpeed[0x100];
		int ScrollPosition[0x100];
		int ScrollPos[0x100];
		int Deform[0x100];
		byte EntryCount;
	};

	class Tiles128x128 {
	public:
		int		gfxDataPos[0x8000];
		ushort	TileIndex[0x8000];
		byte	Direction[0x8000];
		byte	VisualPlane[0x8000];
		byte	CollisionFlags[0x8000];
		byte	CollisionFlagsB[0x8000];
	};

	static int StageListCount[4];
	static SceneInfo StageList[4][0x100];

	static int StageMode = 0;

	static int CameraTarget = -1;
	static int CameraStyle = 0;
	static bool CameraEnabled = 0;
	static int CameraAdjustY = 0;
	static int xScrollOffset = 0;
	static int yScrollOffset = 0;
	static int yScrollA = 0;
	static int yScrollB = 240;
	static int xScrollA = 0;
	static int xScrollB = 320;
	static int yScrollMove = 0;
	static int CameraShakeX = 0;
	static int CameraShakeY = 0;

	static int lastXSize = -1;
	static int lastYSize = -1;

	static int WaterLevel = 0;
	static int WaterDrawPos = 0;

	static bool PauseEnabled = false;
	static bool TimeEnabled = false;
	static int StageTimer = 0;
	static int StageMilliseconds = 0;
	static int StageSeconds = 0;
	static int StageMinutes = 0;

	//Category and Scene IDs
	static int ActiveStageList = 0;
	static int StageListPosition = 0;

	static byte ActiveTileLayers[4];
	static byte TLayerMidPoint;
	static TileLayer StageLayouts[LAYER_COUNT];

	static int bgDeformationData0[DEFORM_COUNT];
	static int bgDeformationData1[DEFORM_COUNT];
	static int bgDeformationData2[DEFORM_COUNT];
	static int bgDeformationData3[DEFORM_COUNT];

	static LineScroll hParallax;
	static LineScroll vParallax;

	static Tiles128x128 Tile128x128;

	void InitFirstStage(void);
	void ProcessStage(void);

	void ResetBackgroundSettings(void);

	void LoadStageFiles(void);
	int LoadActFile(const char* Extention, int StageID, Reader::FileInfo* FileInfo);
	int LoadStageFile(const char* FileName, int StageID, Reader::FileInfo* FileInfo);

	void LoadActLayout(void);
	void LoadStageBackground(void);
	void LoadStageChunks(void);
	void LoadStageCollisions(void);
	void LoadStageGIFFile(int StageID);
	void LoadStageGFXFile(int StageID);
}

#endif // !SCENE_H
