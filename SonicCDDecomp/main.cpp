#include "RetroEngine.hpp"

int main(int argc, char *argv[])
{
    #if RETRO_PLATFORM == RETRO_3DS
    // debug code to make sure it actually runs
    gfxInitDefault();
    DebugConsoleInit();

    while (aptMainLoop()) {
	if (hidKeysDown() & KEY_START)
		break;
    }

    gfxExit();

    #else
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }

    Engine.Init();
    Engine.Run();
    #endif

    return 0;
}
