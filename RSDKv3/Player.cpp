#include "RetroEngine.hpp"

Player playerList[PLAYER_COUNT];
int playerListPos = 0;
int activePlayer  = 0;
int activePlayerCount = 1;

ushort upBuffer        = 0;
ushort downBuffer      = 0;
ushort leftBuffer      = 0;
ushort rightBuffer     = 0;
ushort jumpPressBuffer = 0;
ushort jumpHoldBuffer  = 0;

void ProcessPlayerControl(Player *Player)
{
    if (Player->controlMode == -1) {
        upBuffer <<= 1;
        upBuffer |= (byte)Player->up;
        downBuffer <<= 1;
        downBuffer |= (byte)Player->down;
        leftBuffer <<= 1;
        leftBuffer |= (byte)Player->left;
        rightBuffer <<= 1;
        rightBuffer |= (byte)Player->right;
        jumpPressBuffer <<= 1;
        jumpPressBuffer |= (byte)Player->jumpPress;
        jumpHoldBuffer <<= 1;
        jumpHoldBuffer |= (byte)Player->jumpHold;
    }
    else if (Player->controlMode == 1) {
        Player->up        = upBuffer >> 15;
        Player->down      = downBuffer >> 15;
        Player->left      = leftBuffer >> 15;
        Player->right     = rightBuffer >> 15;
        Player->jumpPress = jumpPressBuffer >> 15;
        Player->jumpHold  = jumpHoldBuffer >> 15;
    }
    else {
        Player->up   = keyDown.up;
        Player->down = keyDown.down;
        if (!keyDown.left || !keyDown.right) {
            Player->left  = keyDown.left;
            Player->right = keyDown.right;
        }
        else {
            Player->left  = false;
            Player->right = false;
        }
        Player->jumpHold  = keyDown.C | keyDown.B | keyDown.A;
        Player->jumpPress = keyPress.C | keyPress.B | keyPress.A;
        upBuffer <<= 1;
        upBuffer |= (byte)Player->up;
        downBuffer <<= 1;
        downBuffer |= (byte)Player->down;
        leftBuffer <<= 1;
        leftBuffer |= (byte)Player->left;
        rightBuffer <<= 1;
        rightBuffer |= (byte)Player->right;
        jumpPressBuffer <<= 1;
        jumpPressBuffer |= (byte)Player->jumpPress;
        jumpHoldBuffer <<= 1;
        jumpHoldBuffer |= (byte)Player->jumpHold;
    }
}