#include "GameEngine.h"

#include <glad/glad.h>

// GLFW library to create window and to manage I/O
#include <glfw/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

extern bool g_GameEngineRunning;

static DROP::GameEngine* s_Instance = nullptr;

namespace DROP
{
	GameEngine::GameEngine(const GameEngineSpecification& specification)
		: m_Specification(specification)
	{
		s_Instance = this;

		Init();
	}

	GameEngine::~GameEngine()
	{
		Shutdown();

		s_Instance = nullptr;
	}

	GameEngine& GameEngine::Get()
	{
		return *s_Instance;
	}

	void GameEngine::Init()
	{
		// Setup GLFW window

		// Setup ImGui style
	}

	void GameEngine::Shutdown()
	{
		for (auto& game : m_GamesStack)
			game->OnDetach();

		m_GamesStack.clear();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();

		g_GameEngineRunning = false;
	}

	void GameEngine::Run()
	{
		m_Running = true;

		// Main loop
		while (!glfwWindowShouldClose(m_WindowHandle))
		{
			// Poll and handle events (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();

			for (auto& game : m_GamesStack)
				game->OnUpdate(m_TimeStep);

			// render your GUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			{
				// Set ImGui style

				for (auto& game : m_GamesStack)
					game->OnUIRender();

				ImGui::End();
			}

			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Render Pass

			// Swapping back and front buffers
			glfwSwapBuffers(m_WindowHandle);
		}
	}


}