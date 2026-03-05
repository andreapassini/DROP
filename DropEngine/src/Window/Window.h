#pragma once

#include "string"
#include "functional"

//#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Engine includes
#include "Types/Types.h"

namespace Drop
{
	struct WindowProps
	{
		std::string title = "Drop Engine";
		int32 width = 1600;
		int32 height = 900;
	};

	struct Window
	{
		std::string title;
		int32 width = 1600;
		int32 height = 900; // cannot use uint32 cause of glfw API Problem
		bool VSync = true;

		//EventCallbackFn EventCallback;

		// For now this is glfw specific
		// we could just make it void ptr
		// and make a macro to cast it to the specific ptr
		GLFWlibrary* glfwLibrary = nullptr;
		GLFWwindow* glfwWindow = nullptr;
	};

	void InitWindow(
		const WindowProps& props
		, Window* window
		, const GLFWallocator* glfwAllocator
	);

	void SetVSync(
		const bool enabled
		, Window* window
	);

	void OnUpdate(
		Window* window
	);

	void OnEndFrame(
		Window* window
	);

	bool IsShouldClose(
		Window* window
	);
}