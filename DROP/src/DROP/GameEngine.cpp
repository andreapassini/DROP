#include "GameEngine.h"

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort

#include <iostream>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtx/string_cast.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "input/Input.h"

// we include the library for images loading
#define STB_IMAGE_IMPLEMENTATION
//#include "../../dependencies/stb_img/include/stb_image/stb_image.h"
#include <stb_image/stb_image.h>
#define stringify( name ) #name

extern bool g_GameEngineRunning;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static Drop::GameEngine* s_Instance = nullptr;

namespace Drop
{
	GameEngine::GameEngine(const GameEngineSpecification& specification)
		: m_Specification(specification),
		m_PhysicsEngine(0.0, 123),
		m_Renderer(specification.Width, specification.Height)
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

	void GameEngine::Close()
	{
		m_Running = false;
	}

	float GameEngine::GetTime()
	{
		return (float)glfwGetTime();
	}

	GameEngine* GameEngine::GetInstance()
	{
		return s_Instance;
	}

	void GameEngine::Init()
	{
		// to be removed
		m_WindowHandle = m_Renderer.m_Window;
		Input::m_WindowHandle = m_Renderer.m_Window;

		// OpenGL Setup
//		// Setup GLFW window
//		// Initialization of OpenGL context using GLFW
//		glfwInit();
//		// We set OpenGL specifications required for this application
//		// In this case: 4.1 Core
//		// If not supported by your graphics HW, the context will not be created and the application will close
//		// N.B.) creating GLAD code to load extensions, try to take into account the specifications and any extensions you want to use,
//		// in relation also to the values indicated in these GLFW commands
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//		// we set if the window is resizable
//		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   // If u want to resize it, u have to change also the camera
//		// we create the application's window
//		m_WindowHandle = glfwCreateWindow(m_Specification.Width, m_Specification.Height, "Drop", nullptr, nullptr);
//		if (!m_WindowHandle)
//		{
//			std::cout << "Failed to create GLFW window" << std::endl;
//			glfwTerminate();
//			assert(false);
//			//return -1;
//			return;
//		}
//		glfwMakeContextCurrent(m_WindowHandle);
//
//#ifdef UNLOCK_FRAMERTE
//		glfwSwapInterval(0); // Unlock framerate
//#endif // UNLOCK_FRAMERTE
//
//		// GLAD tries to load the context set by GLFW
//		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//		{
//			std::cout << "Failed to initialize OpenGL context" << std::endl;
//			assert(false);
//			//return -1;
//			return;
//		}

		// ImGui SETUP
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_Renderer.m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 410");


	}

