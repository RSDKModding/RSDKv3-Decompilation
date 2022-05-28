#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_COUNT (2)

enum PlayerControlModes {
    CONTROLMODE_NONE     = -1,
    CONTROLMODE_NORMAL   = 0,
    CONTROLMODE_SIDEKICK = 1,
};

struct Player {
    int entityNo;
    int XPos;
    int YPos;
    int XVelocity;
    int YVelocity;
    int speed;
    int screenXPos;
    int screenYPos;
    int angle;
    int timer;
    int lookPos;
    int values[8];
    byte collisionMode;
    byte skidding;
    byte pushing;
    byte collisionPlane;
    sbyte controlMode;
    byte controlLock;
    int topSpeed;
    int acceleration;
    int deceleration;
    int airAcceleration;
    int airDeceleration;
    int gravityStrength;
    int jumpStrength;
    int jumpCap;
    int rollingAcceleration;
    int rollingDeceleration;
    byte visible;
    byte tileCollisions;
    byte objectInteractions;
    byte left;
    byte right;
    byte up;
    byte down;
    byte jumpPress;
    byte jumpHold;
    byte followPlayer1;
    byte trackScroll;
    byte gravity;
    byte water;
    byte flailing[3];
    AnimationFile *animationFile;
    Entity *boundEntity;
};

extern Player playerList[PLAYER_COUNT];
extern int playerListPos;
extern int activePlayer;
extern int activePlayerCount;

extern ushort upBuffer;
extern ushort downBuffer;
extern ushort leftBuffer;
extern ushort rightBuffer;
extern ushort jumpPressBuffer;
extern ushort jumpHoldBuffer;

void ProcessPlayerControl(Player *player);

#endif // !PLAYER_H
