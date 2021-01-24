#include "../RetroEngine.hpp"

void DebugConsoleInit() {
	consoleInit(GFX_BOTTOM, NULL);

	printf("--------------------------------\n");
	printf("|   RSDKv3 3DS Debug Console   |\n");
	printf("--------------------------------\n");

	printf("The programmer has a nap.\n");
	printf("Hold out! Programmer!\n\n");
}
