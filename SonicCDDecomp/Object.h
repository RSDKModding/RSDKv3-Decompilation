#ifndef OBJECT_H
#define OBJECT_H

#define OBJECT_COUNT (1184)

namespace Object {

	class Entity {
	public:
		int XPos;
		int YPos;
		int Values[8];
		int Scale;
		int Rotation;
		int AnimationTimer;
		int AnimationSpeed;
		byte Type;
		byte PropertyValue;
		byte State;
		byte Priority;
		byte DrawOrder;
		byte Direction;
		byte InkEffect;
		byte Alpha;
		byte Animation;
		byte PrevAnimation;
		byte Frame;
	};

	static int objectLoop = 0;
	static Entity objectEntityList[OBJECT_COUNT];
}

#endif // !OBJECT_H
