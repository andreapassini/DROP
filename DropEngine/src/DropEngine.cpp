#include "DropEngine.h"

#include <iostream>

#include "Utils/Log.h"
#include "Window/Window.h"

using namespace Drop;

static bool bFirstExe = true;

void StartEngine()
{
	Drop::Log::Init();
	LOG_CORE_WARN("Initialized Log!");

	LOG_CORE_INFO("Drop Engine starting");

	// to be removed
	Window* m_WindowHandle = Window::Create();
	//Input::m_WindowHandle = (GLFWwindow*)m_WindowHandle->GetNativeWindow();


}

void  UpdateEngine(
	const float deltaTime
	, PlatformCalls* platformCalls
) {
	// Call the the Game Update

}

void  UpdatedGameDLL(
) {
	// Update the DLL

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