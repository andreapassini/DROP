#pragma once

#include "Game.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "physics/physicsEngine.h"
#include "rendering/renderer.h"

#include "imgui.h"
#include "utils/Window.h"

#include "DROP/ECS/beecs.h"


struct GLFWwindow;


namespace Drop
{

	struct GameEngineSpecification
	{
		std::string Name = "Drop Engine";
		int Width = 1600;
		int Height = 900;
	};

	class GameEngine
	{
	public:
		GameEngine(const GameEngineSpecification& specification = GameEngineSpecification());
		~GameEngine();


		void Run();
		void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }

		template<typename T>
		void SetGame()
		{
			static_assert(std::is_base_of<Game, T>::value, "Pushed type is not subclass of Game!");
			m_Game = std::make_shared<T>();
			m_Game->OnAttach();
		}

		void SetGame(const std::shared_ptr<Game>& game) { m_Game = game; game->OnAttach(); }

		void Close();

		inline static GameEngine& Get();
		inline Window& GetWindowHandle() const { return *m_WindowHandle; }
		static GameEngine* GetInstance();

		float GetTime();
		static int LoadTexture(const char* path);

		inline const bool IsDrawDebug() const { return m_DrawDebug;  }
		inline void SetDrawDebug(bool debug) { m_DrawDebug = debug; }

	public:
		SceneGraph m_SceneGraph = (RESERVE);

		PhysicsEngine m_PhysicsEngine;
		std::vector<PhysicsObject> m_PhysicsObjects;

		Renderer m_Renderer;
		RendererContext m_renderingContext;
		std::vector<RenderableObject> m_RendereableObjects;
		std::vector<int> m_TextureIds;
		std::vector<Model> m_Models;
		std::vector<Material> m_Materials;
		std::vector<Line> m_DrawableLines;

		std::unordered_map<uint32_t, VgMath::Transform> m_CumulatedTransforms;

		// GPU Particles


	private:
		void Init();
		void Shutdown();
	private:
		GameEngineSpecification m_Specification;
		std::unique_ptr<Window> m_WindowHandle = nullptr;
		bool m_Running = false;

		float m_DeltaTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		std::shared_ptr<Game> m_Game;
		std::function<void()> m_MenubarCallback;

		bool m_DrawDebug = true;
	};

	// Implemented by CLIENT
	GameEngine* CreateGameEngine(int argc, char** argv);
}