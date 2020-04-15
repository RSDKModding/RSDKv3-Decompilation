#ifndef PALETTE_H
#define PALETTE_H

#define PALETTE_COUNT (0x8)
#define PALETTE_SIZE (0x100)

namespace Palette {

	//Raw Palette Colours
	static byte FullPalette_R[PALETTE_COUNT][PALETTE_SIZE];
	static byte FullPalette_G[PALETTE_COUNT][PALETTE_SIZE];
	static byte FullPalette_B[PALETTE_COUNT][PALETTE_SIZE];

	//Palettes (as RGB565 Colours)
	static ushort FullPalette[PALETTE_COUNT][PALETTE_SIZE];
	static ushort* ActivePalette; //Ptr to the 256 colour set thats active

	static byte GFXLineBuffer[PALETTE_SIZE]; //Pointers to active palette

	static int FadeMode = 0;
	static byte FadeA = 0;
	static byte FadeR = 0;
	static byte FadeG = 0;
	static byte FadeB = 0;

	static int PaletteMode = 0;

	void LoadPalette(const char* Filepath, signed int PaletteID, int StartPalleteIndex, int StartIndex, int EndIndex);

	void SetActivePalette(byte NewActivePal, int StartLine, int EndLine);
}

#endif // !PALETTE_H
