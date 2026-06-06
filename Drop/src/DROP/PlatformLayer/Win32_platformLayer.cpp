#ifdef DROP_PLATFORM_WINDOWS

#include <windows.h> 
#include <cstdint>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>
#include <assert.h>

// Platform includes
#include "Win32_platformLayer.h"
#include "platformLayer.h"

#include "DropEngine.h"
#include "GameEngine.h"
#include "DropGame.h"
#include "Memory/memoryAllocator.h"
#include "Strings/stringUtils.h"
#include "Types/Types.h"

struct DropFileTime
{
    FILETIME fileTime;
};

inline FILETIME Win32_GetLastWriteTime(
    char* Filename
) {
    FILETIME LastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (GetFileAttributesEx(Filename, GetFileExInfoStandard, &Data))
    {
        LastWriteTime = Data.ftLastWriteTime;
    }
    
    return(LastWriteTime);
}


// MOVE THE WINDOWS STUFF TO PLATFORM LAYER
// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void Win32_LoadLibrary(
    HINSTANCE& inHinstLib
    , GameDLL& InGameFunctions
    , char* SourceDLLName
    , char* TempDLLName
) {
    BOOL /*fFreeResult,*/ fRunTimeLinkSuccess = FALSE;

    InGameFunctions.bIsValid = false;

    InGameFunctions.DLLastWriteTime = Win32_GetLastWriteTime(SourceDLLName);
    //CopyFile(T_GAME_DLL_NAME, T_GAME_DLL_TEMP_NAME, FALSE);
    CopyFileA(SourceDLLName, TempDLLName, FALSE);

    // Get a handle to the DLL module
    //inHinstLib = LoadLibrary(T_GAME_DLL_NAME);
    inHinstLib = LoadLibraryA(TempDLLName);

    // If the handle is valid, try to get the function address.

    if (NULL != inHinstLib)
    {
        InGameFunctions.procAdresses.PrintNumber = (PRINT_NUMBER)GetProcAddress(inHinstLib, InGameFunctions.procAdresses.PrintNumberName);
        InGameFunctions.procAdresses.StartGame = (START_GAME)GetProcAddress(inHinstLib, InGameFunctions.procAdresses.StartGameName);
        InGameFunctions.procAdresses.UpdateGame = (UPDATE_GAME)GetProcAddress(inHinstLib, InGameFunctions.procAdresses.UpdateGameName);

        // If the function address is valid, call the function.
        fRunTimeLinkSuccess = TRUE;
        if (NULL == InGameFunctions.procAdresses.PrintNumber)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InGameFunctions.procAdresses.StartGame)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InGameFunctions.procAdresses.UpdateGame)
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
    , EngineDLL& InEngineFunctions
    , char* SourceDLLName
    , char* TempDLLName
) {
    BOOL /*fFreeResult,*/ fRunTimeLinkSuccess = FALSE;

    InEngineFunctions.bIsValid = false;

    InEngineFunctions.DLLastWriteTime = Win32_GetLastWriteTime(SourceDLLName);
    //CopyFile(T_ENGINE_DLL_NAME, T_ENGINE_DLL_TEMP_NAME, FALSE);
    CopyFileA(SourceDLLName, TempDLLName, FALSE);

    // Get a handle to the DLL module
    //inHinstLib = LoadLibrary(T_ENGINE_DLL_NAME);
    inHinstLib = LoadLibraryA(TempDLLName);

    // If the handle is valid, try to get the function address.

    if (NULL != inHinstLib)
    {
        InEngineFunctions.procAdresses.StartEngine = (START_ENGINE)GetProcAddress(inHinstLib, InEngineFunctions.procAdresses.StartEngineName);
        InEngineFunctions.procAdresses.UpdateEngine = (UPDATE_ENGINE)GetProcAddress(inHinstLib, InEngineFunctions.procAdresses.UpdateEngineName);

        // If the function address is valid, call the function.

        if (NULL == InEngineFunctions.procAdresses.StartEngine)
        {
            fRunTimeLinkSuccess = FALSE;
        }
        // Check error

        if (NULL == InEngineFunctions.procAdresses.UpdateEngine)
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
    , GameDLL& InGameFunctions
) {
    BOOL fFreeResult;

    if (NULL != inHinstLib)
    {
        // Free the DLL module.
        fFreeResult = FreeLibrary(inHinstLib);
    }

    InGameFunctions.bIsValid = false;
    InGameFunctions.procAdresses.PrintNumber = PrintNumberStub;
    InGameFunctions.procAdresses.StartGame = StartGameStub;
    InGameFunctions.procAdresses.UpdateGame = UpdateGameStub;
}

// https://learn.microsoft.com/it-it/windows/win32/dlls/using-run-time-dynamic-linking
void Win32_UnloadCode(
    HINSTANCE& inHinstLib
    , EngineDLL& InEngineFunctions
) {
    BOOL fFreeResult;

    if (NULL != inHinstLib)
    {
        // Free the DLL module.
        fFreeResult = FreeLibrary(inHinstLib);
    }

    InEngineFunctions.bIsValid = false;
    InEngineFunctions.procAdresses.StartEngine = StartEngineStub;
    InEngineFunctions.procAdresses.UpdateEngine = UpdateEngineStub;
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
    , GameDLL& gameDLL
    , uint32 LoadCounter
    , char* InSourceGameCodeDLLFullPath
    , char* InTempGameCodeDLLFullPath
) {
    FILETIME NewDLLWriteTime = Win32_GetLastWriteTime(InSourceGameCodeDLLFullPath);
    if (CompareFileTime(&NewDLLWriteTime, &gameDLL.DLLastWriteTime) != 0)
    {
        // Something is wrong with this, i cannot, without bp, update the DLL
        // https://hero.handmade.network/forums/code-discussion/t/3266-weird_bug_with_live_code_editing
        // Fix for now - look at episode 39, the blog quoted a fix
        Sleep(30);
        Win32_UnloadCode(
            hinstLib
            , gameDLL
        );
        Win32_LoadLibrary(
            hinstLib
            , gameDLL
            , InSourceGameCodeDLLFullPath
            , InTempGameCodeDLLFullPath
        );
        LoadCounter = 0;
    }
}

void Win32_CheckAndUpdateDLL(
    HINSTANCE& hinstLib
    , EngineDLL& EngineFunctions
    , uint32 LoadCounter
    , char* InSourceEngineCodeDLLFullPath
    , char* InTempEngineCodeDLLFullPath
) {
    FILETIME NewDLLWriteTime = Win32_GetLastWriteTime(InSourceEngineCodeDLLFullPath);
    if (CompareFileTime(&NewDLLWriteTime, &EngineFunctions.DLLastWriteTime) != 0)
    {
        // Something is wrong with this, i cannot, without bp, update the DLL
        // https://hero.handmade.network/forums/code-discussion/t/3266-weird_bug_with_live_code_editing
        // Fix for now - look at episode 39, the blog quoted a fix
        Win32_UnloadCode(
            hinstLib
            , EngineFunctions
        );
        Win32_LoadLibrary(
            hinstLib
            , EngineFunctions
            , InSourceEngineCodeDLLFullPath
            , InTempEngineCodeDLLFullPath
        );
        LoadCounter = 0;
    }
}
 
// ------------------------------------------------------------

// Platform API to engine 

void TestPlatformCall()
{
    std::cout << "Call from: " << __func__ << std::endl;
}

// ------------------------------------------------------------

bool bUpdatedEngineDLLCheck = false;
bool bUpdatedGameDLLCheck = false;

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

namespace Drop 
{

int Main(int argc, char** argv)
{
#if defined(ENVIRONMENT64)
    if (sizeof(void*) != 8)
    {
        wprintf(L"ENV64BIT: Error: pointer should be 8 bytes. Exiting.");
        exit(0);
    }
    wprintf(L"Diagnostics: we are running in 64-bit mode.\n");
#elif defined (ENVIRONMENT32)
    if (sizeof(void*) != 4)
    {
        wprintf(L"ENV32BIT: Error: pointer should be 4 bytes. Exiting.");
        exit(0);
    }
    wprintf(L"Diagnostics: we are running in 32-bit mode.\n");
#else
#error "Must define either ENV32BIT or ENV64BIT".
#endif

    // Engine DLL
    char SourceEngineCodeDLLFullPath[MAX_PATH];
    char TempEngineCodeDLLFullPath[MAX_PATH];
    char engineDLLName[] = ENGINE_DLL_NAME;
    char engineDLLTempName[] = ENGINE_DLL_TEMP_NAME;
    Win32_CleanDLLPath(
        sizeof(SourceEngineCodeDLLFullPath), SourceEngineCodeDLLFullPath
        , sizeof(TempEngineCodeDLLFullPath), TempEngineCodeDLLFullPath
        , sizeof(engineDLLName) - 1, engineDLLName
        , sizeof(engineDLLTempName) - 1, engineDLLTempName
    );

    uint32_t engineLoadCounter = 0;
    EngineDLL engineDLL;
    HINSTANCE engineHinstLib;
    Win32_LoadLibrary(
        engineHinstLib
        , engineDLL
        , SourceEngineCodeDLLFullPath
        , TempEngineCodeDLLFullPath
    );


    // Game DLL
    char SourceGameCodeDLLFullPath[MAX_PATH];
    char TempGameCodeDLLFullPath[MAX_PATH];
    char gameDLLName[] = GAME_DLL_NAME;
    char gameDLLTempName[] = GAME_DLL_TEMP_NAME;
    Win32_CleanDLLPath(
        sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath
        , sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath
        , sizeof(gameDLLName) - 1, gameDLLName
        , sizeof(gameDLLTempName) - 1, gameDLLTempName
    );

    uint32_t gameLoadCounter = 0;
    GameDLL gameDLL;
    HINSTANCE gameHinstLib;
    Win32_LoadLibrary(
        gameHinstLib
        , gameDLL
        , SourceGameCodeDLLFullPath
        , TempGameCodeDLLFullPath
    );

    DropPlatformCalls platformCalls;
    platformCalls.platformCall = TestPlatformCall;
    platformCalls.allocateMemory = AllocateMemory;

    EngineMemory engineMemory;

    engineDLL.procAdresses.StartEngine(
        &platformCalls
        , &engineMemory // this will be initialized by the Engine
    );

    gameDLL.procAdresses.StartGame();

    // MainLoop
    while (g_GameEngineRunning)
    {
        // Call it also for the Engine DLL via an Engine side call
        Win32_CheckAndUpdateDLL(
            engineHinstLib
            , engineDLL
            , engineLoadCounter
            , SourceEngineCodeDLLFullPath
            , TempEngineCodeDLLFullPath
        );

        Win32_CheckAndUpdateDLL(
            gameHinstLib
            , gameDLL
            , gameLoadCounter
            , SourceGameCodeDLLFullPath
            , TempGameCodeDLLFullPath
        );

        if (engineDLL.procAdresses.UpdateEngine)
        {
            engineDLL.procAdresses.UpdateEngine(
                &platformCalls
                , &engineMemory // this will was initialized by the Engine in EngineStart()
                , &gameDLL.procAdresses
            );
        }
        //if(gameDLL.UpdateGame)
        //    gameDLL.UpdateGame(0.0166f, &EngineCalls);
    }

    return 0;
}

void* AllocateMemory(
    size_t sizeInBytes
    , void* startingAddress /*= nullptr*/
){
    wprintf(L"%s of sizeInBytes = %zu.\n", __func__, sizeInBytes);

    return VirtualAlloc(
        startingAddress
        , sizeInBytes
        , MEM_COMMIT | MEM_RESERVE
        , PAGE_READWRITE
    );
}

}
#endif // DROP_PLATFORM_WINDOWS