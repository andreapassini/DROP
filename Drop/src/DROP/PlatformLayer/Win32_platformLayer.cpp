#ifdef DROP_PLATFORM_WINDOWS

#include <windows.h> 
#include <cstdint>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>
#include <assert.h>

// Platform includes
#include "Win32_platformLayer.h"

#include "GameEngine.h"
#include "DROPGame.h"
#include "Memory/memoryAllocator.h"
#include "Strings/stringUtils.h"

#include "PlatformLayer/platformLayer.h"
#include "Types/Types.h"
#include "platformLayer.h"


namespace Drop {

struct DropFileTime
{
    FILETIME fileTime;
};

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
void Win32_LoadLibrary(
    HINSTANCE& inHinstLib
    , GameDLLProcAdresses& InGameFunctions
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

// MOVE THE WINDOWS STUFF TO PLATFORM LAYER
// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void Win32_LoadLibrary(
    HINSTANCE& inHinstLib
    , EngineDLLProcAdresses& InEngineFunctions
    , char* SourceDLLName
    , char* TempDLLName
) {
    BOOL /*fFreeResult,*/ fRunTimeLinkSuccess = FALSE;

    InEngineFunctions.bIsValid = false;

    InEngineFunctions.DLLLastWriteTime = Win32_GetLastWriteTime(SourceDLLName);
    //CopyFile(T_ENGINE_DLL_NAME, T_ENGINE_DLL_TEMP_NAME, FALSE);
    CopyFileA(SourceDLLName, TempDLLName, FALSE);

    // Get a handle to the DLL module
    //inHinstLib = LoadLibrary(T_ENGINE_DLL_NAME);
    inHinstLib = LoadLibraryA(TempDLLName);

    // If the handle is valid, try to get the function address.

    if (NULL != inHinstLib)
    {
        InEngineFunctions.PrintNumber = (PRINT_NUMBER)GetProcAddress(inHinstLib, InEngineFunctions.PrintNumberName);
        InEngineFunctions.StartEngine = (START_ENGINE)GetProcAddress(inHinstLib, InEngineFunctions.StartEngineName);
        InEngineFunctions.UpdateEngine = (UPDATE_ENGINE)GetProcAddress(inHinstLib, InEngineFunctions.UpdateEngineName);

        // If the function address is valid, call the function.
        fRunTimeLinkSuccess = TRUE;
        if (NULL == InEngineFunctions.PrintNumber)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InEngineFunctions.StartEngine)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InEngineFunctions.UpdateEngine)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (fRunTimeLinkSuccess == TRUE)
        {
            InEngineFunctions.bIsValid = true;
        }
    }

    // If unable to call the DLL function, use an alternative.
    if (NULL != fRunTimeLinkSuccess)
        printf("Message printed from executable\n");
}

// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void Win32_UnloadCode(
    HINSTANCE& inHinstLib
    , GameDLLProcAdresses& InGameFunctions
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

// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void Win32_UnloadCode(
    HINSTANCE& inHinstLib
    , EngineDLLProcAdresses& InEngineFunctions
) {
    BOOL fFreeResult;

    if (NULL != inHinstLib)
    {
        // Free the DLL module.
        fFreeResult = FreeLibrary(inHinstLib);
    }

    InEngineFunctions.bIsValid = false;
    InEngineFunctions.PrintNumber = PrintNumberStub;
    InEngineFunctions.StartEngine = StartEngineStub;
    InEngineFunctions.UpdateEngine = UpdateEngineStub;
}

void Win32_CleanDLLPath(
    size_t OutSourceGameCodeDLLFullPathSize, char* OutSourceGameCodeDLLFullPath
    , size_t OutTempGameCodeDLLFullPathSize, char* OutTempGameCodeDLLFullPath
    , size_t InDLLNameSize, char* InDLLName
    , size_t InDLLTempNameSize, char* InDLLTempName
) {
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

    size_t SizeOfOutSourceGameCodeDLLFullPath = sizeof(OutSourceGameCodeDLLFullPath);

    char* SourceGameCodeDLLFilename = InDLLName;
    ConcatStrings(
        OnePastLastSlash - EXEFileName, EXEFileName,
        InDLLNameSize, SourceGameCodeDLLFilename,
        OutSourceGameCodeDLLFullPathSize, OutSourceGameCodeDLLFullPath
    );

    char* TempGameCodeDLLFilename = InDLLTempName;
    ConcatStrings(
        OnePastLastSlash - EXEFileName, EXEFileName,
        InDLLTempNameSize, TempGameCodeDLLFilename,
        OutTempGameCodeDLLFullPathSize, OutTempGameCodeDLLFullPath
    );
}