	void GameEngine::Run()
	{
		m_Running = true;

		//SetupShader(m_Game->m_ShadowShader.Program);
		SetupShader(m_Game->m_LightShader.Program);
		PrintCurrentShader(m_Game->m_CurrentSubroutine);

		// Main loop
		while (!glfwWindowShouldClose(m_WindowHandle))
		{
			// we determine the time passed from the beginning
			// and we calculate time difference between current frame rendering and the previous one
			float currentTime = GetTime();
			m_DeltaTime = currentTime - m_LastFrameTime;
			m_LastFrameTime = currentTime;

			// Poll and handle events (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();

			m_Game->m_Camera.OnUpdate(m_DeltaTime);
			m_Game->m_Camera.OnResize(m_Renderer.m_Width, m_Renderer.m_Height);
			m_Game->OnUpdate(m_DeltaTime);

			uint32_t physIter = 0;
			while (!m_PhysicsEngine.m_IsPaused && currentTime > m_PhysicsEngine.GetVirtualTIme())
			{
				//physicsEngine.AddForceToAll(glm::vec3(0.0f, gravity, 0.0f));

				//for (auto& game : m_GamesStack)
				//	game->OnFixedUpdate(m_PhysicsEngine.GetVirtualTIme());

				m_Game->OnFixedUpdate(m_PhysicsEngine.GetVirtualTIme());

				m_PhysicsEngine.PhysicsStep();

				physIter++;

				if (physIter > m_PhysicsEngine.maxIter)
				{
					std::cout << "Physics Simulation lagging " << std::endl;
					m_PhysicsEngine.SynchVirtualTime(currentTime);
					break;
				}
			}

			// Calculate world transform every time the transform in changed
			m_SceneGraph.CalculateWorldTransforms(
				m_CumulatedTransforms,
				m_ModelMatrices
			);

			// render your GUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				// Set ImGui style

				//for (auto& game : m_GamesStack)
				//	game->OnUIRender();

				m_Game->OnUIRender();

				//ImGui::End();
			}

			m_Renderer.RenderScene(
				m_RendereableObjects,
				m_TextureIds,
				m_Models,
				m_Materials,
				m_ModelMatrices,
				m_Game->m_Camera.GetViewMatrix(),
				m_Game->m_Camera.GetProjectionMatrix(),
				m_Game->m_LightDir,
				&(m_Game->m_ShadowShader),
				&(m_Game->m_LightShader),
				m_Renderer.m_DepthMapFBO,
				m_Renderer.m_DepthMap,
				m_Game->m_Wireframe,
				m_Game->m_CurrentSubroutine,
				&(m_Shaders),
				m_Renderer.m_Width,
				m_Renderer.m_Height
			);

			// Rendering imgui
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Swapping back and front buffers
			glfwSwapBuffers(m_WindowHandle);
		}
	}

	void GameEngine::Shutdown()
	{
		m_Game->OnDetach();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();

		g_GameEngineRunning = false;
	}

	int GameEngine::LoadTexture(const char* path)
	{
		GLuint textureImage = 0;
		int w, h, channels;
		unsigned char* image;
		image = stbi_load(path, &w, &h, &channels, STBI_rgb);

		if (image == nullptr)
			std::cout << "Failed to load texture!" << std::endl;

		glGenTextures(1, &textureImage);
		glBindTexture(GL_TEXTURE_2D, textureImage);
		// 3 channels = RGB ; 4 channel = RGBA
		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		// we set how to consider UVs outside [0,1] range
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// we set the filtering for minification and magnification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

		// we free the memory once we have created an OpenGL texture
		stbi_image_free(image);

		// we set the binding to 0 once we have finished
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureImage;
	}

	void GameEngine::SetupShader(int program)
	{
		int maxSub, maxSubU, countActiveSU;
		GLchar name[256];
		int len, numCompS;

		// global parameters about the Subroutines parameters of the system
		glGetIntegerv(GL_MAX_SUBROUTINES, &maxSub);
		glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubU);
		std::cout << "Max Subroutines:" << maxSub << " - Max Subroutine Uniforms:" << maxSubU << std::endl;

		// get the number of Subroutine uniforms (only for the Fragment shader, due to the nature of the exercise)
		// it is possible to add similar calls also for the Vertex shader
		glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &countActiveSU);

		// print info for every Subroutine uniform
		for (int i = 0; i < countActiveSU; i++)
		{

			// get the name of the Subroutine uniform (in this example, we have only one)
			glGetActiveSubroutineUniformName(program, GL_FRAGMENT_SHADER, i, 256, &len, name);
			// print index and name of the Subroutine uniform
			std::cout << "Subroutine Uniform: " << i << " - name: " << name << std::endl;

			// get the number of subroutines
			glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);

			// get the indices of the active subroutines info and write into the array s
			int* s = new int[numCompS];
			glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_COMPATIBLE_SUBROUTINES, s);
			std::cout << "Compatible Subroutines:" << std::endl;

			// for each index, get the name of the subroutines, print info, and save the name in the shaders vector
			for (int j = 0; j < numCompS; ++j)
			{
				glGetActiveSubroutineName(program, GL_FRAGMENT_SHADER, s[j], 256, &len, name);
				std::cout << "\t" << s[j] << " - " << name << "\n";
				m_Shaders.push_back(name);
			}
			std::cout << std::endl;

			delete[] s;
		}
	}

	void GameEngine::PrintCurrentShader(int subroutine) const
	{
		std::cout << "Current shader subroutine: " << m_Shaders[subroutine] << std::endl;
	}
}
