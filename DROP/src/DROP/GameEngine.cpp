#include "GameEngine.h"


#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>

#define GLFW_INCLUDE_NONE
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
//#include "../../dependencies/stb_img/include/stb_image/stb_image.h"
#include <stb_image/stb_image.h>
#define stringify( name ) #name

#include "DROP/ECS/beecs.h"
#include "rendering/RenderingSystem.h"

extern bool g_GameEngineRunning;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static Drop::GameEngine* s_Instance = nullptr;
static bseecs::ECS g_ECS{};

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
		m_WindowHandle = std::unique_ptr<Window>(Window::Create());
		Input::m_WindowHandle = (GLFWwindow*)m_WindowHandle->GetNativeWindow();

		m_Renderer.Init((GLFWwindow*)m_WindowHandle->GetNativeWindow(), m_renderingContext);

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

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_WindowHandle->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");

		// Testing ECS
		g_ECS.RegisterComponent<Transform>();
		g_ECS.RegisterComponent<Camera, Transform>();
		g_ECS.RegisterComponent<Model, Transform>();
		g_ECS.RegisterComponent<ParticleEmitter, Transform>();
		g_ECS.RegisterComponent<Billboard, Transform>();
		g_ECS.RegisterComponent<PhysicsObject>();
	}

	void GameEngine::Run()
	{
		m_Running = true;

		Shader billboardShader = Shader(
			"..\\DROP\\src\\DROP\\shaders\\billboard.vert",
			"..\\DROP\\src\\DROP\\shaders\\billboard.geom",
			"..\\DROP\\src\\DROP\\shaders\\billboard.frag");

		SceneContext sceneContext{
			m_Game->m_Camera.GetViewMatrix()
			, m_Game->m_Camera.GetProjectionMatrix()
			, m_Game->m_LightDir
			, m_TextureIds
			, m_Models
			, m_Materials
			, m_WindowHandle->GetWidth()
			, m_WindowHandle->GetHeight()
			, m_Game->m_Wireframe
		};	
		
		ParticleEmitter particleEmitter;
		particleEmitter.spawningValues.lifeTime = 5.0f;
		Transform spawningSurfaceTransform;
		spawningSurfaceTransform.m_Translate = (0.0f, 1.0f, 0.0f);
		particleEmitter.spawningValues.endsize = 0.0f;
		particleEmitter.spawningValues.spawningSurface.m_Size.x = 5.0f;
		particleEmitter.spawningValues.spawningSurface.m_Size.y = 10.0f;
		particleEmitter.spawningValues.spawningSurface.m_Transform = &spawningSurfaceTransform;
		float waitTime = 0.0f;
		float spawnDelay = 2.5f;

		// Main loop
		while (!m_WindowHandle->IsShouldClose())
		{
			// we determine the time passed from the beginning
			// and we calculate time difference between current frame rendering and the previous one
			float currentTime = GetTime();
			m_DeltaTime = currentTime - m_LastFrameTime;
			m_LastFrameTime = currentTime;

			// pooling events
			m_WindowHandle->OnUpdate();

			m_Game->m_Camera.OnUpdate(m_DeltaTime);
			m_Game->m_Camera.OnResize(m_WindowHandle->GetWidth(), m_WindowHandle->GetHeight());

			sceneContext.height = m_WindowHandle->GetHeight();
			sceneContext.width = m_WindowHandle->GetWidth();
			sceneContext.wireframe = m_Game->m_Wireframe;

			// Update
			m_Game->OnUpdate(m_DeltaTime);

			// Fixed Update
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

			if (waitTime < currentTime) {
				waitTime = currentTime + spawnDelay;
				//particleEmitter.spawningValues.position.x += 1.0f;
				EmitParticles(particleEmitter);
			}
			UpdateParticles(
				particleEmitter.particles
				, particleEmitter.numberOfParticles
				, m_DeltaTime
			);

			// Calculate world transform every time the transform in changed
			m_SceneGraph.CalculateWorldTransforms(
				m_CumulatedTransforms
			);

			// RENDERER
			RenderingSystem::Update(g_ECS, m_DeltaTime);

			// Move this function into the RenderingSystem::Update function
			m_Renderer.RenderScene(
				sceneContext
				, m_renderingContext
				, m_RendereableObjects
				, m_CumulatedTransforms
				, &(m_Game->m_ShadowShader)
				, &(m_Game->m_LightShader)
			);

			//// Draw Normal as vectors
			//m_Renderer.RenderScene(
			//	sceneContext,
			//	m_RendereableObjects,
			//	m_CumulatedTransforms,
			//	&(displayNormalShader)
			//);

			m_Renderer.RenderParticles(
				sceneContext
				, particleEmitter.particles
				, particleEmitter.numberOfParticles
				, &billboardShader
			);

			if (m_DrawDebug)
			{
				m_Renderer.DrawDebug(
					sceneContext, 
					&(m_Game->m_DebugShader),
					m_DrawableLines
				);

				std::vector<ParticleEmitter> ParticleEmitters;
				ParticleEmitters.push_back(particleEmitter);
				m_Renderer.DrawParticleEmitterSurface(
					sceneContext
					, &(m_Game->m_EmptyQuadShader),
					ParticleEmitters
				);
			}

			// render your GUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			m_Game->OnUIRender();
			ImGui::End();		

			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)m_WindowHandle->GetWidth(), (float)m_WindowHandle->GetHeight());

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
			m_WindowHandle->OnEndFrame();
		}
	}

	void GameEngine::Shutdown()
	{
		m_Game->OnDetach();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		m_WindowHandle.reset(); // delete the object, leaving m_WindowHandle empty

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
}
