#ifdef DROP_PLATFORM_WINDOWS

#include <windows.h> 
#include <cstdint>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>
#include <assert.h>

#include "DROP/GameEngine.h"
#include "DROPGame.h"
#include "DROP/Memory/memoryAllocator.h"
#include "DROP/Strings/stringUtils.h"

#include "DROP/PlatformLayer/platformLayer.h"

namespace Drop {

inline FILETIME Win32_GetLastWriteTime(
    char* Filename
) {
    FILETIME LastWriteTime = {};

    WIN32_FIND_DATAA FindData;
    HANDLE FindHandle = FindFirstFileA(Filename, &FindData);
    if (FindHandle != INVALID_HANDLE_VALUE)
    {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }

    return(LastWriteTime);
}

// MOVE THE WINDOWS STUFF TO PLATFORM LAYER
// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void Win32_LoadGameLibrary(
    HINSTANCE& inHinstLib
    , GameDLLProAdresses& InGameFunctions
    , char* SourceDLLName
    , char* TempDLLName
) {
    BOOL /*fFreeResult,*/ fRunTimeLinkSuccess = FALSE;

    InGameFunctions.bIsValid = false;

    InGameFunctions.DLLLastWriteTime = Win32_GetLastWriteTime(SourceDLLName);
    //CopyFile(T_GAME_DLL_NAME, T_GAME_DLL_TEMP_NAME, FALSE);
    CopyFileA(SourceDLLName, TempDLLName, FALSE);

    // Get a handle to the DLL module
    //inHinstLib = LoadLibrary(T_GAME_DLL_NAME);
    inHinstLib = LoadLibraryA(TempDLLName);

    // If the handle is valid, try to get the function address.

    if (NULL != inHinstLib)
    {
        InGameFunctions.PrintNumber = (PRINT_NUMBER)GetProcAddress(inHinstLib, InGameFunctions.PrintNumberName);
        InGameFunctions.StartGame = (START_GAME)GetProcAddress(inHinstLib, InGameFunctions.StartGameName);
        InGameFunctions.UpdateGame = (UPDATE_GAME)GetProcAddress(inHinstLib, InGameFunctions.UpdateGameName);

        // If the function address is valid, call the function.
        fRunTimeLinkSuccess = TRUE;
        if (NULL == InGameFunctions.PrintNumber)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InGameFunctions.StartGame)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InGameFunctions.UpdateGame)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (fRunTimeLinkSuccess == TRUE)
        {
            InGameFunctions.bIsValid = true;
        }
    }

    // If unable to call the DLL function, use an alternative.
    if (NULL != fRunTimeLinkSuccess)
        printf("Message printed from executable\n");
}

// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void Win32_UnloadGameCode(
    HINSTANCE& inHinstLib
    , GameDLLProAdresses& InGameFunctions
) {
    BOOL fFreeResult;

    if (NULL != inHinstLib)
    {
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

    char SourceGameCodeDLLFilename[] = GAME_DLL_NAME;
    char SourceGameCodeDLLFullPath[MAX_PATH];
    ConcatStrings(
        OnePastLastSlash - EXEFileName, EXEFileName,
        sizeof(SourceGameCodeDLLFilename) - 1, SourceGameCodeDLLFilename,
        sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath
    );

    char TempGameCodeDLLFilename[] = GAME_DLL_TEMP_NAME;
    char TempGameCodeDLLFullPath[MAX_PATH];
    ConcatStrings(
        OnePastLastSlash - EXEFileName, EXEFileName,
        sizeof(TempGameCodeDLLFilename) - 1, TempGameCodeDLLFilename,
        sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath
    );

    uint32_t LoadCounter = 0;
    GameDLLProAdresses gameFunctions;
    HINSTANCE hinstLib;
    Win32_LoadGameLibrary(
        hinstLib
        , gameFunctions
        , SourceGameCodeDLLFullPath
        , TempGameCodeDLLFullPath
    );

    gameFunctions.StartGame();

    // MainLoop
    while (g_GameEngineRunning)
    {
        FILETIME NewDLLWriteTime = Win32_GetLastWriteTime(SourceGameCodeDLLFullPath);
        if (CompareFileTime(&NewDLLWriteTime, &gameFunctions.DLLLastWriteTime) != 0)
        {
            // Something is wrong with this, i cannot, without bp, update the DLL
            // https://hero.handmade.network/forums/code-discussion/t/3266-weird_bug_with_live_code_editing
            // Fix for now - look at episode 39, the blog quoted a fix
            Sleep(30);
            Win32_UnloadGameCode(hinstLib, gameFunctions);
            Win32_LoadGameLibrary(
                hinstLib
                , gameFunctions
                , SourceGameCodeDLLFullPath
                , TempGameCodeDLLFullPath
            );
            LoadCounter = 0;
        }

        gameFunctions.UpdateGame(0.0166f);
    }

    return 0;
}

}

#endif // DROP_PLATFORM_WINDOWS