#pragma once

#include "DROP/math/transform.h"

struct PhysicsComponent
{
	VgMath::Vector3 position;
	VgMath::Vector3 oldPosition;
	VgMath::Vector3 force;
	float mass = 0.0f;

	bool isStatic = false;

	static constexpr float DAMPING = 0.01f; // how much to damp the simulation each frame
	static constexpr float FIXED_TIME_STEP = (1.0f / 120.0f);
	static constexpr float FIXED_TIME_STEP2 = (FIXED_TIME_STEP * FIXED_TIME_STEP);
};