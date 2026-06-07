#pragma once

#include "Memory/arenaAllocator.h"

class Drop::Window;
struct UI::UIContext;

struct EngineState
{
	Drop::Window* windowHandle;
	UI::UIContext* uiContext;
	UI::ImGuiAllocator imGuiAllocator;

	// Keep this last since at Update we will cast mem from .exe to GameState
	// and the persistent buffer left we will be used for the memory allocator 
	// back buffer
	ArenaAllocator persistentArenaAllocator;
	ArenaAllocator sceneArenaAllocator;
	ArenaAllocator frameArenaAllocator;
};