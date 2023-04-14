#ifndef MATH_H
#define MATH_H

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

//#define M_PI (3.1415927)

#define MEM_ZERO(x)  memset(&(x), 0, sizeof((x)))
#define MEM_ZEROP(x) memset((x), 0, sizeof(*(x)))

extern int sinMLookupTable[0x200];
extern int cosMLookupTable[0x200];

extern int sin512LookupTable[0x200];
extern int cos512LookupTable[0x200];

extern int sin256LookupTable[0x100];
extern int cos256LookupTable[0x100];

extern byte arcTan256LookupTable[0x100 * 0x100];

// Setup Angles
void CalculateTrigAngles();

inline int Sin512(int angle)
{
    if (angle < 0)
        angle = 0x200 - angle;
    angle &= 0x1FF;
    return sin512LookupTable[angle];
}

inline int Cos512(int angle)
{
    if (angle < 0)
        angle = 0x200 - angle;
    angle &= 0x1FF;
    return cos512LookupTable[angle];
}

inline int Sin256(int angle)
{
    if (angle < 0)
        angle = 0x100 - angle;
    angle &= 0xFFu;
    return sin256LookupTable[angle];
}

inline int Cos256(int angle)
{
    if (angle < 0)
        angle = 0x100 - angle;
    angle &= 0xFFu;
    return cos256LookupTable[angle];
}

// Get Arc Tan value
byte ArcTanLookup(int X, int Y);

#endif // !MATH_H
