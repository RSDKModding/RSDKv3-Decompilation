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

	int InitRenderDevice(void);
	void GenerateBlendLookupTable(void);
}

#endif // !DRAWING_H
