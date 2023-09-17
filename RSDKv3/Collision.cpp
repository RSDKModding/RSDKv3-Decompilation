#include "RetroEngine.hpp"
#include <stdlib.h>

int collisionLeft   = 0;
int collisionTop    = 0;
int collisionRight  = 0;
int collisionBottom = 0;

CollisionSensor sensors[6];

CollisionStore collisionStorage[2];

const int hammerJumpHitbox[] = {
    -25, -25,  25,  25,
    -25, -25,  25,  25,
    -25, -25,  25,  25,
    -25, -25,  25,  25
};

const int hammerDashHitbox[] = { 
     -10, -17, 23, 17,
     -23, -17, 10, 17,
     -18, -24, 10, 17,
     -10, -26, 25, 17,
     -10, -17, 23, 17,
     -23, -17, 10, 17,
     -18, -24, 10, 17,
     -10, -26, 25, 17
};

const int chibiHammerJumpHitbox[] = {
    -15, -11, 15, 20,
    -15, -11, 15, 20,
};

const int chibiHammerDashHitbox[] = {
    -14, -12, 8, 12,
    -10, -12, 8, 12,
     -8, -12,16, 12
};

#if !RETRO_USE_ORIGINAL_CODE
byte showHitboxes = 0;

int debugHitboxCount = 0;
DebugHitboxInfo debugHitboxList[DEBUG_HITBOX_COUNT];

int AddDebugHitbox(byte type, Entity *entity, int left, int top, int right, int bottom)
{
    int XPos = 0, YPos = 0;
    if (entity) {
        XPos = entity->XPos;
        YPos = entity->YPos;
    }
    else if (type != H_TYPE_FINGER) {
        Player *player = &playerList[activePlayer];
        XPos           = player->XPos;
        YPos           = player->YPos;
    }

    int i = 0;
    for (; i < debugHitboxCount; ++i) {
        if (debugHitboxList[i].left == left && debugHitboxList[i].top == top && debugHitboxList[i].right == right
            && debugHitboxList[i].bottom == bottom && debugHitboxList[i].XPos == XPos && debugHitboxList[i].YPos == YPos
            && debugHitboxList[i].entity == entity) {
            return i;
        }
    }

    if (i < DEBUG_HITBOX_COUNT) {
        debugHitboxList[i].type      = type;
        debugHitboxList[i].entity    = entity;
        debugHitboxList[i].collision = 0;
        debugHitboxList[i].left      = left;
        debugHitboxList[i].top       = top;
        debugHitboxList[i].right     = right;
        debugHitboxList[i].bottom    = bottom;
        debugHitboxList[i].XPos      = XPos;
        debugHitboxList[i].YPos      = YPos;

        int id = debugHitboxCount;
        debugHitboxCount++;
        return id;
    }

    return -1;
}
#endif

