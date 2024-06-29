#include "Input.h"

using namespace DROP;

GLFWwindow* Input::m_WindowHandle = nullptr;

bool Input::IsKeyDown(KeyCode keycode)
{
	GLFWwindow* windowHandle = m_WindowHandle;
	int state = glfwGetKey(windowHandle, (int)keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonDown(MouseButton button)
{
	GLFWwindow* windowHandle = m_WindowHandle;
	int state = glfwGetMouseButton(windowHandle, (int)button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	GLFWwindow* windowHandle = m_WindowHandle;

	double x, y;
	glfwGetCursorPos(windowHandle, &x, &y);
	return { (float)x, (float)y };
}

void Input::SetCursorMode(CursorMode mode)
{
	GLFWwindow* windowHandle = m_WindowHandle;
	glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}

