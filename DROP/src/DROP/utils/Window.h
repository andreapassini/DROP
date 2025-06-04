#pragma once

#include "string"
#include "functional"

#include "../dependencies/GLFW/include/GLFW/glfw3.h"

namespace Drop
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Drop Engine",
			unsigned int width = 1600,
			unsigned int height = 900)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		//using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void OnUpdate();
		void OnEndFrame();
		bool IsShouldClose();
		void SetShouldClose(bool shouldClose);

		inline uint32_t GetWidth() const { return m_Data.Width; }
		inline uint32_t GetHeight() const { return m_Data.Height; }

		// Window attributes
		//virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		void SetVSync(bool enabled);
		bool IsVSync() const;

		inline void* GetNativeWindow() const { return m_Window; }

		static Window* Create(const WindowProps& props = WindowProps());

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			//EventCallbackFn EventCallback;
		};

		WindowData m_Data;

	};
}