inline Hitbox *getPlayerHitbox(Player *player)
{
    AnimationFile *animFile = player->animationFile;
    return &hitboxList
        [animFrames[animationList[animFile->aniListOffset + player->boundEntity->animation].frameListOffset + player->boundEntity->frame].hitboxID
         + animFile->hitboxListOffset];
}
void FindFloorPosition(Player *player, CollisionSensor *sensor, int startY)
{
    int c     = 0;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) + i - TILE_SIZE;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_LRB
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF;
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & 15) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            byte cAngle      = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24;
                            sensor->angle    = (byte)(-0x80 - cAngle);
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            byte cAngle      = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24;
                            sensor->angle    = 0x100 - (byte)(-0x80 - cAngle);
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle > 0xFF)
                        sensor->angle -= 0x100;

                    if ((abs(sensor->angle - angle) > 0x20) && (abs(sensor->angle - 0x100 - angle) > 0x20)
                        && (abs(sensor->angle + 0x100 - angle) > 0x20)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                    else if (sensor->YPos - startY > (TILE_SIZE - 2)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->YPos - startY < -(TILE_SIZE - 2)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void FindLWallPosition(Player *player, CollisionSensor *sensor, int startX)
{
    int c     = 0;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) + i - TILE_SIZE;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile      = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile          = tile + tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8);
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16);
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            int cAngle       = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8;
                            sensor->angle    = (byte)(-0x80 - cAngle);
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            int cAngle       = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16;
                            sensor->angle    = 0x100 - (byte)(-0x80 - cAngle);
                            break;
                        }
                    }
                }
                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle > 0xFF)
                        sensor->angle -= 0x100;

                    if (abs(angle - sensor->angle) > 0x20) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                    else if (sensor->XPos - startX > TILE_SIZE - 2) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->XPos - startX < -(TILE_SIZE - 2)) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void FindRoofPosition(Player *player, CollisionSensor *sensor, int startY)
{
    int c     = 0;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) + TILE_SIZE - i;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile      = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile          = tile + tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24;
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].roofMasks[c] <= -0x40)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            byte cAngle      = collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF;
                            sensor->angle    = (byte)(-0x80 - cAngle);
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].floorMasks[c] >= 0x40)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            byte cAngle      = collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF;
                            sensor->angle    = 0x100 - (byte)(-0x80 - cAngle);
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle > 0xFF)
                        sensor->angle -= 0x100;

                    if (abs(sensor->angle - angle) <= 0x20) {
                        if (sensor->YPos - startY > tsm1) {
                            sensor->YPos     = startY << 16;
                            sensor->collided = false;
                        }
                        if (sensor->YPos - startY < -tsm1) {
                            sensor->YPos     = startY << 16;
                            sensor->collided = false;
                        }
                    }
                    else {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                }
            }
        }
    }
}
void FindRWallPosition(Player *player, CollisionSensor *sensor, int startX)
{
    int c;
    int angle = sensor->angle;
    int tsm1  = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) + TILE_SIZE - i;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile      = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile          = tile + tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16;
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8);
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].rWallMasks[c] <= -0x40)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            int cAngle       = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF0000) >> 16;
                            sensor->angle    = (byte)(-0x80 - cAngle);
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if (collisionMasks[player->collisionPlane].lWallMasks[c] >= 0x40)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            int cAngle       = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF00) >> 8;
                            sensor->angle    = 0x100 - (byte)(-0x80 - cAngle);
                            break;
                        }
                    }
                }
                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle > 0xFF)
                        sensor->angle -= 0x100;

                    if (abs(sensor->angle - angle) > 0x20) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                        sensor->angle    = angle;
                        i                = TILE_SIZE * 3;
                    }
                    else if (sensor->XPos - startX > (TILE_SIZE - 2)) {
                        sensor->XPos     = startX >> 16;
                        sensor->collided = false;
                    }
                    else if (sensor->XPos - startX < -(TILE_SIZE - 2)) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void FloorCollision(Player *player, CollisionSensor *sensor)
{
    int c;
    int startY = sensor->YPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) + i - TILE_SIZE;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_LRB
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= collisionMasks[player->collisionPlane].floorMasks[c] + i - TILE_SIZE
                                || collisionMasks[player->collisionPlane].floorMasks[c] >= tsm1)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF;
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= collisionMasks[player->collisionPlane].floorMasks[c] + i - TILE_SIZE
                                || collisionMasks[player->collisionPlane].floorMasks[c] >= tsm1)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + i - TILE_SIZE)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            int cAngle       = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24;
                            sensor->angle    = (byte)(-0x80 - cAngle);
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + i - TILE_SIZE)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            int cAngle       = (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24;
                            sensor->angle    = 0x100 - (byte)(-0x80 - cAngle);
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle > 0xFF)
                        sensor->angle -= 0x100;

                    if (sensor->YPos - startY > (TILE_SIZE - 2)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->YPos - startY < -(TILE_SIZE + 1)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void LWallCollision(Player *player, CollisionSensor *sensor)
{
    int c;
    int startX = sensor->XPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) + i - TILE_SIZE;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_TOP
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= collisionMasks[player->collisionPlane].lWallMasks[c] + i - TILE_SIZE)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + i - TILE_SIZE)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= collisionMasks[player->collisionPlane].lWallMasks[c] + i - TILE_SIZE)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) <= tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + i - TILE_SIZE)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->XPos - startX > tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->XPos - startX < -tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void RoofCollision(Player *player, CollisionSensor *sensor)
{
    int c;
    int startY = sensor->YPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = sensor->XPos >> 16;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = (sensor->YPos >> 16) + TILE_SIZE - i;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_TOP
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= collisionMasks[player->collisionPlane].roofMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24);
                            break;
                        }
                        case FLIP_X: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= collisionMasks[player->collisionPlane].roofMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = collisionMasks[player->collisionPlane].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - ((collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF000000) >> 24);
                            break;
                        }
                        case FLIP_Y: {
                            c = (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = (byte)(-0x80 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF));
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (XPos & tsm1) + (tileIndex << 4);
                            if ((YPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->YPos     = tsm1 - collisionMasks[player->collisionPlane].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                            sensor->collided = true;
                            sensor->angle    = 0x100 - (byte)(-0x80 - (collisionMasks[player->collisionPlane].angles[tileIndex] & 0xFF));
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->angle < 0)
                        sensor->angle += 0x100;

                    if (sensor->angle > 0xFF)
                        sensor->angle -= 0x100;

                    if (sensor->YPos - startY > (TILE_SIZE - 2)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->YPos - startY < -(TILE_SIZE - 2)) {
                        sensor->YPos     = startY << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void RWallCollision(Player *player, CollisionSensor *sensor)
{
    int c;
    int startX = sensor->XPos >> 16;
    int tsm1   = (TILE_SIZE - 1);
    for (int i = 0; i < TILE_SIZE * 3; i += TILE_SIZE) {
        if (!sensor->collided) {
            int XPos   = (sensor->XPos >> 16) + TILE_SIZE - i;
            int chunkX = XPos >> 7;
            int tileX  = (XPos & 0x7F) >> 4;
            int YPos   = sensor->YPos >> 16;
            int chunkY = YPos >> 7;
            int tileY  = (YPos & 0x7F) >> 4;
            if (XPos > -1 && YPos > -1) {
                int tile = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
                tile += tileX + (tileY << 3);
                int tileIndex = tiles128x128.tileIndex[tile];
                if (tiles128x128.collisionFlags[player->collisionPlane][tile] != SOLID_TOP
                    && tiles128x128.collisionFlags[player->collisionPlane][tile] < SOLID_NONE) {
                    switch (tiles128x128.direction[tile]) {
                        case FLIP_NONE: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= collisionMasks[player->collisionPlane].rWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_X: {
                            c = (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_Y: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= collisionMasks[player->collisionPlane].rWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = collisionMasks[player->collisionPlane].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                        case FLIP_XY: {
                            c = tsm1 - (YPos & tsm1) + (tileIndex << 4);
                            if ((XPos & tsm1) >= tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + TILE_SIZE - i)
                                break;

                            sensor->XPos     = tsm1 - collisionMasks[player->collisionPlane].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                            sensor->collided = true;
                            break;
                        }
                    }
                }

                if (sensor->collided) {
                    if (sensor->XPos - startX > tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                    else if (sensor->XPos - startX < -tsm1) {
                        sensor->XPos     = startX << 16;
                        sensor->collided = false;
                    }
                }
            }
        }
    }
}
void ProcessAirCollision(Player *player)
{
    Hitbox *playerHitbox = getPlayerHitbox(player);
    collisionLeft        = playerHitbox->left[0];
    collisionTop         = playerHitbox->top[0];
    collisionRight       = playerHitbox->right[0];
    collisionBottom      = playerHitbox->bottom[0];

    byte movingDown  = 0;
    byte movingUp    = 1;
    byte movingLeft  = 0;
    byte movingRight = 0;

    if (player->XVelocity < 0) {
        movingRight = 0;
    }
    else {
        movingRight         = 1;
        sensors[0].YPos     = player->YPos + 0x20000;
        sensors[0].collided = false;
        sensors[0].XPos     = player->XPos + (collisionRight << 16);
    }
    if (player->XVelocity > 0) {
        movingLeft = 0;
    }
    else {
        movingLeft          = 1;
        sensors[1].YPos     = player->YPos + 0x20000;
        sensors[1].collided = false;
        sensors[1].XPos     = player->XPos + ((collisionLeft - 1) << 16);
    }
    sensors[2].XPos     = player->XPos + (playerHitbox->left[1] << 16);
    sensors[3].XPos     = player->XPos + (playerHitbox->right[1] << 16);
    sensors[2].collided = false;
    sensors[3].collided = false;
    sensors[4].XPos     = sensors[2].XPos;
    sensors[5].XPos     = sensors[3].XPos;
    sensors[4].collided = false;
    sensors[5].collided = false;
    if (player->YVelocity < 0) {
        movingDown = 0;
    }
    else {
        movingDown      = 1;
        sensors[2].YPos = player->YPos + (collisionBottom << 16);
        sensors[3].YPos = player->YPos + (collisionBottom << 16);
    }
    sensors[4].YPos = player->YPos + ((collisionTop - 1) << 16);
    sensors[5].YPos = player->YPos + ((collisionTop - 1) << 16);
    int cnt         = (abs(player->XVelocity) <= abs(player->YVelocity) ? (abs(player->YVelocity) >> 19) + 1 : (abs(player->XVelocity) >> 19) + 1);
    int XVel        = player->XVelocity / cnt;
    int YVel        = player->YVelocity / cnt;
    int XVel2       = player->XVelocity - XVel * (cnt - 1);
    int YVel2       = player->YVelocity - YVel * (cnt - 1);
    while (cnt > 0) {
        if (cnt < 2) {
            XVel = XVel2;
            YVel = YVel2;
        }
        cnt--;

        if (movingRight == 1) {
            sensors[0].XPos += XVel + 0x10000;
            sensors[0].YPos += YVel;
            LWallCollision(player, &sensors[0]);
            if (sensors[0].collided)
                movingRight = 2;
        }

        if (movingLeft == 1) {
            sensors[1].XPos += XVel - 0x10000;
            sensors[1].YPos += YVel;
            RWallCollision(player, &sensors[1]);
            if (sensors[1].collided)
                movingLeft = 2;
        }

        if (movingRight == 2) {
            player->XVelocity = 0;
            player->speed     = 0;
            player->XPos      = (sensors[0].XPos - collisionRight) << 16;
            sensors[2].XPos   = player->XPos + ((collisionLeft + 1) << 16);
            sensors[3].XPos   = player->XPos + ((collisionRight - 2) << 16);
            sensors[4].XPos   = sensors[2].XPos;
            sensors[5].XPos   = sensors[3].XPos;
            XVel              = 0;
            XVel2             = 0;
            movingRight       = 3;
        }

        if (movingLeft == 2) {
            player->XVelocity = 0;
            player->speed     = 0;
            player->XPos      = (sensors[1].XPos - collisionLeft + 1) << 16;
            sensors[2].XPos   = player->XPos + ((collisionLeft + 1) << 16);
            sensors[3].XPos   = player->XPos + ((collisionRight - 2) << 16);
            sensors[4].XPos   = sensors[2].XPos;
            sensors[5].XPos   = sensors[3].XPos;
            XVel              = 0;
            XVel2             = 0;
            movingLeft        = 3;
        }

        if (movingDown == 1) {
            for (int i = 2; i < 4; i++) {
                if (!sensors[i].collided) {
                    sensors[i].XPos += XVel;
                    sensors[i].YPos += YVel;
                    FloorCollision(player, &sensors[i]);
                }
            }
            if (sensors[2].collided || sensors[3].collided) {
                movingDown = 2;
                cnt        = 0;
            }
        }

        if (movingUp == 1) {
            for (int i = 4; i < 6; i++) {
                if (!sensors[i].collided) {
                    sensors[i].XPos += XVel;
                    sensors[i].YPos += YVel;
                    RoofCollision(player, &sensors[i]);
                }
            }
            if (sensors[4].collided || sensors[5].collided) {
                movingUp = 2;
                cnt      = 0;
            }
        }
    }

    if (movingRight < 2 && movingLeft < 2)
        player->XPos = player->XPos + player->XVelocity;

    if (movingUp < 2 && movingDown < 2) {
        player->YPos = player->YPos + player->YVelocity;
        return;
    }

    if (movingDown == 2) {
        player->gravity = 0;
        if (sensors[2].collided && sensors[3].collided) {
            if (sensors[2].YPos >= sensors[3].YPos) {
                player->YPos  = (sensors[3].YPos - collisionBottom) << 16;
                player->angle = sensors[3].angle;
            }
            else {
                player->YPos  = (sensors[2].YPos - collisionBottom) << 16;
                player->angle = sensors[2].angle;
            }
        }
        else if (sensors[2].collided == 1) {
            player->YPos  = (sensors[2].YPos - collisionBottom) << 16;
            player->angle = sensors[2].angle;
        }
        else if (sensors[3].collided == 1) {
            player->YPos  = (sensors[3].YPos - collisionBottom) << 16;
            player->angle = sensors[3].angle;
        }
        if (player->angle > 0xA0 && player->angle < 0xE0 && player->collisionMode != CMODE_LWALL) {
            player->collisionMode = CMODE_LWALL;
            player->XPos          = player->XPos - 0x40000;
        }
        if (player->angle > 0x20 && player->angle < 0x60 && player->collisionMode != CMODE_RWALL) {
            player->collisionMode = CMODE_RWALL;
            player->XPos          = player->XPos + 0x40000;
        }
        if (player->angle < 0x20 || player->angle > 0xE0) {
            player->controlLock = 0;
        }
        player->boundEntity->rotation = player->angle << 1;

        int speed = 0;
        if (player->down) {
            if (player->angle < 128) {
                if (player->angle < 16) {
                    speed = player->XVelocity;
                }
                else if (player->angle >= 32) {
                    speed = (abs(player->XVelocity) <= abs(player->YVelocity) ? player->YVelocity + player->YVelocity / 12 : player->XVelocity);
                }
                else {
                    speed = (abs(player->XVelocity) <= abs(player->YVelocity >> 1) ? (player->YVelocity + player->YVelocity / 12) >> 1
                                                                                   : player->XVelocity);
                }
            }
            else if (player->angle > 240) {
                speed = player->XVelocity;
            }
            else if (player->angle <= 224) {
                speed = (abs(player->XVelocity) <= abs(player->YVelocity) ? -(player->YVelocity + player->YVelocity / 12) : player->XVelocity);
            }
            else {
                speed = (abs(player->XVelocity) <= abs(player->YVelocity >> 1) ? -((player->YVelocity + player->YVelocity / 12) >> 1)
                                                                               : player->XVelocity);
            }
        }
        else if (player->angle < 0x80) {
            if (player->angle < 0x10) {
                speed = player->XVelocity;
            }
            else if (player->angle >= 0x20) {
                speed = (abs(player->XVelocity) <= abs(player->YVelocity) ? player->YVelocity : player->XVelocity);
            }
            else {
                speed = (abs(player->XVelocity) <= abs(player->YVelocity >> 1) ? player->YVelocity >> 1 : player->XVelocity);
            }
        }
        else if (player->angle > 0xF0) {
            speed = player->XVelocity;
        }
        else if (player->angle <= 0xE0) {
            speed = (abs(player->XVelocity) <= abs(player->YVelocity) ? -player->YVelocity : player->XVelocity);
        }
        else {
            speed = (abs(player->XVelocity) <= abs(player->YVelocity >> 1) ? -(player->YVelocity >> 1) : player->XVelocity);
        }

        if (speed < -0x180000)
            speed = -0x180000;
        if (speed > 0x180000)
            speed = 0x180000;
        player->speed         = speed;
        player->YVelocity     = 0;
        scriptEng.checkResult = 1;
    }

    if (movingUp == 2) {
        int sensorAngle = 0;
        if (sensors[4].collided && sensors[5].collided) {
            if (sensors[4].YPos <= sensors[5].YPos) {
                player->YPos = (sensors[5].YPos - collisionTop + 1) << 16;
                sensorAngle  = sensors[5].angle;
            }
            else {
                player->YPos = (sensors[4].YPos - collisionTop + 1) << 16;
                sensorAngle  = sensors[4].angle;
            }
        }
        else if (sensors[4].collided) {
            player->YPos = (sensors[4].YPos - collisionTop + 1) << 16;
            sensorAngle  = sensors[4].angle;
        }
        else if (sensors[5].collided) {
            player->YPos = (sensors[5].YPos - collisionTop + 1) << 16;
            sensorAngle  = sensors[5].angle;
        }
        sensorAngle &= 0xFF;

        int angle = ArcTanLookup(player->XVelocity, player->YVelocity);
        if (sensorAngle > 0x40 && sensorAngle < 0x62 && angle > 0xA0 && angle < 0xC2) {
            player->gravity               = 0;
            player->angle                 = sensorAngle;
            player->boundEntity->rotation = player->angle << 1;
            player->collisionMode         = CMODE_RWALL;
            player->XPos                  = player->XPos + 0x40000;
            player->YPos                  = player->YPos - 0x20000;
            if (player->angle <= 0x60)
                player->speed = player->YVelocity;
            else
                player->speed = player->YVelocity >> 1;
        }
        if (sensorAngle > 0x9E && sensorAngle < 0xC0 && angle > 0xBE && angle < 0xE0) {
            player->gravity               = 0;
            player->angle                 = sensorAngle;
            player->boundEntity->rotation = player->angle << 1;
            player->collisionMode         = CMODE_LWALL;
            player->XPos                  = player->XPos - 0x40000;
            player->YPos                  = player->YPos - 0x20000;
            if (player->angle >= 0xA0)
                player->speed = -player->YVelocity;
            else
                player->speed = -player->YVelocity >> 1;
        }
        if (player->YVelocity < 0)
            player->YVelocity = 0;
        scriptEng.checkResult = 2;
    }
}
void ProcessPathGrip(Player *player)
{
    int cosValue256;
    int sinValue256;
    sensors[4].XPos = player->XPos;
    sensors[4].YPos = player->YPos;
    for (int i = 0; i < 6; ++i) {
        sensors[i].angle    = player->angle;
        sensors[i].collided = false;
    }
    SetPathGripSensors(player);
    int absSpeed  = abs(player->speed);
    int checkDist = absSpeed >> 18;
    absSpeed &= 0x3FFFF;
    byte cMode = player->collisionMode;

    while (checkDist > -1) {
        if (checkDist >= 1) {
            cosValue256 = cos256LookupTable[player->angle] << 10;
            sinValue256 = sin256LookupTable[player->angle] << 10;
            checkDist--;
        }
        else {
            cosValue256 = absSpeed * cos256LookupTable[player->angle] >> 8;
            sinValue256 = absSpeed * sin256LookupTable[player->angle] >> 8;
            checkDist   = -1;
        }

        if (player->speed < 0) {
            cosValue256 = -cosValue256;
            sinValue256 = -sinValue256;
        }

        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[2].collided = false;
        sensors[4].XPos += cosValue256;
        sensors[4].YPos += sinValue256;
        int tileDistance = -1;
        switch (player->collisionMode) {
            case CMODE_FLOOR: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (player->speed > 0)
                    LWallCollision(player, &sensors[3]);

                if (player->speed < 0)
                    RWallCollision(player, &sensors[3]);

                if (sensors[3].collided) {
                    cosValue256 = 0;
                    checkDist   = -1;
                }

                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindFloorPosition(player, &sensors[i], sensors[i].YPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].collided) {
                            if (sensors[i].YPos < sensors[tileDistance].YPos)
                                tileDistance = i;

                            if (sensors[i].YPos == sensors[tileDistance].YPos && (sensors[i].angle < 0x08 || sensors[i].angle > 0xF8))
                                tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided)
                        tileDistance = i;
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].YPos  = sensors[tileDistance].YPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].YPos  = sensors[0].YPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].YPos  = sensors[0].YPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[3].YPos  = sensors[0].YPos - 0x40000;
                    sensors[3].angle = sensors[0].angle;
                    sensors[4].XPos  = sensors[1].XPos;
                    sensors[4].YPos  = sensors[0].YPos - (collisionBottom << 16);
                }

                if (sensors[0].angle < 0xDE && sensors[0].angle > 0x80)
                    player->collisionMode = CMODE_LWALL;
                if (sensors[0].angle > 0x22 && sensors[0].angle < 0x80)
                    player->collisionMode = CMODE_RWALL;
                break;
            }
            case CMODE_LWALL: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (player->speed > 0)
                    RoofCollision(player, &sensors[3]);

                if (player->speed < 0)
                    FloorCollision(player, &sensors[3]);

                if (sensors[3].collided) {
                    sinValue256 = 0;
                    checkDist   = -1;
                }
                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindLWallPosition(player, &sensors[i], sensors[i].XPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].XPos < sensors[tileDistance].XPos && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].XPos  = sensors[tileDistance].XPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].XPos  = sensors[0].XPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].XPos  = sensors[0].XPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[4].YPos  = sensors[1].YPos;
                    sensors[4].XPos  = sensors[1].XPos - (collisionRight << 16);
                }

                if (sensors[0].angle > 0xE2)
                    player->collisionMode = CMODE_FLOOR;
                if (sensors[0].angle < 0x9E)
                    player->collisionMode = CMODE_ROOF;
                break;
                break;
            }
            case CMODE_ROOF: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (player->speed > 0)
                    RWallCollision(player, &sensors[3]);

                if (player->speed < 0)
                    LWallCollision(player, &sensors[3]);

                if (sensors[3].collided) {
                    cosValue256 = 0;
                    checkDist   = -1;
                }
                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindRoofPosition(player, &sensors[i], sensors[i].YPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].YPos > sensors[tileDistance].YPos && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].YPos  = sensors[tileDistance].YPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].YPos  = sensors[0].YPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].YPos  = sensors[0].YPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[3].YPos  = sensors[0].YPos + 0x40000;
                    sensors[3].angle = sensors[0].angle;
                    sensors[4].XPos  = sensors[1].XPos;
                    sensors[4].YPos  = sensors[0].YPos - ((collisionTop - 1) << 16);
                }

                if (sensors[0].angle > 0xA2)
                    player->collisionMode = CMODE_LWALL;
                if (sensors[0].angle < 0x5E)
                    player->collisionMode = CMODE_RWALL;
                break;
            }
            case CMODE_RWALL: {
                sensors[3].XPos += cosValue256;
                sensors[3].YPos += sinValue256;

                if (player->speed > 0)
                    FloorCollision(player, &sensors[3]);

                if (player->speed < 0)
                    RoofCollision(player, &sensors[3]);

                if (sensors[3].collided) {
                    sinValue256 = 0;
                    checkDist   = -1;
                }
                for (int i = 0; i < 3; i++) {
                    sensors[i].XPos += cosValue256;
                    sensors[i].YPos += sinValue256;
                    FindRWallPosition(player, &sensors[i], sensors[i].XPos >> 16);
                }

                tileDistance = -1;
                for (int i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].XPos > sensors[tileDistance].XPos && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].XPos  = sensors[tileDistance].XPos << 16;
                    sensors[0].angle = sensors[tileDistance].angle;
                    sensors[1].XPos  = sensors[0].XPos;
                    sensors[1].angle = sensors[0].angle;
                    sensors[2].XPos  = sensors[0].XPos;
                    sensors[2].angle = sensors[0].angle;
                    sensors[4].YPos  = sensors[1].YPos;
                    sensors[4].XPos  = sensors[1].XPos - ((collisionLeft - 1) << 16);
                }

                if (sensors[0].angle < 0x1E)
                    player->collisionMode = CMODE_FLOOR;
                if (sensors[0].angle > 0x62)
                    player->collisionMode = CMODE_ROOF;
                break;
            }
        }
        if (tileDistance > -1)
            player->angle = sensors[0].angle;

        if (!sensors[3].collided)
            SetPathGripSensors(player);
        else
            checkDist = -2;
    }

    switch (cMode) {
        case CMODE_FLOOR: {
            if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
                player->angle                 = sensors[0].angle;
                player->boundEntity->rotation = player->angle << 1;
                player->flailing[0]           = sensors[0].collided;
                player->flailing[1]           = sensors[1].collided;
                player->flailing[2]           = sensors[2].collided;
                if (!sensors[3].collided) {
                    player->pushing = 0;
                    player->XPos    = sensors[4].XPos;
                }
                else {
                    if (player->speed > 0)
                        player->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (player->speed < 0)
                        player->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;

                    player->speed = 0;
                    if ((player->left || player->right) && player->pushing < 2)
                        player->pushing++;
                }
                player->YPos = sensors[4].YPos;
                return;
            }
            player->gravity       = 1;
            player->collisionMode = CMODE_FLOOR;
            player->XVelocity     = cos256LookupTable[player->angle] * player->speed >> 8;
            player->YVelocity     = sin256LookupTable[player->angle] * player->speed >> 8;
            if (player->YVelocity < -0x100000)
                player->YVelocity = -0x100000;

            if (player->YVelocity > 0x100000)
                player->YVelocity = 0x100000;

            player->speed = player->XVelocity;
            player->angle = 0;
            if (!sensors[3].collided) {
                player->pushing = 0;
                player->XPos    = player->XPos + player->XVelocity;
            }
            else {
                if (player->speed > 0)
                    player->XPos = (sensors[3].XPos - collisionRight) << 16;
                if (player->speed < 0)
                    player->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;

                player->speed = 0;
                if ((player->left || player->right) && player->pushing < 2)
                    player->pushing++;
            }
            player->YPos = player->YPos + player->YVelocity;
            return;
        }
        case CMODE_LWALL: {
            if (!sensors[0].collided && !sensors[1].collided && !sensors[2].collided) {
                player->gravity       = 1;
                player->collisionMode = CMODE_FLOOR;
                player->XVelocity     = cos256LookupTable[player->angle] * player->speed >> 8;
                player->YVelocity     = sin256LookupTable[player->angle] * player->speed >> 8;
                if (player->YVelocity < -1048576) {
                    player->YVelocity = -1048576;
                }
                if (player->YVelocity > 0x100000) {
                    player->YVelocity = 0x100000;
                }
                player->speed = player->XVelocity;
                player->angle = 0;
            }
            else if (player->speed >= 0x28000 || player->speed <= -0x28000 || player->controlLock != 0) {
                player->angle                 = sensors[0].angle;
                player->boundEntity->rotation = player->angle << 1;
            }
            else {
                player->gravity       = 1;
                player->angle         = 0;
                player->collisionMode = CMODE_FLOOR;
                player->speed         = player->XVelocity;
                player->controlLock   = 30;
            }
            if (!sensors[3].collided) {
                player->YPos = sensors[4].YPos;
            }
            else {
                if (player->speed > 0)
                    player->YPos = (sensors[3].YPos - collisionTop) << 16;

                if (player->speed < 0)
                    player->YPos = (sensors[3].YPos - collisionBottom) << 16;

                player->speed = 0;
            }
            player->XPos = sensors[4].XPos;
            return;
        }
        case CMODE_ROOF: {
            if (!sensors[0].collided && !sensors[1].collided && !sensors[2].collided) {
                player->gravity       = 1;
                player->collisionMode = CMODE_FLOOR;
                player->XVelocity     = cos256LookupTable[player->angle] * player->speed >> 8;
                player->YVelocity     = sin256LookupTable[player->angle] * player->speed >> 8;
                player->flailing[0]   = 0;
                player->flailing[1]   = 0;
                player->flailing[2]   = 0;
                if (player->YVelocity < -0x100000)
                    player->YVelocity = -0x100000;

                if (player->YVelocity > 0x100000)
                    player->YVelocity = 0x100000;

                player->angle = 0;
                player->speed = player->XVelocity;
                if (!sensors[3].collided) {
                    player->XPos = player->XPos + player->XVelocity;
                }
                else {
                    if (player->speed > 0)
                        player->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (player->speed < 0)
                        player->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;

                    player->speed = 0;
                }
            }
            else if (player->speed <= -0x28000 || player->speed >= 0x28000) {
                player->angle                 = sensors[0].angle;
                player->boundEntity->rotation = player->angle << 1;
                if (!sensors[3].collided) {
                    player->XPos = sensors[4].XPos;
                }
                else {
                    if (player->speed < 0)
                        player->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (player->speed > 0)
                        player->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;
                    player->speed = 0;
                }
            }
            else {
                player->gravity       = 1;
                player->angle         = 0;
                player->collisionMode = CMODE_FLOOR;
                player->speed         = player->XVelocity;
                player->flailing[0]   = 0;
                player->flailing[1]   = 0;
                player->flailing[2]   = 0;
                if (!sensors[3].collided) {
                    player->XPos = player->XPos + player->XVelocity;
                }
                else {
                    if (player->speed > 0)
                        player->XPos = (sensors[3].XPos - collisionRight) << 16;

                    if (player->speed < 0)
                        player->XPos = (sensors[3].XPos - collisionLeft + 1) << 16;
                    player->speed = 0;
                }
            }
            player->YPos = sensors[4].YPos;
            return;
        }
        case CMODE_RWALL: {
            if (!sensors[0].collided && !sensors[1].collided && !sensors[2].collided) {
                player->gravity       = 1;
                player->collisionMode = CMODE_FLOOR;
                player->XVelocity     = cos256LookupTable[player->angle] * player->speed >> 8;
                player->YVelocity     = sin256LookupTable[player->angle] * player->speed >> 8;
                if (player->YVelocity < -0x100000)
                    player->YVelocity = -0x100000;

                if (player->YVelocity > 0x100000)
                    player->YVelocity = 0x100000;

                player->speed = player->XVelocity;
                player->angle = 0;
            }
            else if (player->speed <= -0x28000 || player->speed >= 0x28000 || player->controlLock != 0) {
                player->angle                 = sensors[0].angle;
                player->boundEntity->rotation = player->angle << 1;
            }
            else {
                player->gravity       = 1;
                player->angle         = 0;
                player->collisionMode = CMODE_FLOOR;
                player->speed         = player->XVelocity;
                player->controlLock   = 30;
            }
            if (!sensors[3].collided) {
                player->YPos = sensors[4].YPos;
            }
            else {
                if (player->speed > 0)
                    player->YPos = (sensors[3].YPos - collisionBottom) << 16;

                if (player->speed < 0)
                    player->YPos = (sensors[3].YPos - collisionTop + 1) << 16;

                player->speed = 0;
            }
            player->XPos = sensors[4].XPos;
            return;
        }
        default: return;
    }
}
void SetPathGripSensors(Player *player)
{
    Hitbox *playerHitbox = getPlayerHitbox(player);
    switch (player->collisionMode) {
        case CMODE_FLOOR: {
            collisionLeft   = playerHitbox->left[0];
            collisionTop    = playerHitbox->top[0];
            collisionRight  = playerHitbox->right[0];
            collisionBottom = playerHitbox->bottom[0];
            sensors[0].YPos = sensors[4].YPos + (collisionBottom << 16);
            sensors[1].YPos = sensors[0].YPos;
            sensors[2].YPos = sensors[0].YPos;
            sensors[3].YPos = sensors[4].YPos + 0x40000;
            sensors[0].XPos = sensors[4].XPos + ((playerHitbox->left[1] - 1) << 16);
            sensors[1].XPos = sensors[4].XPos;
            sensors[2].XPos = sensors[4].XPos + (playerHitbox->right[1] << 16);
            if (player->speed > 0) {
                sensors[3].XPos = sensors[4].XPos + ((collisionRight + 1) << 16);
                return;
            }
            sensors[3].XPos = sensors[4].XPos + ((collisionLeft - 1) << 16);
            return;
        }
        case CMODE_LWALL: {
            collisionLeft   = playerHitbox->left[2];
            collisionTop    = playerHitbox->top[2];
            collisionRight  = playerHitbox->right[2];
            collisionBottom = playerHitbox->bottom[2];
            sensors[0].XPos = sensors[4].XPos + (collisionRight << 16);
            sensors[1].XPos = sensors[0].XPos;
            sensors[2].XPos = sensors[0].XPos;
            sensors[3].XPos = sensors[4].XPos + 0x40000;
            sensors[0].YPos = sensors[4].YPos + ((playerHitbox->top[3] - 1) << 16);
            sensors[1].YPos = sensors[4].YPos;
            sensors[2].YPos = sensors[4].YPos + (playerHitbox->bottom[3] << 16);
            if (player->speed > 0) {
                sensors[3].YPos = sensors[4].YPos + (collisionTop << 16);
                return;
            }
            sensors[3].YPos = sensors[4].YPos + ((collisionBottom - 1) << 16);
            return;
        }
        case CMODE_ROOF: {
            collisionLeft   = playerHitbox->left[4];
            collisionTop    = playerHitbox->top[4];
            collisionRight  = playerHitbox->right[4];
            collisionBottom = playerHitbox->bottom[4];
            sensors[0].YPos = sensors[4].YPos + ((collisionTop - 1) << 16);
            sensors[1].YPos = sensors[0].YPos;
            sensors[2].YPos = sensors[0].YPos;
            sensors[3].YPos = sensors[4].YPos - 0x40000;
            sensors[0].XPos = sensors[4].XPos + ((playerHitbox->left[5] - 1) << 16);
            sensors[1].XPos = sensors[4].XPos;
            sensors[2].XPos = sensors[4].XPos + (playerHitbox->right[5] << 16);
            if (player->speed < 0) {
                sensors[3].XPos = sensors[4].XPos + ((collisionRight + 1) << 16);
                return;
            }
            sensors[3].XPos = sensors[4].XPos + ((collisionLeft - 1) << 16);
            return;
        }
        case CMODE_RWALL: {
            collisionLeft   = playerHitbox->left[6];
            collisionTop    = playerHitbox->top[6];
            collisionRight  = playerHitbox->right[6];
            collisionBottom = playerHitbox->bottom[6];
            sensors[0].XPos = sensors[4].XPos + ((collisionLeft - 1) << 16);
            sensors[1].XPos = sensors[0].XPos;
            sensors[2].XPos = sensors[0].XPos;
            sensors[3].XPos = sensors[4].XPos - 0x40000;
            sensors[0].YPos = sensors[4].YPos + ((playerHitbox->top[7] - 1) << 16);
            sensors[1].YPos = sensors[4].YPos;
            sensors[2].YPos = sensors[4].YPos + (playerHitbox->bottom[7] << 16);
            if (player->speed > 0) {
                sensors[3].YPos = sensors[4].YPos + (collisionBottom << 16);
                return;
            }
            sensors[3].YPos = sensors[4].YPos + ((collisionTop - 1) << 16);
            return;
        }
        default: return;
    }
}
void ProcessPlayerTileCollisions(Player *player)
{
    player->flailing[0]   = 0;
    player->flailing[1]   = 0;
    player->flailing[2]   = 0;
    scriptEng.checkResult = false;
    if (player->gravity == 1)
        ProcessAirCollision(player);
    else
        ProcessPathGrip(player);
}

