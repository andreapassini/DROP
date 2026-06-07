#pragma once

#include "Memory/arenaAllocator.h"

class Drop::Window;
struct Drop::UI;

struct EngineState
{
	Drop::Window* windowHandle;
	Drop::UI* uiContext;

	// Keep this last since at Update we will cast mem from .exe to GameState
	// and the persistent buffer left we will be used for the memory allocator 
	// back buffer
	ArenaAllocator persistentArenaAllocator;
	ArenaAllocator sceneArenaAllocator;
	ArenaAllocator frameArenaAllocator;
};