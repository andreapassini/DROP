#pragma once

#include <unordered_map>

#include "../math/transform.h"

#include "physicsObject.h"
#include "constraint.h"
#include "collider.h"

class PhysicsEngine
{
public:
	std::unordered_map<uint32_t, PhysicsObject> physicsObjetcs;
	// Yes i have to use the pointer and ruin the data-oriented aproach 
	// since virtual works only with pointers
	std::unordered_map<uint32_t, vector<Constraint*>> constraints;
	std::unordered_map<uint32_t, vector<Collider*>> colliders;

	bool isPaused;
	uint32_t constraintsIterations;
	uint32_t collisionsIterations;

	PhysicsEngine(double startingTime, uint32_t reserve_val);

	void PhysicsStep();

	void SynchVirtualTime(double timeToSync);

	double getVirtualTIme();

	void AddForceToAll(VgMath::Vector3 force);

	static constexpr uint32_t maxIter = 15;

private:
	double virtualTime;

	void ApplyForces();
	void ApplyConstraints();
	void HandleCollision();

	static void ApplyForceToSinglePhysicsObject(PhysicsObject* const physicsObject);

};

