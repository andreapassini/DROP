#pragma once

#include <unordered_map>

#include "DROP/math/transform.h"

#include "physicsObject.h"
#include "constraint.h"
#include "collider.h"

class PhysicsEngine
{
public:
	PhysicsEngine(double startingTime, uint32_t reserve_val);

	void PhysicsStep();

	void SynchVirtualTime(double timeToSync);

	double GetVirtualTIme();

	void AddForceToAll(VgMath::Vector3 force);
public:
	std::unordered_map<uint32_t, PhysicsObject> m_PhysicsObjetcs;
	// Yes I have to use the pointer and ruin the data-oriented approach 
	// since virtual works only with pointers
	std::unordered_map<uint32_t, vector<Constraint*>> m_Constraints;
	std::unordered_map<uint32_t, vector<Collider*>> m_Colliders;

	bool m_IsPaused;
	uint32_t m_ConstraintsIterations;
	uint32_t m_CollisionsIterations;
	static constexpr uint32_t maxIter = 15;

private:
	void ApplyForces();
	void ApplyConstraints();
	void HandleCollision();
	static void ApplyForceToSinglePhysicsObject(PhysicsObject* const physicsObject);
private:
	double m_VirtualTime;
};

