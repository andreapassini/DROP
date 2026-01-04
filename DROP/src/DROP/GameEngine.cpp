#include "GameEngine.h"

#include "Drop/Utils/Log.h"
#include "Window/Window.h"

using namespace Drop;

namespace GameEngine {

void GameEngine::StartEngine()
{
	Drop::Log::Init();
	LOG_CORE_WARN("Initialized Log!");

	LOG_CORE_INFO("Drop Engine starting");

	// to be removed
	Window* m_WindowHandle = Window::Create();
	//Input::m_WindowHandle = (GLFWwindow*)m_WindowHandle->GetNativeWindow();


}

}