#pragma once

#include "Game.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "../../dependencies/IMGUI/include/imgui/imgui.h"
#include "../../dependencies/IMGUI/include/imgui/imgui_impl_glfw.h"
#include "../../dependencies/IMGUI/include/imgui/imgui_impl_opengl3.h"

//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_glfw.h"
//#include "imgui/imgui_impl_opengl3.h"

#include "physics/physicsEngine.h"
#include "rendering/renderer.h"

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

		static GameEngine& Get();

		void Run();
		void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }

		//template<typename T>
		//void PushGame()
		//{
		//	static_assert(std::is_base_of<Game, T>::value, "Pushed type is not subclass of Game!");
		//	m_GamesStack.emplace_back(std::make_shared<T>())->OnAttach();
		//}

		template<typename T>
		void SetGame()
		{
			static_assert(std::is_base_of<Game, T>::value, "Pushed type is not subclass of Game!");
			m_Game = std::make_shared<T>();
			m_Game->OnAttach();
		}

		//void PushGame(const std::shared_ptr<Game>& game) { m_GamesStack.emplace_back(game); game->OnAttach(); }
		void SetGame(const std::shared_ptr<Game>& game) { m_Game = game; game->OnAttach(); }

		void Close();

		float GetTime();
		GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }
		static GameEngine* GetInstance();

		static int LoadTexture(const char* path);
		void SetupShader(int program);
		void PrintCurrentShader(int subroutine) const;
	public:
		SceneGraph m_SceneGraph = (RESERVE);

		PhysicsEngine m_PhysicsEngine;
		std::vector<PhysicsObject> m_PhysicsObjects;

		Renderer m_Renderer;
		std::vector<RenderableObject> m_RendereableObjects;
		std::vector<std::string> m_Shaders;
		std::vector<int> m_TextureIds;
		std::vector<Model> m_Models;
		std::vector<Material> m_Materials;

		std::unordered_map<uint32_t, VgMath::Transform> m_CumulatedTransforms;
		std::unordered_map<uint32_t, glm::mat4> m_ModelMatrices;


	private:
		void Init();
		void Shutdown();
	private:
		GameEngineSpecification m_Specification;
		GLFWwindow* m_WindowHandle = nullptr;
		bool m_Running = false;

		float m_DeltaTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		//std::vector<std::shared_ptr<Game>> m_GamesStack;
		std::shared_ptr<Game> m_Game;
		std::function<void()> m_MenubarCallback;

	};

	// Implemented by CLIENT
	GameEngine* CreateGameEngine(int argc, char** argv);
}