#pragma once

#include "DROP/math/transform.h"
#include <vector>

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

struct alignas(64) PhysicsComponents
{
	//std::vector<VgMath::Vector3> positions;
	std::vector<float> positions_x;
	std::vector<float> positions_y;
	std::vector<float> positions_z;
	//std::vector<VgMath::Vector3> oldPositions;
	std::vector<float> oldPositions_x;
	std::vector<float> oldPositions_y;
	std::vector<float> oldPositions_z;
	//std::vector<VgMath::Vector3> forces;
	std::vector<float> forces_x;
	std::vector<float> forces_y;
	std::vector<float> forces_z;
	std::vector<float> masses;

	uint32_t processor_count = 0;
	size_t numElementsPerThread = 0;

	static constexpr float DAMPING = 0.01f; // how much to damp the simulation each frame
	static constexpr float FIXED_TIME_STEP = (1.0f / 120.0f);
	static constexpr float FIXED_TIME_STEP2 = (FIXED_TIME_STEP * FIXED_TIME_STEP);
};