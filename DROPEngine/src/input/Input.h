#pragma once

#include "KeyCodes.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Drop::Input
{
	static bool IsKeyPressed(KeyCode keycode, GLFWwindow* InWindowHandle);
	static bool IsKeyRepeated(KeyCode keycode, GLFWwindow* InWindowHandle);
	static bool IsKeyDown(KeyCode keycode, GLFWwindow* InWindowHandle);
	static bool IsKeyReleased(KeyCode keycode, GLFWwindow* InWindowHandle);
	static bool IsMouseButtonDown(MouseButton button, GLFWwindow* InWindowHandle);

	static glm::vec2 GetMousePosition(GLFWwindow* InWindowHandle);

	static void SetCursorMode(CursorMode mode, GLFWwindow* InWindowHandle);
}
