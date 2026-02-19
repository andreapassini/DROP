#include "DropEngine.h"

#include <iostream>

#include "Utils/Log.h"
#include "Window/Window.h"

#include "GameEngine.h"
#include "EngineState.h"
#include "DROPGame.h"
#include "Memory/arenaAllocator.h"

#include "glfw/glfw3.h"

using namespace Drop;

static DropEngineCalls gDropEngineCalls;
static EngineMemory gEngineMemory;

void TestEngineCall(){
}
 

// For GLFW compatibility
//typedef void* (*GLFWallocatefun)(size_t size, void* user);
void* TempGLFWAllocate(
    size_t size
    , void* user
) {
    ArenaAllocator* arenaAllocator = (ArenaAllocator*)(user);
    assert(arenaAllocator);

    return ArenaAlloc(
        arenaAllocator
        , size
    );
}
//typedef void* (*GLFWreallocatefun)(void* block, size_t size, void* user);
void* TempGLFWReallocate(
    void* block
    , size_t size
    , void* user
) {
    ArenaAllocator* arenaAllocator = (ArenaAllocator*)(user);
    assert(arenaAllocator);

    return ArenaResize(
        arenaAllocator
        , block
        , 0L // this will call an ArenaAlloc right away, not a big deal for now
        , size
    );
}
//typedef void (*GLFWdeallocatefun)(void* block, void* user);
void TempGLFWDeallocate(
    void* block
    , void* user
) {
    ArenaAllocator* arenaAllocator = (ArenaAllocator*)(user);
    assert(arenaAllocator);

    return ArenaFree(
        arenaAllocator
        , block
    );
}


void StartEngine(DropPlatformCalls* platformCalls)
{
    Drop::Log::Init();
    LOG_CORE_WARN("Initialized Log!");

    LOG_CORE_INFO("Drop Engine starting");

    // Memory initialization, consider doing this by reading config file
    gEngineMemory.persistentMemorysizeInBytes = Megabytes(128);
    gEngineMemory.sceneMemorysizeInBytes = Gigabytes(2);
    gEngineMemory.frameMemorySizeInBytes = Megabytes(128);
    gEngineMemory.sizeInBytes = gEngineMemory.persistentMemorysizeInBytes 
        + gEngineMemory.sceneMemorysizeInBytes 
        + gEngineMemory.frameMemorySizeInBytes;

    gEngineMemory.persistentMemory = platformCalls->allocateMemory(
        gEngineMemory.sizeInBytes
        , (LPVOID)TerabytesWRONG(5)
    );
    assert(gEngineMemory.persistentMemory);

    uintptr_t persistentMemoryUIntptr = (uintptr_t)gEngineMemory.persistentMemory;

    gEngineMemory.sceneMemory = (void*)(
        (uintptr_t)gEngineMemory.persistentMemory + gEngineMemory.persistentMemorysizeInBytes
    );

    uintptr_t sceneMemoryUIntptr = (uintptr_t)gEngineMemory.sceneMemory;

    gEngineMemory.frameMemory = (void*)(
        (uintptr_t)gEngineMemory.sceneMemory + gEngineMemory.sceneMemorysizeInBytes
    );

    uintptr_t frameMemoryUIntptr = (uintptr_t)gEngineMemory.frameMemory;

    //  persistentMemory              sceneMemory                frameMemory
    //  persistentMemorysizeInBytes   sceneMemorysizeInBytes     frameMemorySizeInBytes
    // [-----------------------------|--------------------------|-----------------------]

    assert(sizeof(EngineState) <= gEngineMemory.persistentMemorysizeInBytes);
    EngineState* engineState = (EngineState*)(
        (void*)(
            (uintptr_t)gEngineMemory.persistentMemory 
            /*+ sizeof(EngineState)*/ // IDIOT THIS IS NOT AHEAD. THE BUFFER MUST BE MOVED AHEAD FUCKING DUMASS
        )
    );
    assert(engineState);
    uintptr_t engineStateUIntptr = (uintptr_t)((void*)(engineState));

    uintptr_t persistentMemoryAfterGameState = engineStateUIntptr + sizeof(EngineState);

    ArenaInit(
        &engineState->persistentArenaAllocator
        , (void*)persistentMemoryAfterGameState
        , gEngineMemory.persistentMemorysizeInBytes
    );
    ArenaInit(
        &engineState->sceneArenaAllocator
        , gEngineMemory.sceneMemory
        , gEngineMemory.sceneMemorysizeInBytes
    );
    ArenaInit(
        &engineState->frameArenaAllocator
        , gEngineMemory.frameMemory
        , gEngineMemory.frameMemorySizeInBytes
    );

    // We need to allocate memory on the .exe (Platform Layer)
    // Allocate on PermanentStorage
    size_t WindowSize = sizeof(Drop::Window);
    engineState->windowHandle = (Drop::Window*)ArenaAlloc(
        &engineState->persistentArenaAllocator
        , WindowSize
    );
    WindowProps windowProps;
    // glfw allocator
    //typedef struct GLFWallocator
    //{
    //    GLFWallocatefun allocate;
    //    GLFWreallocatefun reallocate;
    //    GLFWdeallocatefun deallocate;
    //    void* user;
    //};
    GLFWallocator currentAllocator;
    currentAllocator.allocate = TempGLFWAllocate;
    currentAllocator.reallocate = TempGLFWReallocate;
    currentAllocator.deallocate = TempGLFWDeallocate;
    currentAllocator.user = (void*)(&engineState->persistentArenaAllocator);

    engineState->windowHandle->Init(
        windowProps
        , &currentAllocator // struct copied inside GLFW call
    );
    //engineState->windowHandle = Window::Create();
    //Input::m_WindowHandle = (GLFWwindow*)m_WindowHandle->GetNativeWindow();
}

void UpdateEngine(
    DropPlatformCalls* platformCalls
    , GameProcAdresses* gameCalls
) {
    // Get the updated engine calls (this could be conditional, only if DLL updated)
    gDropEngineCalls.engineCall = TestEngineCall;

    // Get time from glfwGetTime
    const float deltaTime = (float)glfwGetTime();
    std::cout << "Time: " << deltaTime << std::endl;

    if (gameCalls->UpdateGame)
    {
        // call UpdateGame
        gameCalls->UpdateGame(
            deltaTime
            , &gDropEngineCalls
        );
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