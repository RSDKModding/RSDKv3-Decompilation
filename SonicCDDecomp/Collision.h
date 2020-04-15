#ifndef COLLISION_H
#define COLLISION_H

namespace Collision {

	class CollisionSensor {
		int XPos;
		int YPos;
		int Angle;
		RSDK_BOOL Collided;
	};

	class HitboxInfo {
	public:
		byte CollisionLeft_Floor;
		byte AltCollisionLeft_Floor;
		byte CollisionLeft_LWall;
		byte AltCollisionLeft_LWall;
		byte CollisionLeft_Ceiling;
		byte AltCollisionLeft_Ceiling;
		byte CollisionLeft_RWall;
		byte AltCollisionLeft_RWall;
		byte CollisionTop_Floor;
		byte AltCollisionTop_Floor;
		byte CollisionTop_LWall;
		byte AltCollisionTop_LWall;
		byte CollisionTop_Ceiling;
		byte AltCollisionTop_Ceiling;
		byte CollisionTop_RWall;
		byte AltCollisionTop_RWall;
		byte CollisionRight_Floor;
		byte AltCollisionRight_Floor;
		byte CollisionRight_LWall;
		byte AltCollisionRight_LWall;
		byte CollisionRight_Ceiling;
		byte AltCollisionRight_Ceiling;
		byte CollisionRight_RWall;
		byte AltCollisionRight_RWall;
		byte CollisionBottom_Floor;
		byte AltCollisionBottom_Floor;
		byte CollisionBottom_LWall;
		byte AltCollisionBottom_LWall;
		byte CollisionBottom_Ceiling;
		byte AltCollisionBottom_Ceiling;
		byte CollisionBottom_RWall;
		byte AltCollisionBottom_RWall;
	};

}

#endif // !COLLISION_H
