#ifndef COLLISION_H
#define COLLISION_H

#define COLSTORE_COUNT (2)

enum CollisionSides {
    CSIDE_FLOOR  = 0,
    CSIDE_LWALL  = 1,
    CSIDE_RWALL  = 2,
    CSIDE_ROOF   = 3,
    CSIDE_ENTITY = 4,
};

enum CollisionModes {
    CMODE_FLOOR = 0,
    CMODE_LWALL = 1,
    CMODE_ROOF  = 2,
    CMODE_RWALL = 3,
};

enum CollisionSolidity {
    SOLID_ALL  = 0,
    SOLID_TOP  = 1,
    SOLID_LRB  = 2,
    SOLID_NONE = 3,
};

enum ObjectCollisionTypes {
    C_TOUCH    = 0,
    C_BOX      = 1,
    C_BOX2     = 2,
    C_PLATFORM = 3,
    C_BOX3     = 4, // Introduced in Origins Plus
    C_ENEMY    = 5, // Introduced in Origins Plus
};

struct CollisionSensor {
    int XPos;
    int YPos;
    int angle;
    bool collided;
};

// Introduced in Origins Plus
struct CollisionStore {
    int entityNo;
    sbyte type;
    int left;
    int right;
    int top;
    int bottom;
};
extern CollisionStore collisionStorage[2];

enum EntityCollisionEffects {
    ECEFFECT_NONE         = 0,
    ECEFFECT_RESETSTORAGE = 1,
    ECEFFECT_BOXCOL3      = 2,
};

#if !RETRO_USE_ORIGINAL_CODE
#define DEBUG_HITBOX_COUNT (0x400)

struct DebugHitboxInfo {
    byte type;
    byte collision;
    short left;
    short top;
    short right;
    short bottom;
    int XPos;
    int YPos;
    Entity *entity;
};


enum DebugHitboxTypes { H_TYPE_TOUCH, H_TYPE_BOX, H_TYPE_PLAT, H_TYPE_FINGER, H_TYPE_HAMMER};

extern byte showHitboxes;
extern int debugHitboxCount;
extern DebugHitboxInfo debugHitboxList[DEBUG_HITBOX_COUNT];

int AddDebugHitbox(byte type, Entity *entity, int left, int top, int right, int bottom);
#endif

extern int collisionLeft;
extern int collisionTop;
extern int collisionRight;
extern int collisionBottom;

extern CollisionSensor sensors[6];

void FindFloorPosition(Player *player, CollisionSensor *sensor, int startYPos);
void FindLWallPosition(Player *player, CollisionSensor *sensor, int startXPos);
void FindRoofPosition(Player *player, CollisionSensor *sensor, int startYPos);
void FindRWallPosition(Player *player, CollisionSensor *sensor, int startXPos);
void FloorCollision(Player *player, CollisionSensor *sensor);
void LWallCollision(Player *player, CollisionSensor *sensor);
void RoofCollision(Player *player, CollisionSensor *sensor);
void RWallCollision(Player *player, CollisionSensor *sensor);
void SetPathGripSensors(Player *player);

void ProcessPathGrip(Player *player);
void ProcessAirCollision(Player *player);

void ProcessPlayerTileCollisions(Player *player);

void TouchCollision(int left, int top, int right, int bottom);
void BoxCollision(int left, int top, int right, int bottom);  // Standard
void BoxCollision2(int left, int top, int right, int bottom); // Updated (?)
void PlatformCollision(int left, int top, int right, int bottom);
void BoxCollision3(int left, int top, int right, int bottom); // Added in Origins Plus
void EnemyCollision(int left, int top, int right, int bottom); // Added in Origins Plus

void ObjectFloorCollision(int xOffset, int yOffset, int cPath);
void ObjectLWallCollision(int xOffset, int yOffset, int cPath);
void ObjectRoofCollision(int xOffset, int yOffset, int cPath);
void ObjectRWallCollision(int xOffset, int yOffset, int cPath);

void ObjectFloorGrip(int xOffset, int yOffset, int cPath);
void ObjectLWallGrip(int xOffset, int yOffset, int cPath);
void ObjectRoofGrip(int xOffset, int yOffset, int cPath);
void ObjectRWallGrip(int xOffset, int yOffset, int cPath);
void ObjectEntityGrip(int direction, int extendBottomCol, int effect); // Added in Origins Plus

#endif // !COLLISION_H
