#ifndef OBJECT_H
#define OBJECT_H

#define ENTITY_COUNT     (0x4A0)
#define TEMPENTITY_START (ENTITY_COUNT - 0x80)
#define OBJECT_COUNT     (0x100)

struct Entity {
    int XPos;
    int YPos;
    int values[8];
    int scale;
    int rotation;
    int animationTimer;
    int animationSpeed;
    byte type;
    byte propertyValue;
    byte state;
    byte priority;
    byte drawOrder;
    byte direction;
    byte inkEffect;
    byte alpha;
    byte animation;
    byte prevAnimation;
    byte frame;
};

enum ObjectTypes {
    OBJ_TYPE_BLANKOBJECT = 0 // 0 is always blank obj
};

enum ObjectPriority {
    PRIORITY_ACTIVE_BOUNDS,
    PRIORITY_ACTIVE,
    PRIORITY_ACTIVE_PAUSED,
    PRIORITY_ACTIVE_XBOUNDS,
    PRIORITY_ACTIVE_BOUNDS_REMOVE,
    PRIORITY_INACTIVE,
};

extern int objectLoop;
extern int curObjectType;
extern Entity objectEntityList[ENTITY_COUNT];

extern char typeNames[OBJECT_COUNT][0x40];

extern int OBJECT_BORDER_X1;
extern int OBJECT_BORDER_X2;
extern const int OBJECT_BORDER_Y1;
extern const int OBJECT_BORDER_Y2;

void ProcessStartupObjects();
void ProcessObjects();
void ProcessPausedObjects();

void SetObjectTypeName(const char *objectName, int objectID);

#endif // !OBJECT_H
