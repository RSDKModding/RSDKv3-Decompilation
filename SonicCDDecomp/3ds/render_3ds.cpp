#include "../RetroEngine.hpp"

C3D_Tex      _3ds_textureData[SURFACE_MAX];
_3ds_sprite  _3ds_sprites[SPRITES_MAX];

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
    	byte *gfxDataPtr = &graphicData[surf->dataPosition];

	byte* buffer     = (byte*) malloc(width * height);
	byte* bufferPtr  = buffer;
        byte *lineBuffer = &gfxLineBuffer[0];
	if (width <= 0 || height <= 0)
		return;

	activePalette = fullPalette[*lineBuffer];

    	while (height--) {
        	        	int w = width;
        	while (w--) {
            		if (*gfxDataPtr > 0)
				*bufferPtr = activePalette[*gfxDataPtr];
            		++gfxDataPtr;
        	}

    	}

	C3D_TexInit(&_3ds_textureData[sheetID], 
			gfxSurface[sheetID].width,
			gfxSurface[sheetID].height,
			GPU_RGBA8);
	C3D_TexLoadImage(&_3ds_textureData[sheetID], buffer, GPU_TEXFACE_2D, 0);

	free(buffer);
}

void _3ds_delGfxSurface(int sheetID) {
	C3D_TexDelete(&_3ds_textureData[sheetID]);
}
