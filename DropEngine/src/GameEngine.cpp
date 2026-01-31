#include "GameEngine.h"

#include <iostream>

#include "Utils/Log.h"
#include "Window/Window.h"
#include "DROPGame.h"

using namespace Drop;

void StartEngine()
{
	Drop::Log::Init();
	LOG_CORE_WARN("Initialized Log!");

	LOG_CORE_INFO("Drop Engine starting");

	// to be removed
	Window* m_WindowHandle = Window::Create();
	//Input::m_WindowHandle = (GLFWwindow*)m_WindowHandle->GetNativeWindow();


}

DropEngineCalls gDropEngineCalls;

void UpdateEngine(
	DropPlatformCalls* platformCalls
	, GameProcAdresses* gameCalls
) {
	// Get time from glfwGetTime
	const float deltaTime = (float)glfwGetTime();
	std::cout << "Time: " << deltaTime << std::endl;

	// call UpdateGame
	gameCalls->UpdateGame(
		deltaTime
		, &gDropEngineCalls
	);
}