#include "RetroEngine.h"

int Drawing::InitRenderDevice(void) {
    char GameTitle[0x40];

    sprintf(GameTitle, "%s %s", Engine.GameWindowText, Engine.usingDataFile ? "(Using Data Folder)" : "");

    return 1;
}

void Drawing::GenerateBlendLookupTable(void) {
    int TintValue; 
    int BlendTableID;

    BlendTableID = 0;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 32; j++) {
            Drawing::BlendLookupTable[BlendTableID] = i * j >> 8;
            Drawing::SubtractLookupTable[BlendTableID++] = i * (31 - j) >> 8;
        }
    }

    for (int k = 0; k < 0x10000; k++) {
        TintValue = ((k & 0x1F) + ((k & 0x7E0) >> 6) + ((k & 0xF800) >> 11)) / 3 + 6;
        if (TintValue > 31)
            TintValue = 31;
        Drawing::TintLookupTable[k] = 2113 * TintValue;
    }
}

void Drawing::DrawObjectList(int Layer) {

}

void Drawing::DrawStageGFX(void) {
    Scene::WaterDrawPos = Scene::WaterLevel - Scene::yScrollOffset;
    if (Scene::WaterDrawPos < 0) {
        Scene::WaterDrawPos = 0;
    }

    if (Scene::WaterDrawPos > 240) {
        Scene::WaterDrawPos = 240;
    }

    DrawObjectList(0);
    if (Scene::ActiveTileLayers[0] < LAYER_COUNT) {
        switch (Scene::StageLayouts[Scene::ActiveTileLayers[0]].Type)
        {
        case 1:
            DrawHLineScrollLayer(0);
            break;
        case 2:
            DrawVLineScrollLayer(0);
            break;
        case 3:
            Draw3DFloorLayer(0);
            break;
        case 4:
            Draw3DSkyLayer(0);
            break;
        default:
            break;
        }
    }

    DrawObjectList(1);
    if (Scene::ActiveTileLayers[0] < LAYER_COUNT) {
        switch (Scene::StageLayouts[Scene::ActiveTileLayers[1]].Type)
        {
        case 1:
            DrawHLineScrollLayer(0);
            break;
        case 2:
            DrawVLineScrollLayer(0);
            break;
        case 3:
            Draw3DFloorLayer(0);
            break;
        case 4:
            Draw3DSkyLayer(0);
            break;
        default:
            break;
        }
    }

    DrawObjectList(2);
    if (Scene::ActiveTileLayers[0] < LAYER_COUNT) {
        switch (Scene::StageLayouts[Scene::ActiveTileLayers[2]].Type)
        {
        case 1:
            DrawHLineScrollLayer(0);
            break;
        case 2:
            DrawVLineScrollLayer(0);
            break;
        case 3:
            Draw3DFloorLayer(0);
            break;
        case 4:
            Draw3DSkyLayer(0);
            break;
        default:
            break;
        }
    }

    DrawObjectList(3);
    DrawObjectList(4);
    if (Scene::ActiveTileLayers[0] < LAYER_COUNT) {
        switch (Scene::StageLayouts[Scene::ActiveTileLayers[3]].Type)
        {
        case 1:
            DrawHLineScrollLayer(0);
            break;
        case 2:
            DrawVLineScrollLayer(0);
            break;
        case 3:
            Draw3DFloorLayer(0);
            break;
        case 4:
            Draw3DSkyLayer(0);
            break;
        default:
            break;
        }
    }

    DrawObjectList(5);
    DrawObjectList(6);

    if (Palette::FadeMode > 0) {
        DrawRectangle(0, 0, SCREEN_XSIZE, SCREEN_HEIGHT, Palette::FadeR, Palette::FadeG, Palette::FadeB, Palette::FadeA);
    }
}

void Drawing::DrawHLineScrollLayer(int LayerID) {

}

void Drawing::DrawVLineScrollLayer(int LayerID) {

}

void Drawing::Draw3DFloorLayer(int LayerID) {

}

void Drawing::Draw3DSkyLayer(int LayerID) {

}



void Drawing::DrawRectangle(int XPos, int YPos, int Width, int Height, int R, int G, int B, int A) {

}