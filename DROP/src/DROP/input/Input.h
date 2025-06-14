#pragma once

#include "KeyCodes.h"

#include <glm/glm.hpp>
#include "../dependencies/GLFW/include/GLFW/glfw3.h"

namespace Drop
{
	class Input
	{
	public: 
		static GLFWwindow* m_ActiveWindowHandle;
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsKeyRepeated(KeyCode keycode);
		static bool IsKeyDown(KeyCode keycode);
		static bool IsKeyReleased(KeyCode keycode);
		static bool IsMouseButtonDown(MouseButton button);

		static glm::vec2 GetMousePosition();

		static void SetCursorMode(CursorMode mode);
	};
}
