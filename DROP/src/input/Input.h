#pragma once

#include "KeyCodes.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace DROP
{
	class Input
	{
	public: 
		static GLFWwindow* m_WindowHandle;
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsKeyRepeated(KeyCode keycode);
		static bool IsMouseButtonDown(MouseButton button);

		static glm::vec2 GetMousePosition();

		static void SetCursorMode(CursorMode mode);
	};
}
