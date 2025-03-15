#pragma once

#include "DROP/math/transform.h"

class PhysicsObject
{
public:
	void AddForce(VgMath::Vector3 force_val);
	void PhysicsStep();
public:
	VgMath::Vector3 m_Position;
	VgMath::Vector3 m_OldPosition;
	VgMath::Vector3 m_Force;
	float m_Mass;

	bool m_IsStatic;

	static constexpr float DAMPING = 0.01f; // how much to damp the simulation each frame
	static constexpr float FIXED_TIME_STEP = (1.0f / 30.0f);
	static constexpr float FIXED_TIME_STEP2 = (FIXED_TIME_STEP * FIXED_TIME_STEP);

};