#ifndef SPRITES_H
#define SPRITES_H

namespace Sprites {

	class GFXSurface {
	public:
		char Filename[0x40];
		int Height;
		int Width;
		int WidthShifted;
		int Depth;
		int dataPosition;
	};

}

#endif // !SPRITES_H