void ObjectFloorCollision(int xOffset, int yOffset, int cPath)
{
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int c                 = 0;
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_LRB && tiles128x128.collisionFlags[cPath][chunk] != SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= 15 - collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) <= 15 - collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->YPos = (YPos - yOffset) << 16;
        }
    }
}
void ObjectLWallCollision(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
        chunk         = chunk + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_TOP && tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= 15 - collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) <= 15 - collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->XPos = (XPos - xOffset) << 16;
        }
    }
}
void ObjectRoofCollision(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
        chunk         = chunk + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_TOP && tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= collisionMasks[cPath].roofMasks[c]) {
                        break;
                    }
                    YPos                  = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= 15 - collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (XPos & 15) + (tileIndex << 4);
                    if ((YPos & 15) >= 15 - collisionMasks[cPath].floorMasks[c]) {
                        break;
                    }
                    YPos                  = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->YPos = (YPos - yOffset) << 16;
        }
    }
}
void ObjectRWallCollision(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7) {
        int chunkX    = XPos >> 7;
        int tileX     = (XPos & 0x7F) >> 4;
        int chunkY    = YPos >> 7;
        int tileY     = (YPos & 0x7F) >> 4;
        int chunk     = stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6;
        chunk         = chunk + tileX + (tileY << 3);
        int tileIndex = tiles128x128.tileIndex[chunk];
        if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_TOP && tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
            switch (tiles128x128.direction[chunk]) {
                case 0: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 1: {
                    c = (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= 15 - collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 2: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= collisionMasks[cPath].rWallMasks[c]) {
                        break;
                    }
                    XPos                  = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
                case 3: {
                    c = 15 - (YPos & 15) + (tileIndex << 4);
                    if ((XPos & 15) >= 15 - collisionMasks[cPath].lWallMasks[c]) {
                        break;
                    }
                    XPos                  = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                    scriptEng.checkResult = true;
                    break;
                }
            }
        }
        if (scriptEng.checkResult) {
            entity->XPos = (XPos - xOffset) << 16;
        }
    }
}
void ObjectFloorGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int chunkX            = YPos;
    YPos                  = YPos - 16;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] != SOLID_LRB && tiles128x128.collisionFlags[cPath][chunk] != SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        YPos += 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->YPos - chunkX) < 16) {
            entity->YPos = (entity->YPos - yOffset) << 16;
            return;
        }
        entity->YPos          = (chunkX - yOffset) << 16;
        scriptEng.checkResult = false;
    }
}
void ObjectLWallGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int startX            = XPos;
    XPos                  = XPos - 16;
    int chunk             = xOffset;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            chunk         = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        XPos += 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->XPos - startX) < 16) {
            entity->XPos = (entity->XPos - xOffset) << 16;
            return;
        }
        entity->XPos          = (startX - xOffset) << 16;
        scriptEng.checkResult = tiles128x128.collisionFlags[cPath][chunk] == 1;
    }
}
void ObjectRoofGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int startY            = YPos;
    YPos                  = YPos + 16;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            int chunk     = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].roofMasks[c] <= -64) {
                            break;
                        }
                        entity->YPos          = collisionMasks[cPath].roofMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (XPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].floorMasks[c] >= 64) {
                            break;
                        }
                        entity->YPos          = 15 - collisionMasks[cPath].floorMasks[c] + (chunkY << 7) + (tileY << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        YPos -= 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->YPos - startY) < 16) {
            entity->YPos = (entity->YPos - yOffset) << 16;
            return;
        }
        entity->YPos          = (startY - yOffset) << 16;
        scriptEng.checkResult = false;
    }
}
void ObjectRWallGrip(int xOffset, int yOffset, int cPath)
{
    int c;
    scriptEng.checkResult = false;
    Entity *entity        = &objectEntityList[objectLoop];
    int XPos              = (entity->XPos >> 16) + xOffset;
    int YPos              = (entity->YPos >> 16) + yOffset;
    int startX            = XPos;
    XPos                  = XPos + 16;
    int chunk             = xOffset;
    for (int i = 3; i > 0; i--) {
        if (XPos > 0 && XPos < stageLayouts[0].xsize << 7 && YPos > 0 && YPos < stageLayouts[0].ysize << 7 && !scriptEng.checkResult) {
            int chunkX    = XPos >> 7;
            int tileX     = (XPos & 0x7F) >> 4;
            int chunkY    = YPos >> 7;
            int tileY     = (YPos & 0x7F) >> 4;
            chunk         = (stageLayouts[0].tiles[chunkX + (chunkY << 8)] << 6) + tileX + (tileY << 3);
            int tileIndex = tiles128x128.tileIndex[chunk];
            if (tiles128x128.collisionFlags[cPath][chunk] < SOLID_NONE) {
                switch (tiles128x128.direction[chunk]) {
                    case 0: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 1: {
                        c = (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 2: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].rWallMasks[c] <= -64) {
                            break;
                        }
                        entity->XPos          = collisionMasks[cPath].rWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                    case 3: {
                        c = 15 - (YPos & 15) + (tileIndex << 4);
                        if (collisionMasks[cPath].lWallMasks[c] >= 64) {
                            break;
                        }
                        entity->XPos          = 15 - collisionMasks[cPath].lWallMasks[c] + (chunkX << 7) + (tileX << 4);
                        scriptEng.checkResult = true;
                        break;
                    }
                }
            }
        }
        XPos -= 16;
    }
    if (scriptEng.checkResult) {
        if (abs(entity->XPos - startX) < 16) {
            entity->XPos = (entity->XPos - xOffset) << 16;
            return;
        }
        entity->XPos          = (startX - xOffset) << 16;
        scriptEng.checkResult = tiles128x128.collisionFlags[cPath][chunk] == 1;
    }
}
void ObjectEntityGrip(int direction, int extendBottomCol, int effect)
{
    Player *player       = &playerList[activePlayer];
    Hitbox *playerHitbox = getPlayerHitbox(player);

    collisionLeft   = playerHitbox->left[0];
    collisionTop    = playerHitbox->top[0];
    collisionRight  = playerHitbox->right[0];
    collisionBottom = playerHitbox->bottom[0];

    int storePos   = 0;
    int storeCount = 0;

    scriptEng.checkResult = false;

    for (int i = 0; i < COLSTORE_COUNT; i++) {
        if (collisionStorage[i].entityNo != -1) {
            storeCount++;
            storePos              = i;
            scriptEng.checkResult = true;
        }
    }

    switch (effect) {
        case ECEFFECT_NONE:
        case ECEFFECT_RESETSTORAGE: {
            if (storeCount == 1) {
                scriptEng.checkResult = false;
                int extendedBottomCol = extendBottomCol ? collisionBottom + 5 : -5;
                Entity *entity        = &objectEntityList[collisionStorage[storePos].entityNo];

                int xCheck1 = (collisionStorage[storePos].left << 16) + entity->XPos;
                int xCheck2 = (collisionStorage[storePos].right << 16) + entity->XPos;
                int yCheck1 = ((collisionStorage[storePos].top + extendedBottomCol) << 16) + entity->YPos;
                int yCheck2 = ((collisionStorage[storePos].bottom + extendedBottomCol) << 16) + entity->YPos;

                if (direction) {
                    if ((collisionLeft << 16) + player->XPos <= xCheck2 && xCheck2 < player->XPos - player->XVelocity) {
                        if (yCheck1 < player->YPos + (collisionBottom << 16) && player->YPos + (collisionTop << 16) < yCheck2){
                            player->XPos          = xCheck2 - (collisionLeft << 16);
                            scriptEng.checkResult = true;
                        }
                        else {
                            if (effect == ECEFFECT_RESETSTORAGE) {
                                for (int i = 0; i < COLSTORE_COUNT; i++) {
                                    CollisionStore *entityHitbox = &collisionStorage[i];
                                    entityHitbox->entityNo = -1;
                                    entityHitbox->type     = -1;
                                    entityHitbox->left     = 0;
                                    entityHitbox->top      = 0;
                                    entityHitbox->right    = 0;
                                    entityHitbox->bottom   = 0;
                                }
                            }                        
                        }
                    }
                }
                else {
                    if ((collisionRight << 16) + player->XPos >= xCheck1 && xCheck1 > player->XPos - player->XVelocity) {
                        player->XPos = xCheck1 - (collisionRight << 16);
                        if (yCheck1 < player->YPos + (collisionBottom << 16) && player->YPos + (collisionTop << 16) < yCheck2){
                            scriptEng.checkResult = true;
                            player->XPos          = xCheck1 - (collisionRight << 16);
                        }
                        else {
                            if (effect == ECEFFECT_RESETSTORAGE) {
                                for (int i = 0; i < COLSTORE_COUNT; i++) {
                                    CollisionStore *entityHitbox = &collisionStorage[i];
                                    entityHitbox->entityNo = -1;
                                    entityHitbox->type     = -1;
                                    entityHitbox->left     = 0;
                                    entityHitbox->top      = 0;
                                    entityHitbox->right    = 0;
                                    entityHitbox->bottom   = 0;
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (effect == 1) {
                    for (int i = 0; i < COLSTORE_COUNT; i++) {
                        CollisionStore *entityHitbox = &collisionStorage[i];
                        entityHitbox->entityNo       = -1;
                        entityHitbox->type           = -1;
                        entityHitbox->left           = 0;
                        entityHitbox->top            = 0;
                        entityHitbox->right          = 0;
                        entityHitbox->bottom         = 0;
                    }
                }
            }
            break;
        }
        case ECEFFECT_BOXCOL3: {
            CollisionStore *entityHitbox = &collisionStorage[storePos];
            for (int o = 0; o < ENTITY_COUNT; o++) {
                Entity *entity = &objectEntityList[o];
                if (entityHitbox->type == entity->type) {
                    BoxCollision3(entity->XPos + (entityHitbox->left << 16), entity->YPos + (entityHitbox->top << 16),
                                  entity->XPos + (entityHitbox->right << 16), entity->YPos + (entityHitbox->bottom << 16));
                }
            }
            break;
        }
        default: break;
    }

}
void TouchCollision(int left, int top, int right, int bottom)
{
    Player *player       = &playerList[activePlayer];
    Hitbox *playerHitbox = getPlayerHitbox(player);

    collisionLeft   = player->XPos >> 16;
    collisionTop    = player->YPos >> 16;
    collisionRight  = collisionLeft;
    collisionBottom = collisionTop;
    collisionLeft += playerHitbox->left[0];
    collisionTop += playerHitbox->top[0];
    collisionRight += playerHitbox->right[0];
    collisionBottom += playerHitbox->bottom[0];
    scriptEng.checkResult = collisionRight > left && collisionLeft < right && collisionBottom > top && collisionTop < bottom;

#if !RETRO_USE_ORIGINAL_CODE
    if (showHitboxes) {
        Entity *entity = &objectEntityList[objectLoop];
        left -= entity->XPos >> 16;
        top -= entity->YPos >> 16;
        right -= entity->XPos >> 16;
        bottom -= entity->YPos >> 16;

        int thisHitboxID = AddDebugHitbox(H_TYPE_TOUCH, entity, left, top, right, bottom);
        if (thisHitboxID >= 0 && scriptEng.checkResult)
            debugHitboxList[thisHitboxID].collision |= 1;

        int otherHitboxID =
            AddDebugHitbox(H_TYPE_TOUCH, NULL, playerHitbox->left[0], playerHitbox->top[0], playerHitbox->right[0], playerHitbox->bottom[0]);
        if (otherHitboxID >= 0) {
            debugHitboxList[otherHitboxID].XPos = player->XPos;
            debugHitboxList[otherHitboxID].YPos = player->YPos;

            if (scriptEng.checkResult)
                debugHitboxList[otherHitboxID].collision |= 1;
        }
    }
#endif
}
void BoxCollision(int left, int top, int right, int bottom)
{
    Player *player       = &playerList[activePlayer];
    Hitbox *playerHitbox = getPlayerHitbox(player);

    collisionLeft         = playerHitbox->left[0];
    collisionTop          = playerHitbox->top[0];
    collisionRight        = playerHitbox->right[0];
    collisionBottom       = playerHitbox->bottom[0];
    scriptEng.checkResult = false;

    int spd = 0;
    switch (player->collisionMode) {
        case CMODE_FLOOR:
        case CMODE_ROOF:
            if (player->XVelocity)
                spd = abs(player->XVelocity);
            else
                spd = abs(player->speed);
            break;
        case CMODE_LWALL:
        case CMODE_RWALL: spd = abs(player->XVelocity); break;
        default: break;
    }
    if (spd <= abs(player->YVelocity)) {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[2].collided = false;
        sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
        sensors[1].XPos     = player->XPos;
        sensors[2].XPos     = player->XPos + ((collisionRight - 2) << 16);
        sensors[0].YPos     = player->YPos + (collisionBottom << 16);
        sensors[1].YPos     = sensors[0].YPos;
        sensors[2].YPos     = sensors[0].YPos;
        if (player->YVelocity > -1) {
            for (int i = 0; i < 3; ++i) {
                if (sensors[i].XPos > left && sensors[i].XPos < right && sensors[i].YPos >= top && player->YPos - player->YVelocity < top) {
                    sensors[i].collided = true;
                    player->flailing[i] = true;
                }
            }
        }
        if (sensors[2].collided || sensors[1].collided || sensors[0].collided) {
            if (!player->gravity && (player->collisionMode == CMODE_RWALL || player->collisionMode == CMODE_LWALL)) {
                player->XVelocity = 0;
                player->speed     = 0;
            }
            player->YPos                  = top - (collisionBottom << 16);
            player->gravity               = 0;
            player->YVelocity             = 0;
            player->angle                 = 0;
            player->boundEntity->rotation = 0;
            player->controlLock           = 0;
            scriptEng.checkResult         = true;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
            sensors[1].XPos     = player->XPos + ((collisionRight - 2) << 16);
            sensors[0].YPos     = player->YPos + (collisionTop << 16);
            sensors[1].YPos     = sensors[0].YPos;
            for (int i = 0; i < 2; ++i) {
                if (sensors[i].XPos > left && sensors[i].XPos < right && sensors[i].YPos <= bottom && player->YPos - player->YVelocity > bottom) {
                    sensors[i].collided = true;
                }
            }
            if (sensors[1].collided || sensors[0].collided) {
                if (player->gravity == 1) {
                    player->YPos = bottom - (collisionTop << 16);
                }
                if (player->YVelocity < 1)
                    player->YVelocity = 0;
                scriptEng.checkResult = 4;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[0].XPos     = player->XPos + (collisionRight << 16);
                sensors[1].XPos     = sensors[0].XPos;
                sensors[0].YPos     = player->YPos - 0x20000;
                sensors[1].YPos     = player->YPos + 0x80000;
                for (int i = 0; i < 2; ++i) {
                    if (sensors[i].XPos >= left && player->XPos - player->XVelocity < left && sensors[1].YPos > top && sensors[0].YPos < bottom) {
                        sensors[i].collided = true;
                    }
                }
                if (sensors[1].collided || sensors[0].collided) {
                    player->XPos = left - (collisionRight << 16);
                    if (player->XVelocity > 0) {
                        if (!player->boundEntity->direction)
                            player->pushing = 2;
                        player->XVelocity = 0;
                        player->speed     = 0;
                    }
                    scriptEng.checkResult = 2;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = player->XPos + (collisionLeft << 16);
                    sensors[1].XPos     = sensors[0].XPos;
                    sensors[0].YPos     = player->YPos - 0x20000;
                    sensors[1].YPos     = player->YPos + 0x80000;
                    for (int i = 0; i < 2; ++i) {
                        if (sensors[i].XPos <= right && player->XPos - player->XVelocity > right && sensors[1].YPos > top
                            && sensors[0].YPos < bottom) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || (sensors[0].collided)) {
                        player->XPos = right - (collisionLeft << 16);
                        if (player->XVelocity < 0) {
                            if (player->boundEntity->direction == FLIP_X)
                                player->pushing = 2;
                            player->XVelocity = 0;
                            player->speed     = 0;
                        }
                        scriptEng.checkResult = 3;
                    }
                }
            }
        }
    }
    else {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[0].XPos     = player->XPos + (collisionRight << 16);
        sensors[1].XPos     = sensors[0].XPos;
        sensors[0].YPos     = player->YPos - 0x20000;
        sensors[1].YPos     = player->YPos + 0x80000;
        for (int i = 0; i < 2; ++i) {
            if (sensors[i].XPos >= left && player->XPos - player->XVelocity < left && sensors[1].YPos > top && sensors[0].YPos < bottom) {
                sensors[i].collided = true;
            }
        }
        if (sensors[1].collided || sensors[0].collided) {
            player->XPos = left - (collisionRight << 16);
            if (player->XVelocity > 0) {
                if (!player->boundEntity->direction)
                    player->pushing = 2;
                player->XVelocity = 0;
                player->speed     = 0;
            }
            scriptEng.checkResult = 2;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = player->XPos + (collisionLeft << 16);
            sensors[1].XPos     = sensors[0].XPos;
            sensors[0].YPos     = player->YPos - 0x20000;
            sensors[1].YPos     = player->YPos + 0x80000;
            for (int i = 0; i < 2; ++i) {
                if (sensors[i].XPos <= right && player->XPos - player->XVelocity > right && sensors[1].YPos > top && sensors[0].YPos < bottom) {
                    sensors[i].collided = true;
                }
            }
            if (sensors[1].collided || sensors[0].collided) {
                player->XPos = right - (collisionLeft << 16);
                if (player->XVelocity < 0) {
                    if (player->boundEntity->direction == FLIP_X) {
                        player->pushing = 2;
                    }
                    player->XVelocity = 0;
                    player->speed     = 0;
                }
                scriptEng.checkResult = 3;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[2].collided = false;
                sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
                sensors[1].XPos     = player->XPos;
                sensors[2].XPos     = player->XPos + ((collisionRight - 2) << 16);
                sensors[0].YPos     = player->YPos + (collisionBottom << 16);
                sensors[1].YPos     = sensors[0].YPos;
                sensors[2].YPos     = sensors[0].YPos;
                if (player->YVelocity > -1) {
                    for (int i = 0; i < 3; ++i) {
                        if (sensors[i].XPos > left && sensors[i].XPos < right && sensors[i].YPos >= top && player->YPos - player->YVelocity < top) {
                            sensors[i].collided = true;
                            player->flailing[i] = true;
                        }
                    }
                }
                if (sensors[2].collided || sensors[1].collided || sensors[0].collided) {
                    if (!player->gravity && (player->collisionMode == CMODE_RWALL || player->collisionMode == CMODE_LWALL)) {
                        player->XVelocity = 0;
                        player->speed     = 0;
                    }
                    player->YPos                  = top - (collisionBottom << 16);
                    player->gravity               = 0;
                    player->YVelocity             = 0;
                    player->angle                 = 0;
                    player->boundEntity->rotation = 0;
                    player->controlLock           = 0;
                    scriptEng.checkResult         = true;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
                    sensors[1].XPos     = player->XPos + ((collisionRight - 2) << 16);
                    sensors[0].YPos     = player->YPos + (collisionTop << 16);
                    sensors[1].YPos     = sensors[0].YPos;
                    for (int i = 0; i < 2; ++i) {
                        if (sensors[i].XPos > left && sensors[i].XPos < right && sensors[i].YPos <= bottom
                            && player->YPos - player->YVelocity > bottom) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || sensors[0].collided) {
                        if (player->gravity == 1) {
                            player->YPos = bottom - (collisionTop << 16);
                        }
                        if (player->YVelocity < 1)
                            player->YVelocity = 0;
                        scriptEng.checkResult = 4;
                    }
                }
            }
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    int thisHitboxID = 0;
    if (showHitboxes) {
        Entity *entity = &objectEntityList[objectLoop];
        left -= entity->XPos;
        top -= entity->YPos;
        right -= entity->XPos;
        bottom -= entity->YPos;

        thisHitboxID = AddDebugHitbox(H_TYPE_BOX, &objectEntityList[objectLoop], left >> 16, top >> 16, right >> 16, bottom >> 16);
        if (thisHitboxID >= 0 && scriptEng.checkResult)
            debugHitboxList[thisHitboxID].collision |= 1 << (scriptEng.checkResult - 1);

        int otherHitboxID =
            AddDebugHitbox(H_TYPE_BOX, NULL, playerHitbox->left[0], playerHitbox->top[0], playerHitbox->right[0], playerHitbox->bottom[0]);
        if (otherHitboxID >= 0) {
            debugHitboxList[otherHitboxID].XPos = player->XPos;
            debugHitboxList[otherHitboxID].YPos = player->YPos;

            if (scriptEng.checkResult)
                debugHitboxList[otherHitboxID].collision |= 1 << (4 - scriptEng.checkResult);
        }
    }
#endif
}
void BoxCollision2(int left, int top, int right, int bottom)
{
    Player *player       = &playerList[activePlayer];
    Hitbox *playerHitbox = getPlayerHitbox(player);

    collisionLeft         = playerHitbox->left[0];
    collisionTop          = playerHitbox->top[0];
    collisionRight        = playerHitbox->right[0];
    collisionBottom       = playerHitbox->bottom[0];
    scriptEng.checkResult = false;
    int spd               = 0;
    switch (player->collisionMode) {
        case CMODE_FLOOR:
        case CMODE_ROOF:
            if (player->XVelocity)
                spd = abs(player->XVelocity);
            else
                spd = abs(player->speed);
            break;
        case CMODE_LWALL:
        case CMODE_RWALL: spd = abs(player->XVelocity); break;
        default: break;
    }
    if (spd <= abs(player->YVelocity)) {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[2].collided = false;
        sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
        sensors[1].XPos     = player->XPos;
        sensors[2].XPos     = player->XPos + ((collisionRight - 2) << 16);
        sensors[0].YPos     = player->YPos + (collisionBottom << 16);
        sensors[1].YPos     = sensors[0].YPos;
        sensors[2].YPos     = sensors[0].YPos;
        if (player->YVelocity > -1) {
            for (int i = 0; i < 3; ++i) {
                if (sensors[i].XPos > left && sensors[i].XPos < right && sensors[i].YPos >= top && player->YPos - player->YVelocity < top) {
                    sensors[i].collided = true;
                    player->flailing[i] = true;
                }
            }
        }
        if (sensors[2].collided || sensors[1].collided || sensors[0].collided) {
            if (!player->gravity && (player->collisionMode == CMODE_RWALL || player->collisionMode == CMODE_LWALL)) {
                player->XVelocity = 0;
                player->speed     = 0;
            }
            player->YPos                  = top - (collisionBottom << 16);
            player->gravity               = 0;
            player->YVelocity             = 0;
            player->angle                 = 0;
            player->boundEntity->rotation = 0;
            player->controlLock           = 0;
            scriptEng.checkResult         = 1;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
            sensors[1].XPos     = player->XPos + ((collisionRight - 2) << 16);
            sensors[0].YPos     = player->YPos + (collisionTop << 16);
            sensors[1].YPos     = player->YPos + (collisionTop << 16);

            for (int i = 0; i < 2; ++i) {
                if (left < sensors[i].XPos && right > sensors[i].XPos && bottom >= sensors[i].YPos && bottom < player->YPos - player->YVelocity) {
                    sensors[i].collided = true;
                }
            }

            if (sensors[1].collided || sensors[0].collided) {
                if (player->gravity == 1)
                    player->YPos = bottom - (collisionTop << 16);

                if (player->YVelocity < 1)
                    player->YVelocity = 0;
                scriptEng.checkResult = 4;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[0].XPos     = player->XPos + (collisionRight << 16);
                sensors[1].XPos     = player->XPos + (collisionRight << 16);
                sensors[0].YPos     = player->YPos + ((collisionBottom - 2) << 16);
                sensors[1].YPos     = player->YPos + ((collisionTop + 2) << 16);
                for (int i = 0; i < 2; ++i) {
                    if (sensors[i].XPos >= left && player->XPos - player->XVelocity < left && sensors[0].YPos > top && sensors[1].YPos < bottom) {
                        sensors[i].collided = true;
                    }
                }

                if (sensors[1].collided || sensors[0].collided) {
                    player->XPos = left - (collisionRight << 16);
                    if (player->XVelocity > 0) {
                        if (player->boundEntity->direction == FLIP_NONE)
                            player->pushing = 2;
                        player->XVelocity = 0;
                        player->speed     = 0;
                    }
                    scriptEng.checkResult = 2;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = sensors[0].XPos;
                    sensors[1].XPos     = player->XPos + (collisionLeft << 16);
                    sensors[0].YPos     = player->YPos + ((collisionBottom - 2) << 16);
                    sensors[1].YPos     = player->YPos + ((collisionTop + 2) << 16);
                    for (int i = 0; i < 2; ++i) {
                        if (sensors[i].XPos <= right && player->XPos - player->XVelocity > right && sensors[0].YPos > top
                            && sensors[1].YPos < bottom) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || (sensors[0].collided)) {
                        player->XPos = right - (collisionLeft << 16);
                        if (player->XVelocity < 0) {
                            if (player->boundEntity->direction == FLIP_X)
                                player->pushing = 2;
                            player->XVelocity = 0;
                            player->speed     = 0;
                        }
                        scriptEng.checkResult = 3;
                    }
                }
            }
        }
    }
    else {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[0].XPos     = player->XPos + (collisionRight << 16);
        sensors[1].XPos     = player->XPos + (collisionRight << 16);
        sensors[0].YPos     = player->YPos + ((collisionBottom - 2) << 16);
        sensors[1].YPos     = player->YPos + ((collisionTop + 2) << 16);
        for (int i = 0; i < 2; ++i) {
            if (sensors[i].XPos >= left && player->XPos - player->XVelocity < left && sensors[0].YPos > top && sensors[1].YPos < bottom) {
                sensors[i].collided = true;
            }
        }

        if (sensors[1].collided || sensors[0].collided) {
            player->XPos = left - (collisionRight << 16);
            if (player->XVelocity > 0) {
                if (!player->boundEntity->direction)
                    player->pushing = 2;
                player->XVelocity = 0;
                player->speed     = 0;
            }
            scriptEng.checkResult = 2;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;
            sensors[0].XPos     = sensors[0].XPos;
            sensors[1].XPos     = player->XPos + (collisionLeft << 16);
            sensors[0].YPos     = player->YPos + ((collisionBottom - 2) << 16);
            sensors[1].YPos     = player->YPos + ((collisionTop + 2) << 16);
            for (int i = 0; i < 2; ++i) {
                if (sensors[i].XPos <= right && player->XPos - player->XVelocity > right && sensors[0].YPos > top && sensors[1].YPos < bottom) {
                    sensors[i].collided = true;
                }
            }

            if (sensors[1].collided || sensors[0].collided) {
                player->XPos = right - (collisionLeft << 16);
                if (player->XVelocity < 0) {
                    if (player->boundEntity->direction == FLIP_X) {
                        player->pushing = 2;
                    }
                    player->XVelocity = 0;
                    player->speed     = 0;
                }
                scriptEng.checkResult = 3;
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                sensors[2].collided = false;
                sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
                sensors[1].XPos     = player->XPos;
                sensors[2].XPos     = player->XPos + ((collisionRight - 2) << 16);
                sensors[0].YPos     = player->YPos + (collisionBottom << 16);
                sensors[1].YPos     = sensors[0].YPos;
                sensors[2].YPos     = sensors[0].YPos;
                if (player->YVelocity > -1) {
                    for (int i = 0; i < 3; ++i) {
                        if (sensors[i].XPos > left && sensors[i].XPos < right && sensors[i].YPos >= top && player->YPos - player->YVelocity < top) {
                            sensors[i].collided = true;
                            player->flailing[i] = true;
                        }
                    }
                }
                if (sensors[2].collided || sensors[1].collided || sensors[0].collided) {
                    if (!player->gravity && (player->collisionMode == CMODE_RWALL || player->collisionMode == CMODE_LWALL)) {
                        player->XVelocity = 0;
                        player->speed     = 0;
                    }
                    player->YPos                  = top - (collisionBottom << 16);
                    player->gravity               = 0;
                    player->YVelocity             = 0;
                    player->angle                 = 0;
                    player->boundEntity->rotation = 0;
                    player->controlLock           = 0;
                    scriptEng.checkResult         = 1;
                }
                else {
                    sensors[0].collided = false;
                    sensors[1].collided = false;
                    sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
                    sensors[1].XPos     = player->XPos + ((collisionRight - 2) << 16);
                    sensors[0].YPos     = player->YPos + (collisionTop << 16);
                    sensors[1].YPos     = player->YPos + (collisionTop << 16);

                    for (int i = 0; i < 2; ++i) {
                        if (left < sensors[i].XPos && right > sensors[i].XPos && bottom >= sensors[i].YPos
                            && bottom < player->YPos - player->YVelocity) {
                            sensors[i].collided = true;
                        }
                    }

                    if (sensors[1].collided || sensors[0].collided) {
                        if (player->gravity == 1) {
                            player->YPos = bottom - (collisionTop << 16);
                        }
                        if (player->YVelocity < 1)
                            player->YVelocity = 0;
                        scriptEng.checkResult = 4;
                    }
                }
            }
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    int thisHitboxID = 0;
    if (showHitboxes) {
        Entity *entity = &objectEntityList[objectLoop];
        left -= entity->XPos;
        top -= entity->YPos;
        right -= entity->XPos;
        bottom -= entity->YPos;

        thisHitboxID = AddDebugHitbox(H_TYPE_BOX, &objectEntityList[objectLoop], left >> 16, top >> 16, right >> 16, bottom >> 16);
        if (thisHitboxID >= 0 && scriptEng.checkResult)
            debugHitboxList[thisHitboxID].collision |= 1 << (scriptEng.checkResult - 1);

        int otherHitboxID =
            AddDebugHitbox(H_TYPE_BOX, NULL, playerHitbox->left[0], playerHitbox->top[0], playerHitbox->right[0], playerHitbox->bottom[0]);
        if (otherHitboxID >= 0) {
            debugHitboxList[otherHitboxID].XPos = player->XPos;
            debugHitboxList[otherHitboxID].YPos = player->YPos;

            if (scriptEng.checkResult)
                debugHitboxList[otherHitboxID].collision |= 1 << (4 - scriptEng.checkResult);
        }
    }
#endif
}
void PlatformCollision(int left, int top, int right, int bottom)
{
    Player *player       = &playerList[activePlayer];
    Hitbox *playerHitbox = getPlayerHitbox(player);

    collisionLeft         = playerHitbox->left[0];
    collisionTop          = playerHitbox->top[0];
    collisionRight        = playerHitbox->right[0];
    collisionBottom       = playerHitbox->bottom[0];
    sensors[0].collided   = false;
    sensors[1].collided   = false;
    sensors[2].collided   = false;
    sensors[0].XPos       = player->XPos + ((collisionLeft + 1) << 16);
    sensors[1].XPos       = player->XPos;
    sensors[2].XPos       = player->XPos + (collisionRight << 16);
    sensors[0].YPos       = player->YPos + (collisionBottom << 16);
    sensors[1].YPos       = sensors[0].YPos;
    sensors[2].YPos       = sensors[0].YPos;
    scriptEng.checkResult = false;
    for (int i = 0; i < 3; ++i) {
        if (sensors[i].XPos > left && sensors[i].XPos < right && sensors[i].YPos > top - 2 && sensors[i].YPos < bottom && player->YVelocity >= 0) {
            sensors[i].collided = 1;
            player->flailing[i] = 1;
        }
    }

    if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
        if (!player->gravity && (player->collisionMode == CMODE_RWALL || player->collisionMode == CMODE_LWALL)) {
            player->XVelocity = 0;
            player->speed     = 0;
        }
        player->YPos                  = top - (collisionBottom << 16);
        player->gravity               = 0;
        player->YVelocity             = 0;
        player->angle                 = 0;
        player->boundEntity->rotation = 0;
        player->controlLock           = 0;
        scriptEng.checkResult         = true;
    }

#if !RETRO_USE_ORIGINAL_CODE
    int thisHitboxID = 0;
    if (showHitboxes) {
        Entity *entity = &objectEntityList[objectLoop];
        left -= entity->XPos;
        top -= entity->YPos;
        right -= entity->XPos;
        bottom -= entity->YPos;

        thisHitboxID = AddDebugHitbox(H_TYPE_PLAT, &objectEntityList[objectLoop], left >> 16, top >> 16, right >> 16, bottom >> 16);
        if (thisHitboxID >= 0 && scriptEng.checkResult)
            debugHitboxList[thisHitboxID].collision |= 1 << 0;

        int otherHitboxID =
            AddDebugHitbox(H_TYPE_PLAT, NULL, playerHitbox->left[0], playerHitbox->top[0], playerHitbox->right[0], playerHitbox->bottom[0]);
        if (otherHitboxID >= 0) {
            debugHitboxList[otherHitboxID].XPos = player->XPos;
            debugHitboxList[otherHitboxID].YPos = player->YPos;

            if (scriptEng.checkResult)
                debugHitboxList[otherHitboxID].collision |= 1 << 3;
        }
    }
#endif
}
void BoxCollision3(int left, int top, int right, int bottom)
{
    Player *player       = &playerList[activePlayer];
    Hitbox *playerHitbox = getPlayerHitbox(player);

    collisionLeft   = playerHitbox->left[0];
    collisionTop    = playerHitbox->top[0];
    collisionRight  = playerHitbox->right[0];
    collisionBottom = playerHitbox->bottom[0];

    scriptEng.checkResult = false;

    sensors[0].collided = false;
    sensors[1].collided = false;
    sensors[2].collided = false;
    sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
    sensors[1].XPos     = player->XPos;
    sensors[2].XPos     = player->XPos + ((collisionRight - 2) << 16);
    sensors[0].YPos     = player->YPos + (collisionBottom << 16);
    sensors[1].YPos     = sensors[0].YPos;
    sensors[2].YPos     = sensors[0].YPos;

    if (player->YVelocity > -1) {
        for (int i = 0; i < 3; ++i) {
            if ((left < sensors[i].XPos && sensors[i].XPos < right) && (top <= sensors[i].YPos && player->YPos - player->YVelocity < top)) {
                sensors[i].collided = true;
                player->flailing[i] = 1;
            }
        }
    }
    if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
        if ((!player->gravity) && (player->collisionMode == CMODE_RWALL || player->collisionMode == CMODE_LWALL)) {
            player->XVelocity = 0;
            player->speed     = 0;
        }
        player->gravity               = 0;
        player->YPos                  = top - (collisionBottom << 16);
        player->YVelocity             = 0;
        player->angle                 = 0;
        player->boundEntity->rotation = 0;
        player->controlLock           = 0;
        scriptEng.checkResult         = true;
    }
    else {
        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[0].XPos     = player->XPos + ((collisionLeft + 2) << 16);
        sensors[1].XPos     = player->XPos + ((collisionRight - 2) << 16);
        sensors[0].YPos     = player->YPos + (collisionTop << 16);
        sensors[1].YPos     = sensors[0].YPos;
        for (int i = 0; i < 2; ++i) {
            if ((left < sensors[i].XPos && sensors[i].XPos < right)
                && (sensors[i].YPos <= bottom && bottom < player->YPos - player->YVelocity)) {
                sensors[i].collided = true;
            }
        }
        if (sensors[0].collided || sensors[1].collided) {
            if (player->gravity == 1) {
                player->YPos = bottom - (collisionTop << 16);
            }
            if (player->YVelocity < 1) {
                player->YVelocity = 0;
            }
            scriptEng.checkResult = 4;
        }
        else {
            sensors[0].collided = false;
            sensors[1].collided = false;

            if (left <= player->XPos + (collisionRight << 16) && player->XPos - player->XVelocity < left) {
                for (int i = 0; i < 2; ++i) {
                    if (top < player->YPos + (collisionBottom << 16) && sensors[i].YPos < bottom) {
                        sensors[i].collided = true;
                    }
                }
            }
            if (sensors[0].collided || sensors[1].collided) {
                scriptEng.checkResult = 2;
                player->XPos          = left - (collisionRight << 16);
                for (int i = 0; i < COLSTORE_COUNT; i++) {
                    CollisionStore *entityHitbox = &collisionStorage[i];
                    if (entityHitbox->entityNo == objectLoop)
                        break;

                    if (entityHitbox->entityNo == -1) {
                        entityHitbox->entityNo       = objectLoop;
                        entityHitbox->type           = objectEntityList[objectLoop].type;
                        entityHitbox->left           = scriptEng.operands[1];
                        entityHitbox->top            = scriptEng.operands[2];
                        entityHitbox->right          = scriptEng.operands[3];
                        entityHitbox->bottom         = scriptEng.operands[4];
                        break;
                    }
                }
            }
            else {
                sensors[0].collided = false;
                sensors[1].collided = false;
                if ((collisionLeft << 16) + player->XPos <= right && right < player->XPos - player->XVelocity) {
                    for (int i = 0; i < 2; ++i) {
                        if (top < (collisionBottom << 16) + player->YPos && sensors[i].YPos < bottom) {
                            sensors[i].collided = true;
                        }
                    }
                }
                if (sensors[0].collided || sensors[1].collided) {
                    scriptEng.checkResult = 3;
                    player->XPos          = right - (collisionLeft << 16);
                    for (int i = 0; i < COLSTORE_COUNT; i++) {
                        CollisionStore *entityHitbox = &collisionStorage[i];
                        if (entityHitbox->entityNo == objectLoop)
                            break;

                        if (entityHitbox->entityNo == -1) {
                            entityHitbox->entityNo       = objectLoop;
                            entityHitbox->type           = objectEntityList[objectLoop].type;
                            entityHitbox->left           = scriptEng.operands[1];
                            entityHitbox->top            = scriptEng.operands[2];
                            entityHitbox->right          = scriptEng.operands[3];
                            entityHitbox->bottom         = scriptEng.operands[4];
                            break;
                        }
                    }
                }
            }
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    int thisHitboxID = 0;
    if (showHitboxes) {
        Entity *entity = &objectEntityList[objectLoop];
        left -= entity->XPos;
        top -= entity->YPos;
        right -= entity->XPos;
        bottom -= entity->YPos;

        thisHitboxID = AddDebugHitbox(H_TYPE_BOX, &objectEntityList[objectLoop], left >> 16, top >> 16, right >> 16, bottom >> 16);
        if (thisHitboxID >= 0 && scriptEng.checkResult)
            debugHitboxList[thisHitboxID].collision |= 1 << 0;

        int otherHitboxID =
            AddDebugHitbox(H_TYPE_BOX, NULL, playerHitbox->left[0], playerHitbox->top[0], playerHitbox->right[0], playerHitbox->bottom[0]);
        if (otherHitboxID >= 0) {
            debugHitboxList[otherHitboxID].XPos = player->XPos;
            debugHitboxList[otherHitboxID].YPos = player->YPos;

            if (scriptEng.checkResult)
                debugHitboxList[otherHitboxID].collision |= 1 << 3;
        }
    }
#endif
}
// Note for those that care: This is not a direct decomp of this function
// We did try to make one, but the original function was too much of a mess to comprehend fully, so we opted to instead take what we could understand from it and make a new one that replicates the behavior the best we could
// If anyone out there wants to take a shot at fully decompiling the original function, feel free to do so and send a PR, but this should be good enough as is
void EnemyCollision(int left, int top, int right, int bottom)
{
    TouchCollision(left, top, right, bottom);

#if RSDK_AUTOBUILD
    // Skip the hammer hitboxes on autobuilds, just in case
    return;
#endif

    Player *player = &playerList[activePlayer];

    int hammerHitboxLeft   = 0;
    int hammerHitboxRight  = 0;
    int hammerHitboxTop    = 0;
    int hammerHitboxBottom = 0;

#if !RETRO_USE_ORIGINAL_CODE
    bool miniPlayerFlag = GetGlobalVariableByName("Mini_PlayerFlag");
    sbyte playerAmy     = GetGlobalVariableByName("PLAYER_AMY") ? GetGlobalVariableByName("PLAYER_AMY") : 5;
    sbyte aniHammerJump = GetGlobalVariableByName("ANI_HAMMER_JUMP") ? GetGlobalVariableByName("ANI_HAMMER_JUMP") : 45;
    sbyte aniHammerDash = GetGlobalVariableByName("ANI_HAMMER_DASH") ? GetGlobalVariableByName("ANI_HAMMER_DASH") : 46;
#else
    bool mini_PlayerFlag = globalVariables[62];
    sbyte playerAmy      = 5;
    sbyte aniHammerJump  = 45;
    sbyte aniHammerDash  = 46;
#endif

    collisionLeft   = player->XPos >> 16;
    collisionTop    = player->YPos >> 16;
    collisionRight  = collisionLeft;
    collisionBottom = collisionTop;

    if (!scriptEng.checkResult) {
        if (playerListPos == playerAmy) {
            if (player->boundEntity->animation == aniHammerDash) {
                int frame          = (miniPlayerFlag ? player->boundEntity->frame % 3 : player->boundEntity->frame % 8) * 4;
                hammerHitboxLeft   = miniPlayerFlag ? chibiHammerDashHitbox[frame] : hammerDashHitbox[frame];
                hammerHitboxTop    = miniPlayerFlag ? chibiHammerDashHitbox[frame + 1] : hammerDashHitbox[frame + 1];
                hammerHitboxRight  = miniPlayerFlag ? chibiHammerDashHitbox[frame + 2] : hammerDashHitbox[frame + 2];
                hammerHitboxBottom = miniPlayerFlag ? chibiHammerDashHitbox[frame + 3] : hammerDashHitbox[frame + 3];
            }
            if (player->boundEntity->animation == aniHammerJump) {
                int frame          = (miniPlayerFlag ? player->boundEntity->frame % 2 : player->boundEntity->frame % 4) * 4;
                hammerHitboxLeft   = miniPlayerFlag ? chibiHammerJumpHitbox[frame]     : hammerJumpHitbox[frame];
                hammerHitboxTop    = miniPlayerFlag ? chibiHammerJumpHitbox[frame + 1] : hammerJumpHitbox[frame + 1];
                hammerHitboxRight  = miniPlayerFlag ? chibiHammerJumpHitbox[frame + 2] : hammerJumpHitbox[frame + 2];
                hammerHitboxBottom = miniPlayerFlag ? chibiHammerJumpHitbox[frame + 3] : hammerJumpHitbox[frame + 3];
            }
            if (player->boundEntity->direction) {
                int storeHitboxLeft =  hammerHitboxLeft;
                hammerHitboxLeft    = -hammerHitboxRight;
                hammerHitboxRight   = -storeHitboxLeft;
            }
            scriptEng.checkResult = collisionRight + hammerHitboxRight > left && collisionLeft + hammerHitboxLeft < right
                                    && collisionBottom + hammerHitboxBottom > top && collisionTop + hammerHitboxTop < bottom;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (showHitboxes) {
        Entity *entity = &objectEntityList[objectLoop];
        left -= entity->XPos >> 16;
        top -= entity->YPos >> 16;
        right -= entity->XPos >> 16;
        bottom -= entity->YPos >> 16;

        Hitbox *playerHitbox = getPlayerHitbox(player);

        int thisHitboxID = AddDebugHitbox(H_TYPE_HAMMER, entity, left, top, right, bottom);
        if (thisHitboxID >= 0 && scriptEng.checkResult)
            debugHitboxList[thisHitboxID].collision |= 1;

        int otherHitboxID    = AddDebugHitbox(H_TYPE_HAMMER, NULL, hammerHitboxLeft, hammerHitboxTop, hammerHitboxRight, hammerHitboxBottom);
        if (otherHitboxID >= 0) {
            debugHitboxList[otherHitboxID].XPos = player->XPos;
            debugHitboxList[otherHitboxID].YPos = player->YPos;

            if (scriptEng.checkResult)
                debugHitboxList[otherHitboxID].collision |= 1;
        }
    }
#endif
}