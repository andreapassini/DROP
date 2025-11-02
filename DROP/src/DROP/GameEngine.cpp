#include "GameEngine.h"

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>

#include "DROPGame.h"

#ifdef DROP_PLATFORM_WINDOWS

bool g_GameEngineRunning = true;

namespace Drop {

int Main(int argc, char** argv)
{
    StartGame();

    while (g_GameEngineRunning)
    {
        UpdateGame(0.0166f);

        
    }

    return 0;
}

}

#ifdef DROP_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return Drop::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
    return Drop::Main(argc, argv);
}

#endif // DROP_DIST

#endif // DROP_PLATFORM_WINDOWS
