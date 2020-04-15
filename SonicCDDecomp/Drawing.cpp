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