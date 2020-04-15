#include "RetroEngine.h"

void Palette::LoadPalette(const char* Filepath, signed int PaletteID, int StartPalleteIndex, int StartIndex, int EndIndex) {
    Reader::FileInfo FileInfo;
    char FullPath[0x80];

    byte Colour[3];

    StringUtils::StrCopy(FullPath, (char*)"Data/Palettes/");
    StringUtils::StrAdd(FullPath, (char*)Filepath);
    
    if (Reader::LoadFile(FullPath, &FileInfo)) {
        Reader::SetFilePosition(3 * StartIndex);
        if (PaletteID > 7 || PaletteID < 0) {
            PaletteID = 0;
        }

        if (PaletteID) {
            for (int i = StartIndex; i < EndIndex; i++) {
                Reader::FileRead(&Colour, 3);
                Palette::FullPalette[PaletteID][StartPalleteIndex++] = (ushort)((Colour[2] >> 3) | 32 * (Colour[1] >> 2) | ((Colour[0] >> 3) << 11));
            }
        }
        else {
            for (int i = StartIndex; i < EndIndex; i++) {
                Reader::FileRead(&Colour, 3);
                Palette::FullPalette[PaletteID][StartPalleteIndex] = (ushort)((Colour[2] >> 3) | 32 * (Colour[1] >> 2) | ((Colour[0] >> 3) << 11));
                Palette::FullPalette_R[PaletteID][StartPalleteIndex] = Colour[0];
                Palette::FullPalette_G[PaletteID][StartPalleteIndex] = Colour[1];
                Palette::FullPalette_B[PaletteID][StartPalleteIndex++] = Colour[2];
            }
        }
        Reader::CloseFile();
    }

}

void Palette::SetActivePalette(byte NewActivePal, int StartLine, int EndLine) {

}