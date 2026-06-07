#include "UI.h"

#include "Window/Window.h"

#define IMGUI_DISABLE_DEFAULT_ALLOCATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
//// Emedded font
//#include "ImGui/Roboto-Regular.embed"

void UI::InitUI(
	UIContext* uiContext
	, const ImGuiAllocator* imGuiAllocator
	, Drop::Window* window
) {
	if (!uiContext) return;
	if (!imGuiAllocator) return;
	if (!imGuiAllocator->allocFunc) return;
	if (!imGuiAllocator->freeFunc) return;
	if (!imGuiAllocator->userData) return;
	if (!window) return;

	IMGUI_CHECKVERSION();

	ImGui::SetAllocatorFunctions(
		imGuiAllocator->allocFunc
		, imGuiAllocator->freeFunc
		, imGuiAllocator->userData
	);

	uiContext->imGuiContext = ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window->glfwWindow, true);
	ImGui_ImplOpenGL3_Init("#version 410");

}

// Recall at every HotReload
void UI::HotReloadContextReset(
	UIContext* uiContext
	, const ImGuiAllocator* imGuiAllocator
	, Drop::Window* window
) {
	ImGui::SetCurrentContext(uiContext->imGuiContext);

	ImGui::SetAllocatorFunctions(
		imGuiAllocator->allocFunc
		, imGuiAllocator->freeFunc
		, imGuiAllocator->userData
	);
}

void UI::UpdateAndRenderUI(
	UpdateUI* updateUIfunc
	, Drop::Window* window
) {
	if (!updateUIfunc) return;

	// render your GUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Call game UI update
	(*(updateUIfunc))();

	// Just for test
	ImGui::Begin("Drop Rendering");
	ImGui::Text("Some Text data: %d", 123);
	ImGui::End();
	// -------

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)window->width, (float)window->height);
	
	// Rendering ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

}