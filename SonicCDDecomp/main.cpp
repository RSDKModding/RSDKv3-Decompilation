#include "RetroEngine.hpp"

#if RETRO_PLATFORM == RETRO_3DS
void awaitInput() {
     while (aptMainLoop()) {
	hidScanInput();
	if (hidKeysDown())
		break;

	gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();
    }
}  
#endif

int main(int argc, char *argv[])
{
    #if RETRO_PLATFORM == RETRO_3DS
    // debug code to make sure it actually runs
    // TODO: move this code to RetroEngine eventually
    
    // at this point in time, N3DS clock speeds are necessary
    // for the game to even run at full speed
    osSetSpeedupEnable(true);

    gfxInitDefault();
    DebugConsoleInit();
    printf("This port is far from complete. You\n"
           "may experience bugs, glitches, and\n"
	   "other oddities that might detract\n"
	   "from your overall experience.\n\n");

    awaitInput();

    printf("RetroEngine init\n");
    if (!Engine.Init()) {
	    printf("Press any button to continue.\n");
	    awaitInput();
	    gfxExit();
	    return 0;
    }

    printf("Audio enabled: %d\n", audioEnabled);
    printf("Engine succesfully initialised.\n");
    //awaitInput();

    
    printf("Running engine...\n");
    Engine.Run();

    printf("Exiting...");
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
