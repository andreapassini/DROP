#pragma once

#include "KeyCodes.h"

#define GLFW_INCLUDE_NONE
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Drop
{
	class Input
	{
	public: 
		static GLFWwindow* m_WindowHandle;
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
