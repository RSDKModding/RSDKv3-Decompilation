#include "../RetroEngine.hpp"

int spriteIndex;

C3D_Tex     _3ds_textureData[SURFACE_MAX];
C2D_Sprite  _3ds_sprites[SPRITES_MAX];

/*
   JeffRuLz's texture handling code from OpenHCL was referenced heavily here
   swapPixel, powOfTwo, and swizzle were also directly lifted from its code
   See here:  https://github.com/JeffRuLz/OpenHCL/blob/master/platform/3ds/source/graphics.cpp
*/

static inline void swapPixel(u16 &a, u16 &b)
{
	u32 tmp = a;
	a = b;
	b = tmp;
}

static inline void swizzle(u16* p, bool reverse)
{
	// swizzle foursome table
	static const unsigned char table[][4] =
	{
		{  2,  8, 16,  4, },
		{  3,  9, 17,  5, },
		{  6, 10, 24, 20, },
		{  7, 11, 25, 21, },
		{ 14, 26, 28, 22, },
		{ 15, 27, 29, 23, },
		{ 34, 40, 48, 36, },
		{ 35, 41, 49, 37, },
		{ 38, 42, 56, 52, },
		{ 39, 43, 57, 53, },
		{ 46, 58, 60, 54, },
		{ 47, 59, 61, 55, }
	};

	if (!reverse) {
		for (const auto &entry : table)
		{
			u16 tmp = p[entry[0]];
			p[entry[0]] = p[entry[1]];
			p[entry[1]] = p[entry[2]];
			p[entry[2]] = p[entry[3]];
			p[entry[3]] = tmp;
		}
	} else {
		for (const auto &entry : table) {
			u16 tmp = p[entry[3]];
			p[entry[3]] = p[entry[2]];
			p[entry[2]] = p[entry[1]];
			p[entry[1]] = p[entry[0]];
			p[entry[0]] = tmp;
		}
	}

	swapPixel(p[12], p[18]);
	swapPixel(p[13], p[19]);
	swapPixel(p[44], p[50]);
	swapPixel(p[45], p[51]);
}

static inline int powOfTwo(int in)
{
	int out = 8;
	while (out < in)
		out *= 2;
	return out;
}

static inline u32 RGB565_to_RGBA8(u16 pixel) {
	byte a = 0xff;
	byte r = (byte) (pixel & 0x001f);
	byte g = (byte) (pixel & 0x07e0) >> 5;
	byte b = (byte) (pixel & 0xf800) >> 11;

	return (r & (g << 8) & (b << 16) & (a << 24));
}

// NOTE: we going the mobile route for HW rendering and 
// will probably just use tints for mid-frame palette
// changes in stages like Tidal Tempest
// As such, there's no need to keep track of palette changes
// for sprites in VRAM
//
// decodes sprite into a texture using the current active palette data
void _3ds_cacheGfxSurface(int sheetID) {
	GFXSurface* surf = &gfxSurface[sheetID];
	int height = surf->height;
	int width  = surf->width;
	int depth  = surf->depth;
    	byte *gfxDataPtr   = &graphicData[surf->dataPosition];
        byte *lineBuffer   = &gfxLineBuffer[0];
	activePalette = fullPalette[*lineBuffer];

	if (width <= 0 || height <= 0)
		return;

	int x = 0, y = 0;
	int w = powOfTwo(width);
	int h = powOfTwo(height);

	ushort* buffer     = (ushort*) linearAlloc(w * h * sizeof(u16));
	ushort* bufferPtr  = buffer;
	byte* gptr = gfxDataPtr;
 	for (int i = 0; i < w * h; ) {
		// RGB 5 + 6 + 5 = 16 bits
		ushort  tile[8 * 8] = { 0 };
		ushort* tilePtr = tile;
		
		for (int ty = 0; ty < 8; ty++) {
			gptr = gfxDataPtr + (width * y) + x;
			for (int tx = 0; tx < 8; tx++) {
				if (x < width && y < height)
					*tilePtr = activePalette[*gptr];
				tilePtr++;

				gptr++;
				x++;
				i++;
			}

			x -= 8;
			y++;
		}

		x += 8;
		y -= 8;

		if (x >= w - 1) {
			x =  0;
			y += 8;
		}

		swizzle(tile, false);
		
		for (int a = 0; a < 8 * 8; a++)
			*(bufferPtr++) = tile[a];
	}

	C3D_TexInitVRAM(&_3ds_textureData[sheetID], w, h, GPU_RGB565);
	C3D_TexUpload(&_3ds_textureData[sheetID], buffer);

	linearFree(buffer);
}

void _3ds_delGfxSurface(int sheetID) {
	C3D_TexDelete(&_3ds_textureData[sheetID]);
}

