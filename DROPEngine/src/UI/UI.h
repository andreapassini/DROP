#pragma once

struct ImGuiContext;

namespace Drop {
	struct Window;
}

namespace UI {
typedef void* (*ImGuiAllocFunc)(size_t sz, void* user_data);
typedef void (*ImGuiFreeFunc)(void* ptr, void* user_data);
typedef void (*UpdateUI)();

struct ImGuiAllocator
{
	ImGuiAllocFunc allocFunc;
	ImGuiFreeFunc freeFunc;
	void* userData;
};

struct UIContext
{
	ImGuiContext* imGuiContext = nullptr;
};

void InitUI(
	UIContext* uiContext
	, const ImGuiAllocator* imGuiAllocator
	, Drop::Window* window
);

void HotReloadContextReset(
	UIContext* uiContext
	, const ImGuiAllocator* imGuiAllocator
	, Drop::Window* window
);

void UpdateAndRenderUI(
	UpdateUI* updateUIfunc
	, Drop::Window* window
);

}
