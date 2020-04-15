#ifndef MATH_H
#define MATH_H

#define M_PI						(3.1415927)

#define MEM_ZERO(x) memset(&(x), 0, sizeof((x)))
#define MEM_ZEROP(x) memset((x), 0, sizeof(*(x)))

namespace Math {

	static int SinM[512];
	static int CosM[512];

	static int Sin512[512];
	static int Cos512[512];

	static int Sin256[256];
	static int Cos256[256];

	static int ATan256[0x10000];

	//Setup Angles
	void CalculateTrigAngles(void);

	//Get Arc Tan value
	byte ArcTanLookup(int X, int Y);
}

#endif // !MATH_H
