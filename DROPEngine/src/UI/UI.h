#pragma once

struct ImGuiContext;

namespace Drop
{
typedef void* (*ImGuiAllocFunc)(size_t sz, void* user_data);
typedef void (*ImGuiFreeFunc)(void* ptr, void* user_data);

struct ImGuiAllocator {
	ImGuiAllocFunc allocFunc;
	ImGuiFreeFunc freeFunc;
	void* userData;
};

struct UI {
	ImGuiContext* imGuiContext = nullptr;
};

void InitUI(
	UI* uiContext
	, const ImGuiAllocator* imGuiAllocator
);

}