#pragma once

#include <unordered_map>

#include "../math/transform.h"

#include "physicsObject.h"

class PhysicsEngine
{
public:
	std::unordered_map<uint32_t, PhysicsObject> physicsObjetcs;

	bool isPaused;

	PhysicsEngine(double startingTime, uint32_t reserve_val);

	void PhysicsStep();

	void SynchVirtualTime(double timeToSync);

	double getVirtualTIme();

	static constexpr uint32_t maxIter = 15;

private:
	double virtualTime;

	static void SinglePhysicsStep(PhysicsObject* const physicsObject);

};

