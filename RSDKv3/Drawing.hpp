#ifndef DRAWING_H
#define DRAWING_H

#define SURFACE_MAX (24)
#define GFXDATA_MAX (0x800 * 0x800)

#define BLENDTABLE_YSIZE (0x100)
#define BLENDTABLE_XSIZE (0x20)
#define BLENDTABLE_SIZE  (BLENDTABLE_XSIZE * BLENDTABLE_YSIZE)
#define TINTTABLE_SIZE   (0x1000)

#define DRAWLAYER_COUNT (7)

enum FlipFlags { FLIP_NONE, FLIP_X, FLIP_Y, FLIP_XY };
enum InkFlags { INK_NONE, INK_BLEND, INK_ALPHA, INK_ADD, INK_SUB };
enum DrawFXFlags { FX_SCALE, FX_ROTATE, FX_ROTOZOOM, FX_INK, FX_TINT, FX_FLIP };

struct DrawListEntry {
    int entityRefs[ENTITY_COUNT];
    int listSize;
};

struct GFXSurface {
    char fileName[0x40];
    int height;
    int width;
    int widthShifted;
    int texStartX;
    int texStartY;
    int dataPosition;
};

extern short blendLookupTable[BLENDTABLE_SIZE];
extern short subtractLookupTable[BLENDTABLE_SIZE];
extern short tintLookupTable[TINTTABLE_SIZE];

extern int SCREEN_XSIZE;
extern int SCREEN_CENTERX;
extern int SCREEN_XSIZE_CONFIG;

extern int touchWidth;
extern int touchHeight;

extern float videoAR;
extern bool videoPlaying;

extern DrawListEntry drawListEntries[DRAWLAYER_COUNT];

extern int gfxDataPosition;
extern GFXSurface gfxSurface[SURFACE_MAX];
extern byte graphicData[GFXDATA_MAX];

#define VERTEX_LIMIT        (0x2000)
#define INDEX_LIMIT         (VERTEX_LIMIT * 6)
#define VERTEX3D_LIMIT      (0x1904)
#define TILEUV_SIZE         (0x1000)
#define HW_TEXTURE_LIMIT    (6)
#define HW_TEXTURE_SIZE     (0x400)
#define HW_TEXTURE_DATASIZE (HW_TEXTURE_SIZE * HW_TEXTURE_SIZE * 2)
#define HW_TEXBUFFER_SIZE   (HW_TEXTURE_SIZE * HW_TEXTURE_SIZE)

struct DrawVertex {
    short x;
    short y;
    short u;
    short v;

    Colour colour;
};

struct DrawVertex3D {
    float x;
    float y;
    float z;
    short u;
    short v;

    Colour colour;
};

extern DrawVertex gfxPolyList[VERTEX_LIMIT];
extern short gfxPolyListIndex[INDEX_LIMIT];
extern ushort gfxVertexSize;
extern ushort gfxVertexSizeOpaque;
extern ushort gfxIndexSize;
extern ushort gfxIndexSizeOpaque;

extern DrawVertex3D polyList3D[VERTEX3D_LIMIT];

extern ushort vertexSize3D;
extern ushort indexSize3D;
extern ushort tileUVArray[TILEUV_SIZE];
extern float floor3DXPos;
extern float floor3DYPos;
extern float floor3DZPos;
extern float floor3DAngle;
extern bool render3DEnabled;
extern bool hq3DFloorEnabled;

extern ushort texBuffer[HW_TEXBUFFER_SIZE];
extern byte texBufferMode;

#if !RETRO_USE_ORIGINAL_CODE
extern int viewOffsetX;
#endif
extern int viewWidth;
extern int viewHeight;
extern float viewAspect;
extern int bufferWidth;
extern int bufferHeight;
extern int virtualX;
extern int virtualY;
extern int virtualWidth;
extern int virtualHeight;
extern float viewAngle;
extern float viewAnglePos;

#if RETRO_USING_OPENGL
extern GLuint gfxTextureID[HW_TEXTURE_LIMIT];
extern GLuint framebufferHW;
extern GLuint renderbufferHW;
extern GLuint retroBuffer;
extern GLuint retroBuffer2x;
extern GLuint videoBuffer;
#endif
extern DrawVertex screenRect[4];
extern DrawVertex retroScreenRect[4];

int InitRenderDevice();
void FlipScreen();
void FlipScreenFB();
void FlipScreenNoFB();
void FlipScreenHRes();
void RenderFromTexture();
void RenderFromRetroBuffer();

void FlipScreenVideo();

void ReleaseRenderDevice();

void setFullScreen(bool fs);

void GenerateBlendLookupTable();

inline void ClearGraphicsData()
{
    for (int i = 0; i < SURFACE_MAX; ++i) StrCopy(gfxSurface[i].fileName, "");
    gfxDataPosition = 0;
}
void ClearScreen(byte index);

void SetScreenSize(int width, int height);
void CopyFrameOverlay2x();
void TransferRetroBuffer();

inline bool CheckSurfaceSize(int size)
{
    for (int cnt = 2; cnt < 2048; cnt <<= 1) {
        if (cnt == size)
            return true;
    }
    return false;
}

void UpdateHardwareTextures();
void SetScreenDimensions(int width, int height, int winWidth, int winHeight);
void ScaleViewport(int width, int height);
void CalcPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

void SetupPolygonLists();
void UpdateTextureBufferWithTiles();
void UpdateTextureBufferWithSortedSprites();
void UpdateTextureBufferWithSprites();

// Layer Drawing
void DrawObjectList(int layer);
void DrawStageGFX();

// TileLayer Drawing
void DrawHLineScrollLayer(int layerID);
void DrawVLineScrollLayer(int layerID);
void Draw3DFloorLayer(int layerID);
void Draw3DSkyLayer(int layerID);

// Shape Drawing
void DrawRectangle(int XPos, int YPos, int width, int height, int R, int G, int B, int A);
void SetFadeHQ(int R, int G, int B, int A);
void DrawTintRectangle(int XPos, int YPos, int width, int height);
void DrawScaledTintMask(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                        int sheetID);

// Sprite Drawing
void DrawSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID);
void DrawSpriteFlipped(int XPos, int YPos, int width, int height, int sprX, int sprY, int direction, int sheetID);
void DrawSpriteScaled(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                      int sheetID);
void DrawScaledChar(int direction, int XPos, int YPos, int pivotX, int pivotY, int scaleX, int scaleY, int width, int height, int sprX, int sprY,
                    int sheetID);
void DrawSpriteRotated(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY, int width, int height, int rotation,
                       int sheetID);
void DrawSpriteRotozoom(int direction, int XPos, int YPos, int pivotX, int pivotY, int sprX, int sprY, int width, int height, int rotation, int scale,
                        int sheetID);

void DrawBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int sheetID);
void DrawAlphaBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID);
void DrawAdditiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID);
void DrawSubtractiveBlendedSprite(int XPos, int YPos, int width, int height, int sprX, int sprY, int alpha, int sheetID);

void DrawObjectAnimation(void *objScr, void *ent, int XPos, int YPos);

void DrawFace(void *v, uint colour);
void DrawTexturedFace(void *v, byte sheetID);

void DrawBitmapText(void *menu, int XPos, int YPos, int scale, int spacing, int rowStart, int rowCount);

void DrawTextMenu(void *menu, int XPos, int YPos);
void DrawTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawStageTextEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawBlendedTextMenuEntry(void *menu, int rowID, int XPos, int YPos, int textHighlight);
void DrawBitmapText(void *menu, int XPos, int YPos, int scale, int spacing, int rowStart, int rowCount);

#endif // !DRAWING_H
