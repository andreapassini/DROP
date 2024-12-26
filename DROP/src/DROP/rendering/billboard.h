#pragma once

#include "../math/mat3.h"

using namespace VgMath;

struct Billboard {
	Transform* transform = nullptr; // when using ECS, this will be in an another comp.

	Vector3 postion{ 0.0f };
	float scale = 0.0f;
	uint32_t textureID = 0;
};