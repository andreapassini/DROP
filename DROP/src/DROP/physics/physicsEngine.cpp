#include "physicsEngine.h"

#include <future> 
#include <thread>

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

void PhysicsEngine::SoA_PhysicsStep(
	ECS& ecs
	, const int32_t max
) {
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	SoA_ApplyForces(ecs, max);

	//// Handle constraints
	//ApplyConstraints();

	//// Handle collisions
	//HandleCollision();
}

void PhysicsEngine::SIMD_PhysicsStep(
	ECS& ecs
	, const int32_t max
) {
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	SIMD_ApplyForces(ecs, max);

	//// Handle constraints
	//ApplyConstraints();

	//// Handle collisions
	//HandleCollision();
}

void PhysicsEngine::SIMD_SoA_PhysicsStep(
	ECS& ecs
	, const int32_t max
) {
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	SIMD_SoA_ApplyForces(ecs, max);

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

void PhysicsEngine::MultiThread_SoA_PhysicsStep(
	ECS& ecs
	, const int32_t max
) {
	if (m_IsPaused)
		return;

	m_VirtualTime += PhysicsObject::FIXED_TIME_STEP;
	
	MultiThread_SoA_ApplyForces(ecs, max);

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

void SoA_VerletResolution(
	VgMath::Vector3* positions
	, VgMath::Vector3* oldPositions
	, VgMath::Vector3* forces
	, float* masses
	, float DAMPING
	, float FIXED_TIME_STEP2
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
		// assuming to always add gravity
		forces[i] = VgMath::Vector3(0.0, -9.8, 0.0);

		VgMath::Vector3 accel = forces[i] / masses[i];
		VgMath::Vector3 tempPos = positions[i];
		positions[i] = ((2.0f - DAMPING) * positions[i])
			- ((1.0f - DAMPING) * oldPositions[i])
			+ (accel * FIXED_TIME_STEP2);
		oldPositions[i] = tempPos;

		// impulse only in one frame
		//forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
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
			- ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition) 
			+ (accel * currentPhysicsComp.FIXED_TIME_STEP2);
		currentPhysicsComp.oldPosition = tempPos;


		// impulse only in one frame
		currentPhysicsComp.force = VgMath::Vector3(0.0, 0.0, 0.0);
	}
}

void PhysicsEngine::SoA_ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	PhysicsComponents& physicsComponents = ecs.GetSingletonComponent<PhysicsComponents>();

	for (int32_t i = 0; i < max; i++)
	{
		// assuming to always add gravity
		physicsComponents.forces[i] = VgMath::Vector3(0.0, -9.8, 0.0);

		VgMath::Vector3 tempPos = physicsComponents.positions[i];
		VgMath::Vector3 accel = physicsComponents.forces[i] / physicsComponents.masses[i];
		physicsComponents.positions[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions[i])
			+ (accel * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.oldPositions[i] = tempPos;

		// impulse only in one frame
		//physicsComponents.forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
	}
}

void PhysicsEngine::MultiThread_ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	std::vector<std::future<void>> futures;

	std::vector<PhysicsComponent>& densePhysicsComponents = ecs.GetComponentPool<PhysicsComponent>().Data();

	size_t numElementsPerThread = 10'000;
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

void PhysicsEngine::MultiThread_SoA_ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	std::vector<std::future<void>> futures;

	PhysicsComponents& physicsComponents = ecs.GetSingletonComponent<PhysicsComponents>();

	//may return 0 when not able to detect
	physicsComponents.processor_count = std::thread::hardware_concurrency();
	if (physicsComponents.processor_count == 0)
	{
		assert(0);
	}

	//physicsComponents.numElementsPerThread = max / physicsComponents.processor_count;
	physicsComponents.numElementsPerThread = 10'000;
	for (int32_t i = 0; i < max; i+= physicsComponents.numElementsPerThread) {
		int32_t offset = i /** numElementsPerThread*/;
		futures.push_back(
			std::async(std::launch::async,
				SoA_VerletResolution
				, physicsComponents.positions.data()
				, physicsComponents.oldPositions.data()
				, physicsComponents.forces.data()
				, physicsComponents.masses.data()
				, physicsComponents.DAMPING
				, physicsComponents.FIXED_TIME_STEP2
				, offset
				, physicsComponents.numElementsPerThread
				, max
			)
		);
	}

	for (auto& handle : futures) {
		handle.wait();
	}
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

#pragma omp simd
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

void PhysicsEngine::SIMD_SoA_ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	PhysicsComponents& physicsComponents = ecs.GetSingletonComponent<PhysicsComponents>();

#pragma omp simd simdlen(8)
	for (int32_t i = 0; i < max; i++)
	{
		// assuming to always add gravity
		physicsComponents.forces[i] = VgMath::Vector3(0.0, -9.8, 0.0);

		VgMath::Vector3 tempPos = physicsComponents.positions[i];
		VgMath::Vector3 accel = physicsComponents.forces[i] / physicsComponents.masses[i];
		physicsComponents.positions[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions[i])
			+ (accel * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.oldPositions[i] = tempPos;

		//// impulse only in one frame
		//physicsComponents.forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
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

bool PhysicsEngine::CheckEquality(
	std::vector<PhysicsComponent>& A
	, std::vector<PhysicsComponent>& B
) {
	bool bEquals = false;

	int32_t ANum = A.size();
	int32_t BNum = B.size();

	if (ANum != BNum)
	{
		return bEquals;
	}

	bEquals = true;

	for (size_t i = 0; i < ANum; i++)
	{
		if (A[i].oldPosition.x != B[i].oldPosition.x
			&& A[i].oldPosition.y != B[i].oldPosition.y
			&& A[i].oldPosition.z != B[i].oldPosition.z
			&& A[i].position.x != B[i].position.x
			&& A[i].position.y != B[i].position.y
			&& A[i].position.z != B[i].position.z
		) {
			bEquals = false;
			break;
		}
	}

	return bEquals;
}

void PhysicsEngine::ResetPositions(
	std::vector<PhysicsComponent>& A
) {
	for (size_t i = 0; i < A.size(); i++)
	{
		A[i].oldPosition.x = 0.0;
		A[i].oldPosition.y = 0.0;
		A[i].oldPosition.z = 0.0;

		A[i].position.x = 0.0;
		A[i].position.y = 0.0;
		A[i].position.z = 0.0;
	}
}

// SoA

bool PhysicsEngine::CheckEquality(
	PhysicsComponents& A
	, PhysicsComponents& B
) {
	bool bEquals = false;

	int32_t ANum = A.positions.size();
	int32_t BNum = B.positions.size();

	if (ANum != BNum)
	{
		return bEquals;
	}

	bEquals = true;

	for (size_t i = 0; i < ANum; i++)
	{
		if (A.oldPositions[i].x != B.oldPositions[i].x
			&& A.oldPositions[i].y != B.oldPositions[i].y
			&& A.oldPositions[i].z != B.oldPositions[i].z
			&& A.positions[i].x != B.positions[i].x
			&& A.positions[i].y != B.positions[i].y
			&& A.positions[i].z != B.positions[i].z
		) {
			bEquals = false;
			break;
		}
	}

	return bEquals;
}

void PhysicsEngine::ResetPositions(
	PhysicsComponents& A
) {
	for (size_t i = 0; i < A.positions.size(); i++)
	{
		A.oldPositions[i].x = 0.0;
		A.oldPositions[i].y = 0.0;
		A.oldPositions[i].z = 0.0;

		A.positions[i].x = 0.0;
		A.positions[i].y = 0.0;
		A.positions[i].z = 0.0;
	}
}
