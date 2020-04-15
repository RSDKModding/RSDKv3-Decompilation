#ifndef SCENE_H
#define SCENE_H

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
		byte	CollisionFlags[0x10000];
	};

	static int StageListCount[4];
	static SceneInfo StageList[4][0x100];

	static int StageMode = 0;

	void InitFirstStage(void);
	void ProcessStage(void);
}

#endif // !SCENE_H
