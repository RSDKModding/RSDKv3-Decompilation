#ifndef RENDER_3DS_H
#define RENDER_3DS_H

#define SPRITES_MAX 50

typedef struct {
	C2D_Sprite sprite;
	byte enabled = 0;
} _3ds_sprite;

extern C3D_Tex     _3ds_textureData[SURFACE_MAX];
extern _3ds_sprite _3ds_sprites[SPRITES_MAX];

void _3ds_cacheGfxSurface(int sheetID);
void _3ds_delGfxSurface(int sheetID);

#endif
