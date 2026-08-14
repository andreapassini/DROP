#include "DropEngine.h"

#include <iostream>

#include "Utils/Log.h"
#include "Window/Window.h"
#include "UI/UI.h"

#include "GameEngine.h"
#include "EngineState.h"
#include "DROPGame.h"
#include "Memory/stackAllocator.h"

#include "GLAD/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

using namespace Drop;

static DropEngineCalls gDropEngineCalls;

void TestEngineCall(){
}


// For GLFW compatibility
//typedef void* (*GLFWallocatefun)(size_t size, void* user);
void* TempGLFWAllocate(
    size_t size
    , void* user
) {
    StackAllocator* stackAllocator = (StackAllocator*)(user);
    assert(stackAllocator);

    return StackAlloc(
        stackAllocator
        , size
    );
}
//typedef void* (*GLFWreallocatefun)(void* block, size_t size, void* user);
void* TempGLFWReallocate(
    void* block
    , size_t size
    , void* user
) {
    StackAllocator* stackAllocator = (StackAllocator*)(user);
    assert(stackAllocator);

    //  StackAllocator* stackAllocator
    //  , void* ptr
    //  , size_t newSize
    //  , size_t alignment = DEFAULT_ALIGNMENT
    return StackRealloc(
        stackAllocator
        , block
        , size
    );
}
//typedef void (*GLFWdeallocatefun)(void* block, void* user);
void TempGLFWDeallocate(
    void* block
    , void* user
) {
    StackAllocator* stackAllocator = (StackAllocator*)(user);
    assert(stackAllocator);

    return StackFree(
        stackAllocator
        , block
    );
}


