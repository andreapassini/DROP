#pragma once

#include "Memory/arenaAllocator.h"

class Drop::Window;

struct EngineState
{
	Drop::Window* windowHandle;

	ArenaAllocator persistentArenaAllocator;
	ArenaAllocator sceneArenaAllocator;
	ArenaAllocator frameArenaAllocator;
};