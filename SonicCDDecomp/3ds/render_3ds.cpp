#include "../RetroEngine.hpp"

// https://github.com/bubble2k16/snes9x_3ds/blob/3e5cdba3577aafefb0860966a3daf694ece8e168/source/pixform.h#L248
#define BUILD_PIXEL_RGB5551(R,G,B) (((int) (R) << 11) | ((int) (G) << 6) | (int) ((B) << 1) | 1)
#define RGB565_to_RGBA5551(px) (BUILD_PIXEL_RGB5551( (px & 0xf800) >> 11, (px & 0x07e0) >> 6, (px & 0x001f)))

int spriteIndex = 0;

C3D_Tex      _3ds_textureData[SURFACE_MAX];
_3ds_sprite  _3ds_sprites[SPRITES_MAX];

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

	u16* buffer     = (u16*) linearAlloc(w * h * sizeof(u16));
	u16* bufferPtr  = buffer;
	byte* gptr = gfxDataPtr;
 	for (int i = 0; i < w * h; ) {
		u16  tile[8 * 8] = { 0 };
		u16* tilePtr = tile;
		
		for (int ty = 0; ty < 8; ty++) {
			gptr = gfxDataPtr + (width * y) + x;
			for (int tx = 0; tx < 8; tx++) {
				if (x < width && y < height) {
					if (*gptr >> 0)
						*tilePtr = RGB565_to_RGBA5551(activePalette[*gptr]);
					else
						*tilePtr = 0;
				}
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

	C3D_TexInit(&_3ds_textureData[sheetID], w, h, GPU_RGBA5551);
	C3D_TexUpload(&_3ds_textureData[sheetID], buffer);

	linearFree(buffer);
}

void _3ds_delGfxSurface(int sheetID) {
	C3D_TexDelete(&_3ds_textureData[sheetID]);
}

void _3ds_prepSprite(int XPos, int YPos, int width, int height, 
		     int sprX, int sprY, int sheetID, int direction,
		     float scaleX, float scaleY, float angle) {
    	// we don't actually draw the sprite immediately, we only 
    	// set up a sprite to be drawn next C2D_SceneBegin
    	if (spriteIndex < SPRITES_MAX) {
		_3ds_sprite spr;

		// set up reference to texture
		spr.sid = sheetID;

		// set up subtexture
		spr.subtex.width  = gfxSurface[sheetID].width;
		spr.subtex.height = gfxSurface[sheetID].height;
		spr.subtex.left   = (float) sprX                / _3ds_textureData[sheetID].width;
		spr.subtex.top    = 1 - (float) sprY            / _3ds_textureData[sheetID].height;
		spr.subtex.right  = (float) (sprX + width)      / _3ds_textureData[sheetID].width;
		spr.subtex.bottom = 1 - (float) (sprY + height) / _3ds_textureData[sheetID].height;

		// set up draw params
		spr.params.pos.x = XPos;
		spr.params.pos.y = YPos;
		switch (direction) {
			case FLIP_X:
				spr.params.pos.w = -width * scaleX;
				spr.params.pos.h = height * scaleY;
				break;
			case FLIP_Y:
				spr.params.pos.w = width   * scaleX;
				spr.params.pos.h = -height * scaleY;
				break;
			case FLIP_XY:
				spr.params.pos.w = -width  * scaleX;
				spr.params.pos.h = -height * scaleY;
				break;
			default:
				spr.params.pos.w = width  * scaleX;
				spr.params.pos.h = height * scaleY;
				break;
		}

		spr.params.center.x = (spr.subtex.right - spr.subtex.left)   / 2;
		spr.params.center.y = (spr.subtex.top   - spr.subtex.bottom) / 2;
		spr.params.depth = 0;
		spr.params.angle = angle;

		_3ds_sprites[spriteIndex] = spr;
    	}
}
