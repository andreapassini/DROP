#pragma once

#include "Memory/arenaAllocator.h"

class Drop::Window;

struct EngineState
{
	ArenaAllocator persistentArenaAllocator;
	ArenaAllocator sceneArenaAllocator;
	ArenaAllocator frameArenaAllocator;
	Drop::Window* windowHandle;
};