#include "Input.h"

#include "Types/Types.h"

using namespace Drop;

bool Input::IsKeyPressed(KeyCode keycode, GLFWwindow* InWindowHandle)
{
	int32 state = 0;
	if (!InWindowHandle) 
	{
		return state;
	}

	state = glfwGetKey(InWindowHandle, (int32)keycode);
	return state == GLFW_PRESS;
}

bool Input::IsKeyRepeated(KeyCode keycode, GLFWwindow* InWindowHandle)
{
	int32 state = 0;
	if (!InWindowHandle) 
	{
		return state;
	}

	state = glfwGetKey(InWindowHandle, (int32)keycode);
	return state == GLFW_REPEAT;
}

// Pressed or Repeated
bool Input::IsKeyDown(KeyCode keycode, GLFWwindow* InWindowHandle)
{
	int32 state = 0;
	if (!InWindowHandle) 
	{
		return state;
	}

	state = glfwGetKey(InWindowHandle, (int32)keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Drop::Input::IsKeyReleased(KeyCode keycode, GLFWwindow* InWindowHandle)
{
	int32 state = 0;
	if (!InWindowHandle) 
	{
		return state;
	}

	state = glfwGetKey(InWindowHandle, (int32)keycode);
	return state == GLFW_RELEASE;
}



bool Input::IsMouseButtonDown(MouseButton button, GLFWwindow* InWindowHandle)
{
	int32 state = 0;
	if (!InWindowHandle) 
	{
		return state;
	}

	state = glfwGetMouseButton(InWindowHandle, (int32)button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition(GLFWwindow* InWindowHandle)
{
	if (!InWindowHandle) {
		return { 0.0f, 0.0f };
	}

	double x, y;
	glfwGetCursorPos(InWindowHandle, &x, &y);
	return { (float)x, (float)y };
}

void Input::SetCursorMode(CursorMode mode, GLFWwindow* InWindowHandle)
{
	if (!InWindowHandle) {
		return;
	}

	glfwSetInputMode(InWindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int32)mode);
}

