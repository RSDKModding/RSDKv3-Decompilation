#ifndef PALETTE_H
#define PALETTE_H

#define PALETTE_COUNT (0x8)
#define PALETTE_SIZE  (0x100)

struct PaletteEntry {
    byte r;
    byte g;
    byte b;
};

// Palettes (as RGB565 Colours)
extern PaletteEntry fullPalette32[PALETTE_COUNT][PALETTE_SIZE];
extern ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];
extern ushort *activePalette; // Ptr to the 256 colour set thats active
extern PaletteEntry *activePalette32;

extern byte gfxLineBuffer[SCREEN_YSIZE]; // Pointers to active palette

extern int fadeMode;
extern byte fadeA;
extern byte fadeR;
extern byte fadeG;
extern byte fadeB;

extern int paletteMode;

void LoadPalette(const char *filePath, int paletteID, int startPaletteIndex, int startIndex, int endIndex);

inline void SetActivePalette(byte newActivePal, int startLine, int endLine)
{
    if (newActivePal < PALETTE_COUNT)
        while (startLine++ < endLine) gfxLineBuffer[startLine % SCREEN_YSIZE] = newActivePal;

    activePalette   = fullPalette[gfxLineBuffer[0]];
    activePalette32 = fullPalette32[gfxLineBuffer[0]];
}

inline void SetPaletteEntry(byte paletteIndex, byte r, byte g, byte b)
{
    activePalette[paletteIndex]     = ((int)b >> 3) | 32 * ((int)g >> 2) | ((ushort)((int)r >> 3) << 11);
    activePalette32[paletteIndex].r = r;
    activePalette32[paletteIndex].g = g;
    activePalette32[paletteIndex].b = b;
}

inline void CopyPalette(byte src, byte dest)
{
    if (src < PALETTE_COUNT && dest < PALETTE_COUNT) {
        for (int i = 0; i < PALETTE_SIZE; ++i) {
            fullPalette[dest][i] = fullPalette[src][i];
            fullPalette32[dest][i] = fullPalette32[src][i];
        }
    }
}

inline void RotatePalette(byte startIndex, byte endIndex, bool right)
{
    if (right) {
        ushort startClr         = activePalette[endIndex];
        PaletteEntry startClr32 = activePalette32[startIndex];
        for (int i = endIndex; i > startIndex; --i) {
            activePalette[i]   = activePalette[i - 1];
            activePalette32[i] = activePalette32[i - 1];
        }
        activePalette[startIndex] = startClr;
        activePalette32[endIndex] = startClr32;
    }
    else {
        ushort startClr         = activePalette[startIndex];
        PaletteEntry startClr32 = activePalette32[startIndex];
        for (int i = startIndex; i < endIndex; ++i) {
            activePalette[i]   = activePalette[i + 1];
            activePalette32[i] = activePalette32[i + 1];
        }
        activePalette[endIndex]   = startClr;
        activePalette32[endIndex] = startClr32;
    }
}

inline void SetFade(byte R, byte G, byte B, ushort A)
{
    fadeMode = 1;
    fadeR    = R;
    fadeG    = G;
    fadeB    = B;
    fadeA    = A > 0xFF ? 0xFF : A;
}
void SetLimitedFade(byte paletteID, byte R, byte G, byte B, ushort alpha, int startIndex, int endIndex);

#endif // !PALETTE_H
