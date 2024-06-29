#pragma once

#include "Game.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <memory>
#include <vector>
#include <string>
#include <functional>

struct GLFWwindow;

namespace DROP
{

	struct GameEngineSpecification
	{
		std::string Name = "Walnut App";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class GameEngine
	{
	public:
		GameEngine(const GameEngineSpecification& specification = GameEngineSpecification());
		~GameEngine();

		static GameEngine& Get();

		void Run();

		template<typename T>
		void PushGame()
		{
			static_assert(std::is_base_of<Game, T>::value, "Pushed type is not subclass of Game!");
			m_GamesStack.emplace_back(std::make_shared<T>())->OnAttach();
		}

		void PushGame(const std::shared_ptr<Game>& game) { m_GamesStack.emplace_back(game); game->OnAttach(); }

		void Close();

		float GetTime();
		GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }

	private:
		void Init();
		void Shutdown();
	private:
		GameEngineSpecification m_Specification;
		GLFWwindow* m_WindowHandle = nullptr;
		bool m_Running = false;

		float m_TimeStep = 0.0f;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		std::vector<std::shared_ptr<Game>> m_GamesStack;
		std::function<void()> m_MenubarCallback;
	};

	// Implemented by CLIENT
	GameEngine* CreateGameEngine(int argc, char** argv);
}