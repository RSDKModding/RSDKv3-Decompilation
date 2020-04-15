#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_COUNT (2)

namespace Player {
	class Player {
	public:
		int EntityNo;
		int XPos;
		int YPos;
		int XVelocity;
		int YVelocity;
		int Speed;
		int ScreenXPos;
		int ScreenYPos;
		int Angle;
		int Timer;
		int LookPos;
		int Values[8];
		byte CollisionMode;
		byte Skidding;
		byte Pushing;
		byte CollisionPlane;
		byte ControlMode;
		byte ControlLock;
		byte field_52;
		byte field_53;
		int TopSpeed;
		int Acceleration;
		int Deceleration;
		int AirAcceleration;
		int AirDeceleration;
		int GravityStrength;
		int JumpStrength;
		int JumpCap;
		int RollingAcceleration;
		int RollingDeceleration;
		byte Visible;
		byte TileCollisions;
		byte ObjectInteractions;
		byte Left;
		byte Right;
		byte Up;
		byte Down;
		byte JumpPress;
		byte JumpHold;
		byte FollowPlayer1;
		byte TrackScroll;
		byte Gravity;
		byte Water;
		byte Flailing[3];
		void* CollisionData;
		Object::Entity ObjectData;
	};

	static Player Players[PLAYER_COUNT];
	static int PlayerListPos = 0;

	void SetPlayerScreenPosition(Player* Player);
	void SetPlayerScreenPositionCDStyle(Player* Player);
	void SetPlayerHLockedScreenPosition(Player* Player);
	void SetPlayerLockedScreenPosition(Player* Player);
}

#endif // !PLAYER_H
