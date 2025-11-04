#include "GameEngine.h"

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>

#include "DROPGame.h"

#ifdef DROP_PLATFORM_WINDOWS

#include <windows.h> 

bool g_GameEngineRunning = true;

namespace Drop {

// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void LinkGameLibrary(HINSTANCE& inHinstLib, GameDLLProAdresses& InGameFunctions)
{
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    InGameFunctions.bIsValid = false;

    // Get a handle to the DLL module
    inHinstLib = LoadLibrary(GAME_DLL_NAME);

    // If the handle is valid, try to get the function address.

    if (NULL != inHinstLib)
    {
        InGameFunctions.PrintNumber = (PRINT_NUMBER)GetProcAddress(inHinstLib, InGameFunctions.PrintNumberName);
        InGameFunctions.StartGame = (START_GAME)GetProcAddress(inHinstLib, InGameFunctions.StartGameName);
        InGameFunctions.UpdateGame = (UPDATE_GAME)GetProcAddress(inHinstLib, InGameFunctions.UpdateGameName);

        // If the function address is valid, call the function.
        fRunTimeLinkSuccess = TRUE;
        if (NULL == InGameFunctions.PrintNumber) {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InGameFunctions.StartGame) {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InGameFunctions.UpdateGame) {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (fRunTimeLinkSuccess == TRUE) {
            InGameFunctions.bIsValid = true;
        }
    }

    // If unable to call the DLL function, use an alternative.
    if (NULL != fRunTimeLinkSuccess)
        printf("Message printed from executable\n");
}

// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void FreeGameLibrary(HINSTANCE& inHinstLib, GameDLLProAdresses& InGameFunctions)
{
    BOOL fFreeResult;

    if (NULL != inHinstLib) {
        // Free the DLL module.
        fFreeResult = FreeLibrary(inHinstLib);
    }
    
    InGameFunctions.bIsValid = false;
    InGameFunctions.PrintNumber = PrintNumberStub;
    InGameFunctions.StartGame = StartGameStub;
    InGameFunctions.UpdateGame = UpdateGameStub;
}

int Main(int argc, char** argv)
{
    GameDLLProAdresses GameFunctions;
    HINSTANCE hinstLib;
    LinkGameLibrary(hinstLib, GameFunctions);

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
