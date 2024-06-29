#include "GameEngine.h"

#include <glad/glad.h>

// GLFW library to create window and to manage I/O
#include <glfw/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>

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
		// Initialization of OpenGL context using GLFW
		glfwInit();
		// We set OpenGL specifications required for this application
		// In this case: 4.1 Core
		// If not supported by your graphics HW, the context will not be created and the application will close
		// N.B.) creating GLAD code to load extensions, try to take into account the specifications and any extensions you want to use,
		// in relation also to the values indicated in these GLFW commands
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		// we set if the window is resizable
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   // If u want to resize it, u have to change also the camera
		// we create the application's window
		m_WindowHandle = glfwCreateWindow(m_Width, m_Height, "DROP", nullptr, nullptr);
		if (!m_WindowHandle)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			assert(false);
			//return -1;
			return;
		}
		glfwMakeContextCurrent(m_WindowHandle);

#ifdef UNLOCK_FRAMERTE
		glfwSwapInterval(0); // Unlock framerate
#endif // UNLOCK_FRAMERTE

		// GLAD tries to load the context set by GLFW
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			assert(false);
			//return -1;
			return;
		}


		// OpenGL Setup


		// ImGui SETUP
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
		ImGui_ImplOpenGL3_Init("#version 410");

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