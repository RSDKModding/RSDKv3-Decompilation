#include "RetroEngine.hpp"

Player playerList[PLAYER_COUNT];
int playerListPos     = 0;
int activePlayer      = 0;
int activePlayerCount = 1;

ushort upBuffer        = 0;
ushort downBuffer      = 0;
ushort leftBuffer      = 0;
ushort rightBuffer     = 0;
ushort jumpPressBuffer = 0;
ushort jumpHoldBuffer  = 0;

void ProcessPlayerControl(Player *player)
{
    if (player->controlMode == -1) {
        upBuffer <<= 1;
        upBuffer |= (byte)player->up;
        downBuffer <<= 1;
        downBuffer |= (byte)player->down;
        leftBuffer <<= 1;
        leftBuffer |= (byte)player->left;
        rightBuffer <<= 1;
        rightBuffer |= (byte)player->right;
        jumpPressBuffer <<= 1;
        jumpPressBuffer |= (byte)player->jumpPress;
        jumpHoldBuffer <<= 1;
        jumpHoldBuffer |= (byte)player->jumpHold;
    }
    else if (player->controlMode == 1) {
        player->up        = upBuffer >> 15;
        player->down      = downBuffer >> 15;
        player->left      = leftBuffer >> 15;
        player->right     = rightBuffer >> 15;
        player->jumpPress = jumpPressBuffer >> 15;
        player->jumpHold  = jumpHoldBuffer >> 15;
    }
    else {
        player->up   = keyDown.up;
        player->down = keyDown.down;
        if (!keyDown.left || !keyDown.right) {
            player->left  = keyDown.left;
            player->right = keyDown.right;
        }
        else {
            player->left  = false;
            player->right = false;
        }
        player->jumpHold  = keyDown.C | keyDown.B | keyDown.A;
        player->jumpPress = keyPress.C | keyPress.B | keyPress.A;
        upBuffer <<= 1;
        upBuffer |= (byte)player->up;
        downBuffer <<= 1;
        downBuffer |= (byte)player->down;
        leftBuffer <<= 1;
        leftBuffer |= (byte)player->left;
        rightBuffer <<= 1;
        rightBuffer |= (byte)player->right;
        jumpPressBuffer <<= 1;
        jumpPressBuffer |= (byte)player->jumpPress;
        jumpHoldBuffer <<= 1;
        jumpHoldBuffer |= (byte)player->jumpHold;
    }
}