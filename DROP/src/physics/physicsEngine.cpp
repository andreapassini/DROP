#include "physicsEngine.h"

#include <future> 

void PhysicsEngine::SinglePhysicsStep(PhysicsObject* const physicsObject) {
	physicsObject->PhysicsStep();
}

PhysicsEngine::PhysicsEngine(double startingTime, uint32_t reserve_val)
{
	isPaused = false;
	virtualTime = startingTime;

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

	std::vector<std::future<void>> futures;

	
	for (auto& it : physicsObjetcs) {
		futures.push_back(
			std::async(std::launch::async,
				SinglePhysicsStep,
				&it.second
			)
		);
	}

	for (auto& handle : futures) {
		handle.wait();
	}
}

double PhysicsEngine::getVirtualTIme() { return virtualTime; }

