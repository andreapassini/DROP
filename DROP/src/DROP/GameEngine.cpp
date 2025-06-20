#include "GameEngine.h"


#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>

#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <glm/gtx/string_cast.hpp>

#include "input/Input.h"
#include "utils/Log.h"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

// Emedded font
#include "ImGui/Roboto-Regular.embed"

// we include the library for images loading
#define STB_IMAGE_IMPLEMENTATION
#include "../dependencies/stb_img/include/stb_image/stb_image.h"
#define stringify( name ) #name

#include "DROP/ECS/beecs.h"
#include "rendering/RenderingSystem.h"
#include "particles/particleSystem.h"
#include "sceneGraph/scene.h"

extern bool g_GameEngineRunning;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static Drop::GameEngine* s_Instance = nullptr;
Scene* Drop::GameEngine::g_activeScene = nullptr;

namespace Drop
{
	GameEngine::GameEngine(const GameEngineSpecification& specification)
		: m_Specification(specification),
		m_PhysicsEngine(0.0, 123)
	{
		assert(!s_Instance, "Game Engine already Exists");
		s_Instance = this;

		Init();
	}

	GameEngine::~GameEngine()
	{
		Shutdown();

		s_Instance = nullptr;
	}

	void GameEngine::Close()
	{
		m_Running = false;
	}

	inline GameEngine& GameEngine::Get()
	{
		return *s_Instance;
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
		Drop::Log::Init();
		LOG_CORE_WARN("Initialized Log!");

		LOG_CORE_INFO("Drop Engine starting");

		// to be removed
		m_ActiveWindowHandle = std::unique_ptr<Window>(Window::Create());
		Input::m_ActiveWindowHandle = (GLFWwindow*)m_ActiveWindowHandle->GetNativeWindow();

		// ImGui SETUP
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_ActiveWindowHandle->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");

		g_activeScene = new Scene();

		g_activeScene->ecs.RegisterSingletonComponent<SceneContext>(arena);
		g_activeScene->ecs.RegisterSingletonComponent<RendererContext>(arena);

		//g_activeScene->ecs.RegisterSingletonComponent<SceneContext>(arena);
		//g_activeScene->ecs.RegisterSingletonComponent<RendererContext>(arena);

		RendererContext* renderContext = &g_activeScene->ecs.GetSingletonComponent<RendererContext>();

		renderContext->backFaceCulling = false;

		// #TODO Fix this, since meshes are clockwise and particles(billboards) counterclockwise
		//renderContext->backFaceCulling = true;
		//renderContext->clockWiseOrder = EClockWise::CLOCKWISE;

		Renderer::Init(
			(GLFWwindow*)m_ActiveWindowHandle->GetNativeWindow()
			, *renderContext
		);

		//Scene currScene;
		//currScene.sceneName = "A scene";
		//currScene.ecs = g_activeScene->ecs;

		//std::string scenePath = "FirstScene.drop";
		//SceneSerializer::SerializeSceneAsText(
		//	scenePath
		//	, &currScene
		//);
	}

	void GameEngine::Run()
	{
		m_Running = true;

		SceneContext& sceneContext = g_activeScene->ecs.GetSingletonComponent<SceneContext>();

		//sceneContext.view = m_Game->m_Camera.GetViewMatrix();
		//sceneContext.projection = m_Game->m_Camera.GetProjectionMatrix();
		sceneContext.camera = &m_Game->m_Camera;

		sceneContext.lightDir = m_Game->m_LightDir;
		sceneContext.lightSpaceMatrix = m_Game->m_lightSpaceMatrix;
		
		sceneContext.models = &m_Models;
		sceneContext.materials = &m_Materials;
		sceneContext.textuers = &m_TextureIds;
		sceneContext.wireframe = m_Game->m_Wireframe;
	
		RendererContext& renderContext = g_activeScene->ecs.GetSingletonComponent<RendererContext>();

		renderContext.window = Input::m_ActiveWindowHandle;

		// Main loop
		while (!m_ActiveWindowHandle->IsShouldClose())
		{
			// we determine the time passed from the beginning
			// and we calculate time difference between current frame rendering and the previous one
			m_CurrentTime = GetTime();
			m_DeltaTime = m_CurrentTime - m_LastFrameTime;
			m_LastFrameTime = m_CurrentTime;

			// pooling events
			m_ActiveWindowHandle->OnUpdate();

			m_Game->m_Camera.OnUpdate(m_DeltaTime);
			m_Game->m_Camera.OnResize(m_ActiveWindowHandle->GetWidth(), m_ActiveWindowHandle->GetHeight());

			sceneContext.height = m_ActiveWindowHandle->GetHeight();
			sceneContext.width = m_ActiveWindowHandle->GetWidth();
			sceneContext.wireframe = m_Game->m_Wireframe;

			// Update
			m_Game->OnUpdate(m_DeltaTime);

			// Fixed Update
			uint32_t physIter = 0;
			while (!m_PhysicsEngine.m_IsPaused && m_CurrentTime > m_PhysicsEngine.GetVirtualTIme())
			{
				//physicsEngine.AddForceToAll(glm::vec3(0.0f, gravity, 0.0f));

				//for (auto& game : m_GamesStack)
				//	game->OnFixedUpdate(m_PhysicsEngine.GetVirtualTIme());

				m_Game->OnFixedUpdate(m_PhysicsEngine.GetVirtualTIme());

				m_PhysicsEngine.PhysicsStep();

				if (!m_PauseParticleUpdate) {
					ParticleSystem::UpdatePB(g_activeScene->ecs, m_DeltaTime);
				}

				physIter++;

				if (physIter > m_PhysicsEngine.maxIter)
				{
					LOG_CORE_WARN("Physics Simulation lagging");
					m_PhysicsEngine.SynchVirtualTime(m_CurrentTime);
					break;
				}
			}

			if (!m_PauseParticleUpdate)
			{
				ParticleSystem::Update(g_activeScene->ecs, m_DeltaTime);
			}

			SceneGraph::CalculateWorldTransforms(g_activeScene->ecs);

			RenderingSystem::Update(g_activeScene->ecs, m_DeltaTime);

			// render your GUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			m_Game->OnUIRender();
			ImGui::End();		

			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)m_ActiveWindowHandle->GetWidth(), (float)m_ActiveWindowHandle->GetHeight());

			// Rendering ImGui
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			// Swapping back and front buffers
			m_ActiveWindowHandle->OnEndFrame();
		}
	}

	void GameEngine::Shutdown()
	{
		m_Game->OnDetach();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		m_ActiveWindowHandle.reset(); // delete the object, leaving m_WindowHandle empty

		g_GameEngineRunning = false;
	}

	int GameEngine::LoadTexture(const char* path)
	{
		LOG_CORE_TRACE(path);

		GLuint textureImage = 0;
		int w, h, channels;
		unsigned char* image;
		//image = stbi_load(path, &w, &h, &channels, STBI_rgb);
		//image = stbi_load(path, &w, &h, &channels, STBI_rgb_alpha);
		image = stbi_load(path, &w, &h, &channels, STBI_default); //    STBI_default = 0, // only used for desired_channels

		if (image == nullptr)
			LOG_CORE_ERROR("Failed to load texture: {0}", path);

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

	//Scene& GameEngine::InitializeScene(const std::string& sceneName)
	//{
	//	g_activeScene = new Scene;
	//	g_activeScene->sceneName = sceneName;

	//	// we assume that we want to use the renderer
	//	//g_activeScene->ecs.RegisterSingletonComponent<>();

	//	return *g_activeScene;
	//}
}
