#include "RetroEngine.hpp"


#include <unistd.h>

int main(int argc, char *argv[])
{
    const char* cwd = getcwd(NULL,0);
    printf("CWD: %s",cwd);
    
    Engine.Init();
    Engine.Run();

    return 0;
}
