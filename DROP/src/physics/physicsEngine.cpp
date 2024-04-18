#include "physicsEngine.h"

#include <future> 

void PhysicsEngine::ApplyForceToSinglePhysicsObject(PhysicsObject* const physicsObject) {
	physicsObject->PhysicsStep();
}

PhysicsEngine::PhysicsEngine(double startingTime, uint32_t reserve_val)
{
	isPaused = false;
	virtualTime = startingTime;

	constraintsIterations = 1;
	collisionsIterations = 1;

	physicsObjetcs.reserve(reserve_val);
}

void PhysicsEngine::SynchVirtualTime(double timeToSync) {
	virtualTime = timeToSync;
}

void PhysicsEngine::PhysicsStep()
{
	if (isPaused)
		return;

	virtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	ApplyForces();

	// Handle constraints
	ApplyConstraints();

	// Handle collisions
	HandleCollision();
}

void PhysicsEngine::ApplyForces()
{
	std::vector<std::future<void>> futures;

	for (auto& it : physicsObjetcs) {
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
	for (size_t i = 0; i < constraintsIterations; i++) {

	}
}

void PhysicsEngine::HandleCollision()
{
	for (size_t i = 0; i < collisionsIterations; i++) {

	}
}

double PhysicsEngine::getVirtualTIme() { return virtualTime; }

void PhysicsEngine::AddForceToAll(VgMath::Vector3 force)
{
	for (size_t i = 0; i < physicsObjetcs.size(); i++) {
		physicsObjetcs[i].AddForce(force);
	}
}

