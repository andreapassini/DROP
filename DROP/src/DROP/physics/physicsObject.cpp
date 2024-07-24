#include "physicsObject.h"

void PhysicsObject::AddForce(VgMath::Vector3 force_val)
{
	this->m_Force += force_val;
}

void PhysicsObject::PhysicsStep()
{
	if (!m_IsStatic) {
		this->m_Force = VgMath::Vector3(0.0, 0.0, 0.0);
		return;
	}

	VgMath::Vector3 tempPos = m_Position;
	VgMath::Vector3 accel = m_Force / m_Mass;
	this->m_Position = ((2.0f - DAMPING) * m_Position) - ((1.0f - DAMPING) * m_OldPosition) + (accel * FIXED_TIME_STEP2);
	this->m_OldPosition = tempPos;

	this->m_Force = VgMath::Vector3(0.0, 0.0, 0.0);
}
