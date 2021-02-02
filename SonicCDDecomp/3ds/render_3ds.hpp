#ifndef RENDER_3DS_H
#define RENDER_3DS_H

#define SPRITES_MAX 768

extern int spriteIndex;

extern C3D_Tex     _3ds_textureData[SURFACE_MAX];
extern C2D_Sprite  _3ds_sprites[SPRITES_MAX];

void _3ds_cacheGfxSurface(int sheetID);
void _3ds_delGfxSurface(int sheetID);

#endif
