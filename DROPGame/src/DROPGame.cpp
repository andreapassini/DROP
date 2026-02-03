#include "DROPGame.h"

#include <iostream>
#include <vector>

static bool bFirstExe = true;

void PrintNumber(const int InNumber)
{
	std::cout << "PrintNumber " << InNumber << std::endl;
}

void StartGame()
{
	std::cout << "<<< StartGame" << std::endl;

    bFirstExe = true;
}

void UpdateGame(
    const float deltaTime
    , DropEngineCalls* EngineCalls
) {	
	if (bFirstExe) {
        bFirstExe = false;
	}

    if (EngineCalls->engineCall)
    {
        EngineCalls->engineCall();
    }
}

#ifdef DROP_PLATFORM_WINDOWS

#include "Windows.h"

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL // handle to DLL module
    , DWORD fdwReason    // reason for calling function
    , LPVOID lpReserved   // reserved
) {
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        bFirstExe = true;
        std::cout << "DLL_PROCESS_ATTACH" << std::endl;
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        std::cout << "DLL_THREAD_ATTACH" << std::endl;
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        std::cout << "DLL_THREAD_DETACH" << std::endl;
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        std::cout << "DLL_PROCESS_DETACH" << std::endl;
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#endif // DROP_PLATFORM_WINDOWS