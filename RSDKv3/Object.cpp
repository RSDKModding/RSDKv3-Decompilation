#include "RetroEngine.hpp"

int objectLoop    = 0;
int curObjectType = 0;
Entity objectEntityList[ENTITY_COUNT];

char typeNames[OBJECT_COUNT][0x40];

int OBJECT_BORDER_X1       = 0x80;
int OBJECT_BORDER_X2       = 0;
const int OBJECT_BORDER_Y1 = 0x100;
const int OBJECT_BORDER_Y2 = SCREEN_YSIZE + 0x100;

void SetObjectTypeName(const char *objectName, int objectID)
{
    int objNameID  = 0;
    int typeNameID = 0;
    while (objectName[objNameID]) {
        if (objectName[objNameID] != ' ')
            typeNames[objectID][typeNameID++] = objectName[objNameID];
        ++objNameID;
    }
    typeNames[objectID][typeNameID] = 0;
    printLog("Set Object (%d) name to: %s", objectID, objectName);
}

void ProcessStartupObjects()
{
    scriptFrameCount = 0;
    ClearAnimationData();
    activePlayer               = 0;
    activePlayerCount          = 1;
    scriptEng.arrayPosition[2] = TEMPENTITY_START;
    Entity *entity             = &objectEntityList[TEMPENTITY_START];
    for (int i = 0; i < OBJECT_COUNT; ++i) {
        ObjectScript *scriptInfo    = &objectScriptList[i];
        objectLoop                  = TEMPENTITY_START;
        curObjectType               = i;
        scriptInfo->frameListOffset = scriptFrameCount;
        scriptInfo->spriteSheetID   = 0;
        entity->type                = i;
        if (scriptData[scriptInfo->subStartup.scriptCodePtr] > 0)
            ProcessScript(scriptInfo->subStartup.scriptCodePtr, scriptInfo->subStartup.jumpTablePtr, SUB_SETUP);
        scriptInfo->frameCount = scriptFrameCount - scriptInfo->frameListOffset;
    }
    entity->type  = 0;
    curObjectType = 0;
}

void ProcessObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        bool active = false;
        int x = 0, y = 0;
        Entity *entity = &objectEntityList[objectLoop];
        switch (entity->priority) {
            case PRIORITY_ACTIVE_BOUNDS:
                x      = entity->XPos >> 16;
                y      = entity->YPos >> 16;
                active = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset && y > yScrollOffset - OBJECT_BORDER_Y1
                         && y < yScrollOffset + OBJECT_BORDER_Y2;
                break;
            case PRIORITY_ACTIVE: active = true; break;
            case PRIORITY_ACTIVE_PAUSED: active = true; break;
            case PRIORITY_ACTIVE_XBOUNDS:
                x      = entity->XPos >> 16;
                active = x > xScrollOffset - OBJECT_BORDER_X1 && x < OBJECT_BORDER_X2 + xScrollOffset;
                break;
            case PRIORITY_ACTIVE_BOUNDS_REMOVE:
                x = entity->XPos >> 16;
                y = entity->YPos >> 16;
                if (x <= xScrollOffset - OBJECT_BORDER_X1 || x >= OBJECT_BORDER_X2 + xScrollOffset || y <= yScrollOffset - OBJECT_BORDER_Y1
                    || y >= yScrollOffset + OBJECT_BORDER_Y2) {
                    active       = false;
                    entity->type = OBJ_TYPE_BLANKOBJECT;
                }
                else {
                    active = true;
                }
                break;
            case PRIORITY_INACTIVE: active = false; break;
            default: break;
        }
        if (active && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            activePlayer             = 0;
            if (scriptData[scriptInfo->subMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->subMain.scriptCodePtr, scriptInfo->subMain.jumpTablePtr, SUB_MAIN);
            if (scriptData[scriptInfo->subPlayerInteraction.scriptCodePtr] > 0) {
                while (activePlayer < activePlayerCount) {
                    if (playerList[activePlayer].objectInteractions)
                        ProcessScript(scriptInfo->subPlayerInteraction.scriptCodePtr, scriptInfo->subPlayerInteraction.jumpTablePtr,
                                      SUB_PLAYERINTERACTION);
                    ++activePlayer;
                }
            }

            if (entity->drawOrder < DRAWLAYER_COUNT)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectLoop;
        }
    }
}

void ProcessPausedObjects()
{
    for (int i = 0; i < DRAWLAYER_COUNT; ++i) drawListEntries[i].listSize = 0;

    for (objectLoop = 0; objectLoop < ENTITY_COUNT; ++objectLoop) {
        Entity *entity = &objectEntityList[objectLoop];

        if (entity->priority == PRIORITY_ACTIVE_PAUSED && entity->type > OBJ_TYPE_BLANKOBJECT) {
            ObjectScript *scriptInfo = &objectScriptList[entity->type];
            activePlayer             = 0;
            if (scriptData[scriptInfo->subMain.scriptCodePtr] > 0)
                ProcessScript(scriptInfo->subMain.scriptCodePtr, scriptInfo->subMain.jumpTablePtr, SUB_MAIN);
            if (scriptData[scriptInfo->subPlayerInteraction.scriptCodePtr] > 0) {
                while (activePlayer < PLAYER_COUNT) {
                    if (playerList[activePlayer].objectInteractions)
                        ProcessScript(scriptInfo->subPlayerInteraction.scriptCodePtr, scriptInfo->subPlayerInteraction.jumpTablePtr,
                                      SUB_PLAYERINTERACTION);
                    ++activePlayer;
                }
            }

            if (entity->drawOrder < DRAWLAYER_COUNT)
                drawListEntries[entity->drawOrder].entityRefs[drawListEntries[entity->drawOrder].listSize++] = objectLoop;
        }
    }
}