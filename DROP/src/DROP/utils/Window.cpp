
#include "Log.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

using namespace Drop;

namespace Drop
{

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new Window(props);
	}

	Window::Window(const WindowProps& props)
	{
		Init(props);
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LOG_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			assert(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		//// we set if the window is resizable
		//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);   // If u want to resize it, u have to change also the camera

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		// Consider making Context Graphics Lib independent
		if (!m_Window)
		{
			assert(false, "Failed to create GLFW window");
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		assert(status, "Failed to initialize Glad!");

		LOG_CORE_INFO("OpenGL Info:");
		LOG_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		LOG_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		LOG_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();

		// Handle Resize
		int width, height;
		glfwGetFramebufferSize((GLFWwindow*)GetNativeWindow(), &width, &height);
		if (width != m_Data.Width || height != m_Data.Height)
		{
			m_Data.Width = width;
			m_Data.Height = height;
		}
	}

	void Window::OnEndFrame()
	{
		glfwSwapBuffers(m_Window);
	}

	bool Window::IsShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}

	void Window::SetShouldClose(bool shouldClose)
	{
		glfwSetWindowShouldClose(m_Window, shouldClose);
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool Window::IsVSync() const
	{
		return m_Data.VSync;
	}

}
