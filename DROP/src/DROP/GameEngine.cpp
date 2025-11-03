#include "GameEngine.h"

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>

#include "DROPGame.h"

#ifdef DROP_PLATFORM_WINDOWS

#include <windows.h> 

// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
static GameDLLProAdresses GameFunctions;

bool g_GameEngineRunning = true;

namespace Drop {

int Main(int argc, char** argv)
{
    HINSTANCE hinstLib;
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.

    hinstLib = LoadLibrary(GAME_DLL_NAME);

    // If the handle is valid, try to get the function address.

    if (hinstLib != NULL)
    {
        GameFunctions.PrintNumber = (PRINT_NUMBER)GetProcAddress(hinstLib, GameFunctions.PrintNumberName);
        GameFunctions.StartGame = (START_GAME)GetProcAddress(hinstLib, GameFunctions.StartGameName);
        GameFunctions.UpdateGame = (UPDATE_GAME)GetProcAddress(hinstLib, GameFunctions.UpdateGameName);

        // If the function address is valid, call the function.

        if (NULL != GameFunctions.PrintNumber)
        {
            fRunTimeLinkSuccess = TRUE;
            (GameFunctions.PrintNumber)(999);
        }
        // Free the DLL module.

        //fFreeResult = FreeLibrary(hinstLib);
    }

    // If unable to call the DLL function, use an alternative.
    if (!fRunTimeLinkSuccess)
        printf("Message printed from executable\n");


    GameFunctions.StartGame();

    while (g_GameEngineRunning)
    {
        GameFunctions.UpdateGame(0.0166f);
        
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
