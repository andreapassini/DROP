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

// SoA
struct PhysicsComponents
{
	std::vector<VgMath::Vector3> positions;
	std::vector<VgMath::Vector3> oldPositions;
	std::vector<VgMath::Vector3> forces;
	std::vector<float> masses;

	uint32_t processor_count = 0;
	size_t numElementsPerThread = 0;

	static constexpr float DAMPING = 0.01f; // how much to damp the simulation each frame
	static constexpr float FIXED_TIME_STEP = (1.0f / 120.0f);
	static constexpr float FIXED_TIME_STEP2 = (FIXED_TIME_STEP * FIXED_TIME_STEP);
};