void Win32_CheckAndUpdateDLL(
    HINSTANCE& hinstLib
    , GameDLLProcAdresses& gameFunctions
    , uint32 LoadCounter
    , char* InSourceGameCodeDLLFullPath
    , char* InTempGameCodeDLLFullPath
) {
    FILETIME NewDLLWriteTime = Win32_GetLastWriteTime(InSourceGameCodeDLLFullPath);
    if (CompareFileTime(&NewDLLWriteTime, &gameFunctions.DLLLastWriteTime) != 0)
    {
        // Something is wrong with this, i cannot, without bp, update the DLL
        // https://hero.handmade.network/forums/code-discussion/t/3266-weird_bug_with_live_code_editing
        // Fix for now - look at episode 39, the blog quoted a fix
        Sleep(30);
        Win32_UnloadGameCode(hinstLib, gameFunctions);
        Win32_LoadLibrary(
            hinstLib
            , gameFunctions
            , InSourceGameCodeDLLFullPath
            , InTempGameCodeDLLFullPath
        );
        LoadCounter = 0;

        std::cout << "Time: " << (float)glfwGetTime() << std::endl;
    }
}

void Win32_CheckAndUpdateDLL(
    HINSTANCE& hinstLib
    , EngineDLLProcAdresses& EngineFunctions
    , uint32 LoadCounter
    , char* InSourceEngineCodeDLLFullPath
    , char* InTempEngineCodeDLLFullPath
) {
    FILETIME NewDLLWriteTime = Win32_GetLastWriteTime(InSourceEngineCodeDLLFullPath);
    if (CompareFileTime(&NewDLLWriteTime, &EngineFunctions.DLLLastWriteTime) != 0)
    {
        // Something is wrong with this, i cannot, without bp, update the DLL
        // https://hero.handmade.network/forums/code-discussion/t/3266-weird_bug_with_live_code_editing
        // Fix for now - look at episode 39, the blog quoted a fix
        Sleep(30);
        Win32_UnloadCode(hinstLib, EngineFunctions);
        Win32_LoadLibrary(
            hinstLib
            , EngineFunctions
            , InSourceEngineCodeDLLFullPath
            , InTempEngineCodeDLLFullPath
        );
        LoadCounter = 0;

        std::cout << "Time: " << (float)glfwGetTime() << std::endl;
    }
}

void GetTime()
{
    std::cout << "Time: " << (float)glfwGetTime() << std::endl;
}

 
// ------------------------------------------------------------

bool bUpdatedEngineDLLCheck = false;
bool bUpdatedGameDLLCheck = false;

int Main(int argc, char** argv)
{
    // Engine DLL
    char SourceEngineCodeDLLFullPath[MAX_PATH];
    char TempEngineCodeDLLFullPath[MAX_PATH];
    char DLLName[] = ENGINE_DLL_NAME;
    char DLLTempName[] = ENGINE_DLL_TEMP_NAME;
    Win32_CleanDLLPath(
        sizeof(SourceEngineCodeDLLFullPath), SourceEngineCodeDLLFullPath
        , sizeof(TempEngineCodeDLLFullPath), TempEngineCodeDLLFullPath
        , sizeof(DLLName) - 1, DLLName
        , sizeof(DLLTempName) - 1, DLLTempName
    );

    uint32_t LoadCounter = 0;
    EngineDLLProcAdresses EngineFunctions;
    HINSTANCE hinstLib;
    Win32_LoadLibrary(
        hinstLib
        , EngineFunctions
        , SourceEngineCodeDLLFullPath
        , TempEngineCodeDLLFullPath
    );


    // Game DLL
    char SourceGameCodeDLLFullPath[MAX_PATH];
    char TempGameCodeDLLFullPath[MAX_PATH];
    char DLLName[] = GAME_DLL_NAME;
    char DLLTempName[] = GAME_DLL_TEMP_NAME;
    Win32_CleanDLLPath(
        sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath
        , sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath
        , sizeof(DLLName) - 1, DLLName
        , sizeof(DLLTempName) - 1, DLLTempName
    );

    uint32_t LoadCounter = 0;
    GameDLLProcAdresses gameFunctions;
    HINSTANCE hinstLib;
    Win32_LoadLibrary(
        hinstLib
        , gameFunctions
        , SourceGameCodeDLLFullPath
        , TempGameCodeDLLFullPath
    );

    DropEngineCalls EngineCalls;
    EngineCalls.engineCall = GetTime;

    // Setup Engine
    GameEngine::StartEngine();

    gameFunctions.StartGame();

    // MainLoop
    while (g_GameEngineRunning)
    {
        Win32_CheckAndUpdateDLL(
            hinstLib
            , gameFunctions
            , LoadCounter
            , SourceGameCodeDLLFullPath
            , TempGameCodeDLLFullPath
        );

        // Call it also for the Engine DLL via an Engine side call
        Win32_CheckAndUpdateDLL(
            hinstLib
            , EngineFunctions
            , LoadCounter
            , SourceEngineCodeDLLFullPath
            , TempEngineCodeDLLFullPath
        );

        if(gameFunctions.UpdateGame)
            gameFunctions.UpdateGame(0.0166f, &EngineCalls);
    }

    return 0;
}

#endif // DROP_PLATFORM_WINDOWS