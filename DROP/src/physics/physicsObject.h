#pragma once

#include "../math/transform.h"

//#define DAMPING 0.01f // how much to damp the simulation each frame

//#define FIXED_TIME_STEP (1.0f / 30.0f)
//#define FIXED_TIME_STEP2 (FIXED_TIME_STEP * FIXED_TIME_STEP)

class PhysicsObject
{
public:
	VgMath::Vector3 position;
	VgMath::Vector3 oldPosition;
	VgMath::Vector3 force;
	float mass;

	bool isStatic;
	
	void AddForce(VgMath::Vector3 force_val);

	void PhysicsStep();

	static constexpr float DAMPING = 0.01f;
	static constexpr float FIXED_TIME_STEP = (1.0f / 30.0f);
	static constexpr float FIXED_TIME_STEP2 = (FIXED_TIME_STEP * FIXED_TIME_STEP);

};

