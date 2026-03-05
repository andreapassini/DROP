#include "Window.h"

#include "Utils/Log.h"
#include "Types/Types.h"

//#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

#define CHECK_AND_RETURN_WINDOW(window) \
	if(!window) \
	{	\
		LOG_CORE_ERROR("Window Error: ({0})", "!window"); \
		return; \
	} \
	if(!window->glfwWindow) \
	{ \
		LOG_CORE_ERROR("Window Error: ({0})", "!window->glfwWindow"); \
		return; \
	} \
	if(!window->glfwLibrary) \
	{ \
		LOG_CORE_ERROR("Window Error: ({0})", "!window->glfwLibrary"); \
		return; \
	} \

#define CHECK_AND_RETURN_WINDOW(window, returnValue) \
	if(!window) \
	{	\
		LOG_CORE_ERROR("Window Error: ({0})", "!window"); \
		return returnValue; \
	} \
	if(!window->glfwWindow) \
	{ \
		LOG_CORE_ERROR("Window Error: ({0})", "!window->glfwWindow"); \
		return returnValue; \
	} \
	if(!window->glfwLibrary) \
	{ \
		LOG_CORE_ERROR("Window Error: ({0})", "!window->glfwLibrary"); \
		return returnValue; \
	} \

using namespace Drop;

namespace Drop
{

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int32 error, const char* description)
	{
		LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	//void glfwInitAllocator(const GLFWallocator* allocator);
	void InitWindow(
		const WindowProps& props
		, Window* window
		, const GLFWallocator* glfwAllocator
	) {
		if (!window)
		{
			return;
		}
		if (!glfwAllocator)
		{
			return;
		}

		window->width = props.width;
		window->height = props.height;
		window->title = props.title; // Fix String allocation

		LOG_CORE_INFO("Creating window {0} ({1}, {2})", window->title, window->width, window->height);

		// Assuming 
		// !window->glfwLibrary 
		// and !window->glfwWindow

		// TODO: glfwTerminate on system shutdown
		glfwInitAllocator(glfwAllocator); // THIS IS STILL USING A STRUCT and not a PTR
		window->glfwLibrary = glfwAllocateLib(glfwAllocator);
		glfwSetLib(window->glfwLibrary);
		
		int32 success = glfwInit();
		assert(success /*, "Could not intialize GLFW!"*/);
		glfwSetErrorCallback(GLFWErrorCallback);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		// we set if the window is resizable
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   // If u want to resize it, u have to change also the camera

		window->glfwWindow = glfwCreateWindow(
			window->width
			, window->height
			, window->title.c_str()
			, nullptr
			, nullptr
		);

		// Consider making Context Graphics Lib independent
		if (!window->glfwWindow)
		{
			assert(false/*, "Failed to create GLFW window"*/);
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(window->glfwWindow);
		int32 status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		assert(status/*, "Failed to initialize Glad!"*/);

		LOG_CORE_INFO("OpenGL Info:");
		LOG_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		LOG_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		LOG_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		glfwSetWindowUserPointer(
			window->glfwWindow
			, window
		);
		SetVSync(
			true
			, window
		);
	}

	void Shutdown(
		Window* window
	) {
		CHECK_AND_RETURN_WINDOW(window);

		glfwDestroyWindow(window->glfwWindow);
		glfwTerminate();
	}

	void OnUpdate(
		Window* window
	) {
		CHECK_AND_RETURN_WINDOW(window);

		glfwPollEvents();

		// Handle Resize
		int32 width = 0;
		int32 height = 0;
		glfwGetFramebufferSize(window->glfwWindow, &width, &height);
		if (width != window->width
			|| height != window->height
		) {
			window->width = width;
			window->height = height;
		}
	}

	void OnEndFrame(
		Window* window
	) {
		CHECK_AND_RETURN_WINDOW(window);

		glfwSwapBuffers(window->glfwWindow);
	}

	bool IsShouldClose(
		Window* window
	) {
		CHECK_AND_RETURN_WINDOW(window, false);

		return glfwWindowShouldClose(window->glfwWindow);
	}

	void SetShouldClose(
		bool shouldClose
		, Window* window
	) {
		CHECK_AND_RETURN_WINDOW(window);

		glfwSetWindowShouldClose(
			window->glfwWindow
			, shouldClose
		);
	}

	void SetVSync(
		const bool enabled
		, Window* window
	) {
		CHECK_AND_RETURN_WINDOW(window);

		enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);

		window->VSync = enabled;
	}
}
