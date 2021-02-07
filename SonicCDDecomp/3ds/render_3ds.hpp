#ifndef RENDER_3DS_H
#define RENDER_3DS_H

#define SPRITES_MAX 256
#define TILES_MAX_3DS 768
#define TILE_MAXSIZE 6    	// arbitrary, but I'm banking on the number of colors cycled
				// to be around this

typedef struct {
	int sid;

	float xscale;
	float yscale;

	float angle;

	C3D_Tex* tex;
	Tex3DS_SubTexture subtex;
	C2D_DrawParams params;
} _3ds_sprite;

typedef struct {
	Tex3DS_SubTexture subtex;
	C2D_DrawParams params;
} _3ds_tile;

extern int spriteIndex;
extern int tileIndex;

extern byte paletteIndex;
extern byte cachedPalettes;
extern byte maxPaletteCycles;

// actual texture data
extern C3D_Tex      _3ds_textureData[SURFACE_MAX];
extern C3D_Tex      _3ds_tilesetData[TILE_MAXSIZE];

// cropped textures for sprites and tiles
extern _3ds_sprite  _3ds_sprites[SPRITES_MAX];
extern _3ds_tile    _3ds_tiles[TILES_MAX_3DS];

void _3ds_cacheSpriteSurface(int sheetID);
void _3ds_delSpriteSurface(int sheetID);
void _3ds_cacheTileSurface(byte* tilesetGfxPtr);
void _3ds_delTileSurface();
void _3ds_cacheGfxSurface(byte* gfxDataPtr, C3D_Tex* dst,
			  int width, int height, bool write);
void _3ds_prepSprite(int XPos, int YPos, int width, int height, 
		     int sprX, int sprY, int sheetID, int direction,
		     float scaleX, float scaleY, float angle);
void _3ds_prepTile(int XPos, int YPos, int tileX, int tileY, int direction);
#endif
