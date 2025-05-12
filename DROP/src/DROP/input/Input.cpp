#include "Input.h"

using namespace Drop;

GLFWwindow* Input::m_ActiveWindowHandle = nullptr;

bool Input::IsKeyPressed(KeyCode keycode)
{
	GLFWwindow* windowHandle = m_ActiveWindowHandle;
	int state = glfwGetKey(windowHandle, (int)keycode);
	return state == GLFW_PRESS;
}

bool Input::IsKeyRepeated(KeyCode keycode)
{
	GLFWwindow* windowHandle = m_ActiveWindowHandle;
	int state = glfwGetKey(windowHandle, (int)keycode);
	return state == GLFW_REPEAT;
}

// Pressed or Repeated
bool Input::IsKeyDown(KeyCode keycode)
{
	GLFWwindow* windowHandle = m_ActiveWindowHandle;
	int state = glfwGetKey(windowHandle, (int)keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Drop::Input::IsKeyReleased(KeyCode keycode)
{
	GLFWwindow* windowHandle = m_ActiveWindowHandle;
	int state = glfwGetKey(windowHandle, (int)keycode);
	return state == GLFW_RELEASE;
}



bool Input::IsMouseButtonDown(MouseButton button)
{
	GLFWwindow* windowHandle = m_ActiveWindowHandle;
	int state = glfwGetMouseButton(windowHandle, (int)button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	GLFWwindow* windowHandle = m_ActiveWindowHandle;

	double x, y;
	glfwGetCursorPos(windowHandle, &x, &y);
	return { (float)x, (float)y };
}

void Input::SetCursorMode(CursorMode mode)
{
	GLFWwindow* windowHandle = m_ActiveWindowHandle;
	glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}

