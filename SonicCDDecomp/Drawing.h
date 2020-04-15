#ifndef DRAWING_H
#define DRAWING_H

namespace Drawing {

	class DrawList {
	public:
		int EntityRefs[OBJECT_COUNT];
		int ListSize;
	};

	static short BlendLookupTable[0x2000];
	static short SubtractLookupTable[0x2000];
	static short TintLookupTable[0x10000];

	static int SCREEN_XSIZE = 424;

	int InitRenderDevice(void);
	void GenerateBlendLookupTable(void);

	//Layer Drawing
	void DrawObjectList(int Layer);
	void DrawStageGFX(void);

	//TileLayer Drawing
	void DrawHLineScrollLayer(int LayerID);
	void DrawVLineScrollLayer(int LayerID);
	void Draw3DFloorLayer(int LayerID);
	void Draw3DSkyLayer(int LayerID);


	//Shape Drawing
	void DrawRectangle(int XPos, int YPos, int Width, int Height, int R, int G, int B, int A);
}

#endif // !DRAWING_H
