#ifndef RETROENGINE_H
#define RETROENGINE_H

//Disables POSIX use c++ name blah blah stuff
#pragma warning(disable:4996)

// ================
// STANDARD LIBS
// ================
#include <stdio.h>
#include <string.h>

// ================
// STANDARD TYPES
// ================
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef int RSDK_BOOL;

#define RSDK_DEBUG					(1)

//Platforms
#define RSDK_PLATFORM_WINDOWS		(0)

#define RSDK_TARGETPLATFORM			(RSDK_PLATFORM_WINDOWS)

#include "Animation.h"
#include "Audio.h"
#include "Collision.h"
#include "Input.h"
#include "Math.h"
#include "Object.h"
#include "Drawing.h"
#include "Drawing3D.h"
#include "Palette.h"
#include "Player.h"
#include "Reader.h"
#include "Scene.h"
#include "Script.h"
#include "Sprite.h"
#include "String.h"
#include "TextSystem.h"

class RetroEngine {
public:

	bool usingDataFile = false;
	bool usingBytecode = false;
	
	bool Initialised = false;
	bool Running = false;

	int GameMode = 1;

	void Init();
	void Run();

	void LoadGameConfig(const char* Filepath);

	char GameWindowText[0x40];
	char GameDescriptionText[0x100];
};

static RetroEngine Engine;

static int GlobalVariablesCount;
static int GlobalVariables[0x100];
static char GlobalVariableNames[0x100][0x100];
#endif // !RETROENGINE_H
