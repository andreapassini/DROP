#include "physicsEngine.h"

#include <future> 

#include "physicsComponent.h"
#include <xmmintrin.h>

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

void PhysicsEngine::SIMD_PhysicsStep(
	ECS& ecs
	, const int32_t max
) {
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	//ApplyForces();
	SIMD_ApplyForces(ecs, max);

	//// Handle constraints
	//ApplyConstraints();

	//// Handle collisions
	//HandleCollision();
}

void PhysicsEngine::PhysicsStep(
	ECS& ecs
	, const int32_t max
) {
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	ApplyForces(ecs, max);

	//// Handle constraints
	//ApplyConstraints();

	//// Handle collisions
	//HandleCollision();
}

void PhysicsEngine::MultiThread_PhysicsStep(
	ECS& ecs
	, const int32_t max
) {
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	MultiThread_ApplyForces(ecs, max);

	//// Handle constraints
	//ApplyConstraints();

	//// Handle collisions
	//HandleCollision();
}

void VerletResolution(
	PhysicsComponent* physicsComponent
	, int32_t offset
	, int32_t numOfElements
	, int32_t max
) {
	if (offset + numOfElements > max)
	{
		// remove exceeding elements
		numOfElements -= (offset + numOfElements) - max; 
		if (numOfElements < 0)
		{
			return;
		}
	}

#pragma omp simd
	for (int32_t i = offset; i < numOfElements; i++)
	{
		PhysicsComponent& currentPhysicsComp = physicsComponent[i];

		// assuming to always add gravity
		currentPhysicsComp.force = VgMath::Vector3(0.0, -9.8, 0.0);

		VgMath::Vector3 tempPos = currentPhysicsComp.position;
		VgMath::Vector3 accel = currentPhysicsComp.force / currentPhysicsComp.mass;
		currentPhysicsComp.position = ((2.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.position) 
			- ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition) + (accel * currentPhysicsComp.FIXED_TIME_STEP2);
		currentPhysicsComp.oldPosition = tempPos;

		// impulse only in one frame
		currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
	}
}

void PhysicsEngine::ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	std::vector<PhysicsComponent>& densePhysicsComponents = ecs.GetComponentPool<PhysicsComponent>().Data();


	for (int32_t i = 0; i < max; i++)
	{
		PhysicsComponent& currentPhysicsComp = densePhysicsComponents[i];

		// assuming to always add gravity
		currentPhysicsComp.force = VgMath::Vector3(0.0, -9.8, 0.0);

		VgMath::Vector3 tempPos = currentPhysicsComp.position;
		VgMath::Vector3 accel = currentPhysicsComp.force / currentPhysicsComp.mass;
		currentPhysicsComp.position = ((2.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.position)
			- ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition) + (accel * currentPhysicsComp.FIXED_TIME_STEP2);
		currentPhysicsComp.oldPosition = tempPos;

		// impulse only in one frame
		currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
	}

//
////#pragma omp simd
//	for (size_t i = 0; i < densePhysicsComponents.size(); i++)
//	{
//		PhysicsComponent& currentPhysicsComp = densePhysicsComponents[i];
//
//		// SIMD TEST
//		currentPhysicsComp.force.x *= 10.0f;
//		//
//
//		//if (currentPhysicsComp.isStatic)
//		//{
//		//	currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
//		//	return;
//		//}
//
//		//// assuming to always add gravity
//		//currentPhysicsComp.force = VgMath::Vector3(0.0, -9.8, 0.0);
//
//		//VgMath::Vector3 tempPos = currentPhysicsComp.position;
//		//VgMath::Vector3 accel = currentPhysicsComp.force / currentPhysicsComp.mass;
//		//currentPhysicsComp.position = ((2.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.position) 
//		//	- ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition) + (accel * currentPhysicsComp.FIXED_TIME_STEP2);
//		//currentPhysicsComp.oldPosition = tempPos;
//
//		//// impulse only in one frame
//		//currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
//	}
}

void PhysicsEngine::MultiThread_ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	std::vector<std::future<void>> futures;

	std::vector<PhysicsComponent>& densePhysicsComponents = ecs.GetComponentPool<PhysicsComponent>().Data();

	size_t numElementsPerThread = 1'000;
	//size_t max = densePhysicsComponents.size();
	for (int32_t i = 0; i < max; i+= numElementsPerThread) {
		int32_t offset = i /** numElementsPerThread*/;
		futures.push_back(
			std::async(std::launch::async,
				VerletResolution
				, &densePhysicsComponents[i]
				, offset
				, numElementsPerThread
				, max
			)
		);
	}

	for (auto& handle : futures) {
		handle.wait();
	}

//
////#pragma omp simd
//	for (size_t i = 0; i < densePhysicsComponents.size(); i++)
//	{
//		PhysicsComponent& currentPhysicsComp = densePhysicsComponents[i];
//
//		// SIMD TEST
//		currentPhysicsComp.force.x *= 10.0f;
//		//
//
//		//if (currentPhysicsComp.isStatic)
//		//{
//		//	currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
//		//	return;
//		//}
//
//		//// assuming to always add gravity
//		//currentPhysicsComp.force = VgMath::Vector3(0.0, -9.8, 0.0);
//
//		//VgMath::Vector3 tempPos = currentPhysicsComp.position;
//		//VgMath::Vector3 accel = currentPhysicsComp.force / currentPhysicsComp.mass;
//		//currentPhysicsComp.position = ((2.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.position) 
//		//	- ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition) + (accel * currentPhysicsComp.FIXED_TIME_STEP2);
//		//currentPhysicsComp.oldPosition = tempPos;
//
//		//// impulse only in one frame
//		//currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
//	}
}

void PhysicsEngine::SIMD_ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	std::vector<PhysicsComponent>& densePhysicsComponents = ecs.GetComponentPool<PhysicsComponent>().Data();

	//size_t n = densePhysicsComponents.size();

	//for (size_t i = 0; i < n; i += 8)
	//{  // Process 8 floats at a time
	//	__m256 va = _mm256_loadu_ps(&a[i]);  // Load 8 floats
	//	__m256 vb = _mm256_loadu_ps(&b[i]);
	//	__m256 vr = _mm256_add_ps(va, 6);   // Vectorized addition
	//	_mm256_storeu_ps(&result[i], vr);    // Store result
	//}

#pragma omp simd simdlen(128)
	for (int32_t i = 0; i < max; i++)
	{
		PhysicsComponent& currentPhysicsComp = densePhysicsComponents[i];

		// assuming to always add gravity
		currentPhysicsComp.force = VgMath::Vector3(0.0, -9.8, 0.0);

		VgMath::Vector3 tempPos = currentPhysicsComp.position;
		VgMath::Vector3 accel = currentPhysicsComp.force / currentPhysicsComp.mass;
		currentPhysicsComp.position = ((2.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.position) 
			- ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition) + (accel * currentPhysicsComp.FIXED_TIME_STEP2);
		currentPhysicsComp.oldPosition = tempPos;

		// impulse only in one frame
		currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
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