void StartEngine(
    DropPlatformCalls* platformCalls
    , EngineMemory* engineMemory
    , void* windowPtr
) {
    Drop::Log::Init();
    LOG_CORE_WARN("Initialized Log!");

    LOG_CORE_INFO("Drop Engine starting");

    // Memory initialization, consider doing this by reading config file
    engineMemory->persistentMemorysizeInBytes = Megabytes(128);
    engineMemory->sceneMemorysizeInBytes = Gigabytes(2);
    engineMemory->frameMemorySizeInBytes = Megabytes(128);
    engineMemory->sizeInBytes = engineMemory->persistentMemorysizeInBytes
        + engineMemory->sceneMemorysizeInBytes 
        + engineMemory->frameMemorySizeInBytes;

    engineMemory->persistentMemory = platformCalls->allocateMemory(
        engineMemory->sizeInBytes
        , (LPVOID)TerabytesWRONG(5)
    );
    assert(engineMemory->persistentMemory);

    uintptr_t persistentMemoryUIntptr = (uintptr_t)engineMemory->persistentMemory;

    engineMemory->sceneMemory = (void*)(
        (uintptr_t)engineMemory->persistentMemory + engineMemory->persistentMemorysizeInBytes
    );

    uintptr_t sceneMemoryUIntptr = (uintptr_t)engineMemory->sceneMemory;

    engineMemory->frameMemory = (void*)(
        (uintptr_t)engineMemory->sceneMemory + engineMemory->sceneMemorysizeInBytes
    );

    uintptr_t frameMemoryUIntptr = (uintptr_t)engineMemory->frameMemory;

    //  persistentMemory              sceneMemory                frameMemory
    //  persistentMemorysizeInBytes   sceneMemorysizeInBytes     frameMemorySizeInBytes
    // [-----------------------------|--------------------------|-----------------------]

    assert(sizeof(EngineState) <= engineMemory->persistentMemorysizeInBytes);
    EngineState* engineState = (EngineState*)(
        (void*)(
            (uintptr_t)engineMemory->persistentMemory 
            /*+ sizeof(EngineState)*/ // IDIOT THIS IS NOT AHEAD. THE BUFFER MUST BE MOVED AHEAD FUCKING DUMASS
        )
    );
    assert(engineState);
    uintptr_t engineStateUIntptr = (uintptr_t)((void*)(engineState));

    uintptr_t persistentMemoryAfterGameState = engineStateUIntptr + sizeof(EngineState);

    StackInit(
        &engineState->persistentStackAllocator
        , (void*)persistentMemoryAfterGameState
        , engineMemory->persistentMemorysizeInBytes
    );
    StackInit(
        &engineState->sceneStackAllocator
        , engineMemory->sceneMemory
        , engineMemory->sceneMemorysizeInBytes
    );
    StackInit(
        &engineState->frameStackAllocator
        , engineMemory->frameMemory
        , engineMemory->frameMemorySizeInBytes
    );

    // We need to allocate memory on the .exe (Platform Layer)
    // Allocate on PermanentStorage
    size_t WindowSize = sizeof(Drop::Window);
    engineState->windowHandle = StackAlloc<Drop::Window>(
        &engineState->persistentStackAllocator
        , WindowSize
    );

    WindowProps windowProps;
    GLFWallocator currentAllocator;
    currentAllocator.allocate = TempGLFWAllocate;
    currentAllocator.reallocate = TempGLFWReallocate;
    currentAllocator.deallocate = TempGLFWDeallocate;
    currentAllocator.user = (void*)(&engineState->persistentStackAllocator);

    engineState->windowHandle->glfwWindow = (GLFWwindow*)windowPtr;
    InitWindow(
        windowProps
        , engineState->windowHandle
        , &currentAllocator
    );

    // ImGUI
    // We need to allocate memory on the .exe (Platform Layer)
    // Allocate on PermanentStorage
    size_t UIByteSize = sizeof(UI::UIContext);
    engineState->uiContext = StackAlloc<UI::UIContext>(
        &engineState->persistentStackAllocator
        , UIByteSize
    );

    engineState->imGuiAllocator.allocFunc = TempGLFWAllocate;
    engineState->imGuiAllocator.freeFunc = TempGLFWDeallocate;
    engineState->imGuiAllocator.userData = (void*)(&engineState->persistentStackAllocator);
    UI::InitUI(
        engineState->uiContext
        , &engineState->imGuiAllocator
        , engineState->windowHandle
    );

    //engineState->windowHandle = Window::Create();
    //Input::m_WindowHandle = (GLFWwindow*)m_WindowHandle->GetNativeWindow();

    enum EClockWise
    {
        COUNTER_CLOCKWISE
        , CLOCKWISE
    };


    // we enable Z test
    glEnable(GL_DEPTH_TEST);

    // Backface culling
    if (true)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        switch (EClockWise::CLOCKWISE)
        {
        case EClockWise::CLOCKWISE:
            glFrontFace(GL_CW);
            break;
        case EClockWise::COUNTER_CLOCKWISE:
            glFrontFace(GL_CCW);
            break;
        default:
            glFrontFace(GL_CCW);
            break;
        }
    }

    constexpr glm::vec3 COLOR_OIL_BLUE = glm::vec3(
        21.0f / 255.0f,
        45.0f / 255.0f,
        50.0f / 255.0f);
    constexpr glm::vec3 DEFAULT_CLEAR_COLOR = COLOR_OIL_BLUE;
    //the "clear" color for the frame buffer
    //glClearColor(DEFAULT_CLEAR_COLOR.r, DEFAULT_CLEAR_COLOR.g, DEFAULT_CLEAR_COLOR.b, 1.0f);
    glClearColor(DEFAULT_CLEAR_COLOR.r, DEFAULT_CLEAR_COLOR.g, DEFAULT_CLEAR_COLOR.b, 0.5f);

    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //// we bind the depth map FBO
    //glBindFramebuffer(GL_FRAMEBUFFER, rendererContext.depthMapFBO);
    //glFramebufferTexture2D(
    //    GL_FRAMEBUFFER
    //    , GL_DEPTH_ATTACHMENT
    //    , GL_TEXTURE_2D
    //    , rendererContext.depthMap
    //    , 0);
    // we set that we are not calculating nor saving color data
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void UpdateEngine(
    DropPlatformCalls* platformCalls
    , EngineMemory* engineMemory
    , GameProcAdresses* gameCalls
) {
    // Get the updated engine calls (this could be conditional, only if DLL updated)
    gDropEngineCalls.engineCall = TestEngineCall;

    EngineState* engineState = (EngineState*)(engineMemory->persistentMemory);
    assert(engineState);

    // THIS SHOULD BE DONE ON REATTACH
    // Reattach GLFW
    //glfwSetLib(engineState->windowHandle->glfwLibrary);
    //std::cout << "engineState->windowHandle->glfwLibrary = " 
    //    << (uintptr_t)engineState->windowHandle->glfwLibrary << std::endl;
    //glfwSetWindowUserPointer(
    //    engineState->windowHandle->glfwWindow
    //    , &engineState->windowHandle
    //);
    // Reattach ImGui
    UI::HotReloadContextReset(
        engineState->uiContext
        , &engineState->imGuiAllocator
        , engineState->windowHandle
    );
    // -------------

    // Get time from glfwGetTime
    const float deltaTime = (float)glfwGetTime();
    std::cout << "Time: " << deltaTime << std::endl;

    OnUpdate(engineState->windowHandle);

    if (gameCalls->UpdateGame)
    {
        // call UpdateGame
        gameCalls->UpdateGame(
            deltaTime
            , &gDropEngineCalls
        );
    }

    UI::UpdateUI Update;
    // Render UI
    UI::UpdateAndRenderUI(
        &Update
        , engineState->windowHandle
    );

    // Swap buffers
    OnEndFrame(engineState->windowHandle);
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
        std::cout << "[" << __FILE__ << "] " << "DLL_PROCESS_ATTACH" << std::endl;
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        std::cout << "[" << __FILE__ << "] " "DLL_THREAD_ATTACH" << std::endl;
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        std::cout << "[" << __FILE__ << "] " "DLL_THREAD_DETACH" << std::endl;
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        std::cout << "[" << __FILE__ << "] " "DLL_PROCESS_DETACH" << std::endl;
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#endif // DROP_PLATFORM_WINDOWS