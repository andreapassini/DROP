#pragma once

#include <string>

#include "Drop/ECS/beecs.h"

struct Scene
{
	std::string sceneName;
	bseecs::ECS ecs;
};