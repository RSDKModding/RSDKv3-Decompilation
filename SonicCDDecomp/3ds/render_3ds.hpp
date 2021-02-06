#ifndef RENDER_3DS_H
#define RENDER_3DS_H

#define SPRITES_MAX 768

typedef struct {
	int sid;

	float xscale;
	float yscale;

	float angle;

	C3D_Tex* tex;
	Tex3DS_SubTexture subtex;
	C2D_DrawParams params;
} _3ds_sprite;

extern int spriteIndex;

extern C3D_Tex      _3ds_textureData[SURFACE_MAX];
extern _3ds_sprite  _3ds_sprites[SPRITES_MAX];

void _3ds_cacheGfxSurface(int sheetID);
void _3ds_delGfxSurface(int sheetID);
void _3ds_prepSprite(int XPos, int YPos, int width, int height, 
		     int sprX, int sprY, int sheetID, int direction,
		     float scaleX, float scaleY, float angle);
#endif
