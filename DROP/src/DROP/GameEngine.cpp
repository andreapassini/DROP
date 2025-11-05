#include "GameEngine.h"

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>

#include "DROPGame.h"

#ifdef DROP_PLATFORM_WINDOWS

#include <windows.h> 

bool g_GameEngineRunning = true;

namespace Drop {

// MOVE THE WINDOWS STUFF TO PLATFORM LAYER
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

static void CatStrings(size_t SourceACount, char* SourceA,
    size_t SourceBCount, char* SourceB,
    size_t DestCount, char* Dest)
{
    // TODO(casey): Dest bounds checking!

    for (int Index = 0;
        Index < SourceACount;
        ++Index)
    {
        *Dest++ = *SourceA++;
    }

    for (int Index = 0;
        Index < SourceBCount;
        ++Index)
    {
        *Dest++ = *SourceB++;
    }

    *Dest++ = 0;
}


inline FILETIME Win32GetLastWriteTime(char* Filename)
{
    FILETIME LastWriteTime = {};

    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(Filename, &FindData);
    if (FindHandle != INVALID_HANDLE_VALUE)
    {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }

    return(LastWriteTime);
}
// MOVE THE WINDOWS STUFF TO PLATFORM LAYER

int Main(int argc, char** argv)
{
    // NOTE(casey): Never use MAX_PATH in code that is user-facing, because it
    // can be dangerous and lead to bad results.
    char EXEFileName[MAX_PATH];
    DWORD SizeOfFilename = GetModuleFileNameA(0, EXEFileName, sizeof(EXEFileName));
    char* OnePastLastSlash = EXEFileName;
    for (char* Scan = EXEFileName;
        *Scan;
        ++Scan)
    {
        if (*Scan == '\\')
        {
            OnePastLastSlash = Scan + 1;
        }
    }

    char SourceGameCodeDLLFilename[] = "handmade.dll";
    char SourceGameCodeDLLFullPath[MAX_PATH];
    CatStrings(OnePastLastSlash - EXEFileName, EXEFileName,
        sizeof(SourceGameCodeDLLFilename) - 1, SourceGameCodeDLLFilename,
        sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath);

    char TempGameCodeDLLFilename[] = "handmade_temp.dll";
    char TempGameCodeDLLFullPath[MAX_PATH];
    CatStrings(OnePastLastSlash - EXEFileName, EXEFileName,
        sizeof(TempGameCodeDLLFilename) - 1, TempGameCodeDLLFilename,
        sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath);

    GameDLLProAdresses GameFunctions;
    HINSTANCE hinstLib;
    LinkGameLibrary(hinstLib, GameFunctions);

    GameFunctions.StartGame();
    
    while (g_GameEngineRunning)
    {
        FILETIME NewDLLWriteTime = Win32GetLastWriteTime(SourceGameCodeDLLFullPath);
        if (CompareFileTime(&NewDLLWriteTime, &Game.DLLLastWriteTime) != 0)
        {
            Win32UnloadGameCode(&Game);
            Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                TempGameCodeDLLFullPath);
            LoadCounter = 0;
        }


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
