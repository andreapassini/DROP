#include "UI.h"

#define IMGUI_DISABLE_DEFAULT_ALLOCATORS
#include "imgui.h"

#include "Window/Window.h"

void Drop::InitUI(
	UI* uiContext
	, const ImGuiAllocator* imGuiAllocator
) {
	if (!uiContext) return;
	if (!imGuiAllocator) return;
	if (!imGuiAllocator->allocFunc) return;
	if (!imGuiAllocator->freeFunc) return;
	if (!imGuiAllocator->userData) return;

	IMGUI_CHECKVERSION();

	// Recall at every HotReload
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

	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_ActiveWindowHandle->GetNativeWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");

}