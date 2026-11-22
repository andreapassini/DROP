#pragma once

#include <unordered_map>

#include "DROP/math/transform.h"

#include "physicsObject.h"
#include "constraint.h"
#include "collider.h"

#include "DROP/ECS/beecs.h"
#include "DROP/physics/physicsComponent.h"

using namespace bseecs;

class PhysicsEngine
{
public:
	PhysicsEngine(double startingTime, uint32_t reserve_val);

	void PhysicsStep(ECS& ecs
		, const int32_t max
	);
	void MultiThread_PhysicsStep(ECS& ecs
		, const int32_t max
	);
	void SIMD_PhysicsStep(ECS& ecs
		, const int32_t max
	);

	void SoA_PhysicsStep(
		ECS& ecs
		, const int32_t max
		, PhysicsComponents& physicsComponents
	);
	void SIMD_SoA_PhysicsStep(ECS& ecs
		, const int32_t max
	);
	void SIMD_ByHand_SoA_PhysicsStep(ECS& ecs
		, const int32_t max
	);
	void MultiThread_SoA_PhysicsStep(
		ECS& ecs
		, const int32_t max
		, const int32_t numThreads
		, const int32_t numElementsPerThread
		, PhysicsComponentsFalseSharing& physicsComponentsFalseSharing
	);

	void MultiThread_SoA_SIMDByHand_PhysicsStep(
		ECS& ecs
		, const int32_t max
		, const int32_t numThreads
		, const int32_t numElementsPerThread
		, PhysicsComponentsFalseSharing& physicsComponentsFalseSharing
	);

	void SynchVirtualTime(double timeToSync);

	double GetVirtualTIme();

	void AddForceToAll(VgMath::Vector3 force);

	void static ResetPositions(
		std::vector<PhysicsComponent> A
	);

	static bool CheckEquality(
		std::vector<PhysicsComponent>& A
		, std::vector<PhysicsComponent>& B
	);

	void static ResetPositions(
		PhysicsComponents* A
	);

	void static ResetPositions(
		PhysicsComponentsFalseSharing* A
	);

	static void PhysicsEngine::Copy(
		PhysicsComponents* Source
		, PhysicsComponents* Destination
	);

	static void PhysicsEngine::Copy(
		PhysicsComponentsFalseSharing* Source
		, PhysicsComponentsFalseSharing* Destination
	);
	
	static bool CheckEquality(
		PhysicsComponents* A
		, PhysicsComponents* B
	);
	
	static bool CheckEquality(
		PhysicsComponentsFalseSharing* A
		, PhysicsComponentsFalseSharing* B
	);
	

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

	void ApplyForces(ECS& ecs
		, const int32_t max
	);
	void MultiThread_ApplyForces(ECS& ecs
		, const int32_t max
	);
	void SIMD_ApplyForces(ECS& ecs
		, const int32_t max
	);

	void SoA_ApplyForces(
		ECS& ecs
		, const int32_t max
		, PhysicsComponents& physicsComponents
	);

	void MultiThread_SoA_ApplyForces(
		ECS& ecs
		, const int32_t max
		, const int32_t numThreads
		, const int32_t numElementsPerThread
		, PhysicsComponentsFalseSharing& physicsComponentsFalseSharing
	);
	void MultiThread_SoA_SIMDByHand_ApplyForces(
		ECS& ecs
		, const int32_t max
		, const int32_t numThreads
		, const int32_t numElementsPerThread
		, PhysicsComponentsFalseSharing& physicsComponentsFalseSharing
	);
	void SIMD_SoA_ApplyForces(ECS& ecs
		, const int32_t max
	);
	void SIMD_ByHand_SoA_ApplyForces(ECS& ecs
		, const int32_t max
	);

	void ApplyConstraints();
	void HandleCollision();
	static void ApplyForceToSinglePhysicsObject(PhysicsObject* const physicsObject);
private:
	double m_VirtualTime;
};

