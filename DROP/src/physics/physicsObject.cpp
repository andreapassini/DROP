#include "physicsObject.h"

void PhysicsObject::AddForce(VgMath::Vector3 force_val)
{
	this->force += force_val;
}

void PhysicsObject::PhysicsStep()
{
	if (!isStatic) {
		this->force = VgMath::Vector3(0.0, 0.0, 0.0);
		return;
	}

	VgMath::Vector3 tempPos = position;
	VgMath::Vector3 accel = force / mass;
	this->position = ((2.0f - DAMPING) * position) - ((1.0f - DAMPING) * oldPosition) + (accel * FIXED_TIME_STEP2);
	this->oldPosition = tempPos;

	this->force = VgMath::Vector3(0.0, 0.0, 0.0);
}
