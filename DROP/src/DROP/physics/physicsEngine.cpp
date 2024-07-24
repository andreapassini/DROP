#include "physicsEngine.h"

#include <future> 

void PhysicsEngine::ApplyForceToSinglePhysicsObject(PhysicsObject* const physicsObject) {
	physicsObject->PhysicsStep();
}

PhysicsEngine::PhysicsEngine(double startingTime, uint32_t reserve_val)
{
	m_IsPaused = false;
	m_VirtualTime = startingTime;

	m_ConstraintsIterations = 1;
	m_CollisionsIterations = 1;

	m_PhysicsObjetcs.reserve(reserve_val);
}

void PhysicsEngine::SynchVirtualTime(double timeToSync) {
	m_VirtualTime = timeToSync;
}

void PhysicsEngine::PhysicsStep()
{
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	ApplyForces();

	// Handle constraints
	ApplyConstraints();

	// Handle collisions
	HandleCollision();
}

void PhysicsEngine::ApplyForces()
{
	std::vector<std::future<void>> futures;

	for (auto& it : m_PhysicsObjetcs) {
		futures.push_back(
			std::async(std::launch::async,
				ApplyForceToSinglePhysicsObject,
				&it.second
			)
		);
	}

	for (auto& handle : futures) {
		handle.wait();
	}
}

void PhysicsEngine::ApplyConstraints()
{
	for (size_t i = 0; i < m_ConstraintsIterations; i++) {

	}
}

void PhysicsEngine::HandleCollision()
{
	for (size_t i = 0; i < m_CollisionsIterations; i++) {

	}
}

double PhysicsEngine::GetVirtualTIme() { return m_VirtualTime; }

void PhysicsEngine::AddForceToAll(VgMath::Vector3 force)
{
	for (size_t i = 0; i < m_PhysicsObjetcs.size(); i++) {
		m_PhysicsObjetcs[i].AddForce(force);
	}
}

