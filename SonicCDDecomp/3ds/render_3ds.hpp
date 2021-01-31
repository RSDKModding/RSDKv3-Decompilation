#ifndef RENDER_3DS_H
#define RENDER_3DS_H

extern C3D_Tex _3ds_textureData[SURFACE_MAX];

void _3ds_cacheGfxSurface(int sheetID);
void _3ds_delGfxSurface(int sheetID);

#endif
