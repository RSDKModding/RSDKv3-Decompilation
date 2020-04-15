#include "RetroEngine.h"
#include <math.h>

void Math::CalculateTrigAngles(void) {

    for (int i = 0; i < 512; i++)
    {
        float Val = sin(((float)i / 256.0) * M_PI);
        Math::SinM[i] = (Val * 4096.0);
        Val = cos(((float)i / 256.0) * M_PI);
        Math::CosM[i] = (Val * 4096.0);
    }

    Math::CosM[0] = 4096;
    Math::CosM[128] = 0;
    Math::CosM[256] = -4096;
    Math::CosM[384] = 0;
    Math::SinM[0] = 0;
    Math::SinM[128] = 4096;
    Math::SinM[256] = 0;
    Math::SinM[384] = -4096;

    for (int i = 0; i < 512; i++) {
        float Val = sinf(((float)i / 256) * M_PI);
        Math::Sin512[i] = (signed int)(Val * 512.0);
        Val = cosf(((float)i / 256) * M_PI);
        Math::Cos512[i] = (signed int)(Val * 512.0);
    }

    Math::Cos512[0] = 512;
    Math::Cos512[128] = 0;
    Math::Cos512[256] = -512;
    Math::Cos512[384] = 0;
    Math::Sin512[0] = 0;
    Math::Sin512[128] = 512;
    Math::Sin512[256] = 0;
    Math::Sin512[384] = -512;

    for (int i = 0; i < 256; i++) {
        Math::Sin256[i] = (Math::Sin512[i * 2] >> 1);
        Math::Cos256[i] = (Math::Cos512[i * 2] >> 1);
    }

    for (int X = 0; X < 256; X++) {
        for (int Y = 0; Y < 256;) {
            double angle = atan2(Y, X);
            Math::ATan256[(0x100 * X) + Y++] = (signed int)(float)(angle * 40.743664);
        }
    }

}

byte Math::ArcTanLookup(int X, int Y) {
    int XVal;
    byte Result;
    int YVal;

    if (X >= 0) {
        XVal = X;
    }
    else {
        XVal = -X;
    }

    if (Y >= 0) {
        YVal = Y;
    }
    else {
        YVal = -Y;
    }

    if (XVal <= YVal) {
        while (YVal > 255) {
            XVal >>= 4;
            YVal >>= 4;
        }
    }
    else {
        while (XVal > 255) {
            XVal >>= 4;
            YVal >>= 4;
        }
    }
    if (X <= 0) {
        if (Y <= 0)
            Result = Math::ATan256[256 * XVal + YVal] + -128;
        else
            Result = -128 - Math::ATan256[256 * XVal + YVal];
    }
    else if (Y <= 0) {
        Result = -Math::ATan256[256 * XVal + YVal];
    }
    else {
        Result = Math::ATan256[256 * XVal + YVal];
    }
    return Result;
}