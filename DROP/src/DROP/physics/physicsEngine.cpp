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
			- ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition)
			+ (accel * currentPhysicsComp.FIXED_TIME_STEP2);

		//VgMath::Vector3 accel = currentPhysicsComp.force / currentPhysicsComp.mass;

		//VgMath::Vector3 DAMPxPos = ((2.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.position);
		//VgMath::Vector3 DAMPxOldPos = ((1.0f - currentPhysicsComp.DAMPING) * currentPhysicsComp.oldPosition);
		//VgMath::Vector3 Accel = (accel * currentPhysicsComp.FIXED_TIME_STEP2);

		//currentPhysicsComp.position = DAMPxPos
		//	- DAMPxOldPos
		//	+ Accel;
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

		VgMath::Vector3 tempPos = positions[i];
		VgMath::Vector3 accel = forces[i] / masses[i];
		positions[i] = ((2.0f - DAMPING) * positions[i])
			- ((1.0f - DAMPING) * oldPositions[i])
			+ (accel * FIXED_TIME_STEP2);
		oldPositions[i] = tempPos;

		// impulse only in one frame
		forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
	}
}


void SoA_VerletResolution_Reordered(
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

		VgMath::Vector3 DAMPxPos = ((2.0f - DAMPING) * positions[i]);

		VgMath::Vector3 DAMPxOldPos = ((1.0f - DAMPING) * oldPositions[i]);

		VgMath::Vector3 Accel = (accel * FIXED_TIME_STEP2);

		positions[i] = DAMPxPos
			- DAMPxOldPos
			+ Accel;

		oldPositions[i] = tempPos;

		// impulse only in one frame
		forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
	}
}

void SoA_VerletResolution_AllDim(
	float* positions_x
	, float* positions_y
	, float* positions_z
	, float* oldPositions_x
	, float* oldPositions_y
	, float* oldPositions_z
	, float* forces_x
	, float* forces_y
	, float* forces_z
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
		forces_x[i] = 0.0f;
		forces_y[i] = -9.8;
		forces_z[i] = 0.0f;

		//VgMath::Vector3 tempPos = positions[i];
		float tempPos_x = positions_x[i];
		float tempPos_y = positions_y[i];
		float tempPos_z = positions_z[i];

		//VgMath::Vector3 accel = forces[i] / masses[i];
		float accel_x = forces_x[i] / masses[i];
		float accel_y = forces_y[i] / masses[i];
		float accel_z = forces_z[i] / masses[i];

		positions_x[i] = ((2.0f - DAMPING) * positions_x[i])
			- ((1.0f - DAMPING) * oldPositions_x[i])
			+ (accel_x * FIXED_TIME_STEP2);
		positions_y[i] = ((2.0f - DAMPING) * positions_y[i])
			- ((1.0f - DAMPING) * oldPositions_y[i])
			+ (accel_y * FIXED_TIME_STEP2);
		positions_z[i] = ((2.0f - DAMPING) * positions_z[i])
			- ((1.0f - DAMPING) * oldPositions_z[i])
			+ (accel_z * FIXED_TIME_STEP2);

		oldPositions_x[i] = tempPos_x;
		oldPositions_y[i] = tempPos_y;
		oldPositions_z[i] = tempPos_z;

		// impulse only in one frame
		//forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
		forces_x[i] = 0.0f;
		forces_y[i] = 0.0f;
		forces_z[i] = 0.0f;
	}
}

void SoA_HandSIMD_VerletResolution(
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

	float const _2minusDAMPING = (2.0f - DAMPING);
	float const _1minusDAMPING = (1.0f - DAMPING);

//#pragma omp simd
	int32_t i;
	for (i = offset; i+1 < numOfElements; i+=2)
	{
		// assuming to always add gravity
		//forces[i] = VgMath::Vector3(0.0f, -9.8f, 0.0f);

		__m256 positions256 = {
			positions[i].x, positions[i].y, positions[i].z
			, positions[i + 1].x, positions[i + 1].y , positions[i + 1].z
			, 0.0f, 0.0f 
		};

		__m256 tempDumping2minus = {
			_2minusDAMPING, _2minusDAMPING, _2minusDAMPING
			, _2minusDAMPING, _2minusDAMPING, _2minusDAMPING
			, 0.0f, 0.0f
		};

		__m256 tempDumping1minus = {
			_1minusDAMPING, _1minusDAMPING, _1minusDAMPING
			, _1minusDAMPING, _1minusDAMPING, _1minusDAMPING
			, 0.0f, 0.0f
		};

		// 256 / 32 = 8
		// vec3 -> 2 unused floats each times 
		__m256 forces256 = {
			0.0f, -9.8f, 0.0f
			, 0.0f, -9.8f, 0.0f
			, 0.0f, 0.0f
		};

		//VgMath::Vector3 accel = forces[i] / masses[i];
		__m256 masses256 = { 
			masses[i], masses[i], masses[i]
			, masses[i+1], masses[i + 1], masses[i + 1]
			, 0.0, 0.0
		};

		//VgMath::Vector3 DAMPxPos = ((2.0f - DAMPING) * positions[i]);
		__m256 DAMPxPos256 = _mm256_sub_ps(positions256, tempDumping2minus);

		VgMath::Vector3 tempOldPosI = positions[i];
		VgMath::Vector3 tempOldPosIadd1 = positions[i+1];
		// Not needed
		//__m256 tempPos256 = { 
		//	positions[i].x, positions[i].y, positions[i].z
		//	, positions[i + 1].x, positions[i + 1].y , positions[i + 1].z
		//	, 0.0f, 0.0f 
		//};

		__m256 accel256 = _mm256_div_ps(forces256, masses256);

		//VgMath::Vector3 DAMPxOldPos = ((1.0f - DAMPING) * oldPositions[i]);
		__m256 DAMPxOldPos256 = _mm256_mul_ps(DAMPxPos256, tempDumping1minus);

		//VgMath::Vector3 Accel = (accel * FIXED_TIME_STEP2);
		__m256 tempFIXED_TIME_STEP2256 = { FIXED_TIME_STEP2 };
		__m256 Accel256 = _mm256_mul_ps(accel256, tempFIXED_TIME_STEP2256);

		//positions[i] = DAMPxPos
		//	- DAMPxOldPos
		//	+ Accel;
		positions256 = _mm256_sub_ps(DAMPxPos256, DAMPxOldPos256);

		oldPositions[i] = tempOldPosI;
		oldPositions[i+1] = tempOldPosIadd1;

		positions256 = _mm256_add_ps(positions256, Accel256);

		// impulse only in one frame
		forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
		forces[i + 1] = VgMath::Vector3(0.0, 0.0, 0.0);

		float tempPos[8];
		_mm256_store_ps(&tempPos[0], positions256);

		positions[i] = VgMath::Vector3(tempPos[0], tempPos[1], tempPos[2]);
		positions[i+1] = VgMath::Vector3(tempPos[3], tempPos[4], tempPos[5]);
		// ignoring [6][7] since they are padding
	} 

	// Handle the edge case of the last i != (numOfElements - 1)
	if (i != (numOfElements - 1))
	{
		i = numOfElements - 1;

		// assuming to always add gravity
		forces[i] = VgMath::Vector3(0.0f, -9.8f, 0.0f);

		VgMath::Vector3 accel = forces[i] / masses[i];

		VgMath::Vector3 tempOldPosI = positions[i];

		VgMath::Vector3 DAMPxPos = ((2.0f - DAMPING) * positions[i]);

		VgMath::Vector3 DAMPxOldPos = ((1.0f - DAMPING) * oldPositions[i]);

		VgMath::Vector3 Accel = (accel * FIXED_TIME_STEP2);

		positions[i] = DAMPxPos
			- DAMPxOldPos
			+ Accel;

		oldPositions[i] = tempOldPosI;

		// impulse only in one frame
		forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
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
		physicsComponents.forces_x[i] = 0.0f;
		physicsComponents.forces_y[i] = -9.8f;
		physicsComponents.forces_z[i] = 0.0f;

		//VgMath::Vector3 tempPos = physicsComponents.positions[i];
		float tempPos_x = physicsComponents.positions_x[i];
		float tempPos_y = physicsComponents.positions_y[i];
		float tempPos_z = physicsComponents.positions_z[i];

		//VgMath::Vector3 accel = physicsComponents.forces[i] / physicsComponents.masses[i];
		float accel_x = physicsComponents.forces_x[i] / physicsComponents.masses[i];
		float accel_y = physicsComponents.forces_y[i] / physicsComponents.masses[i];
		float accel_z = physicsComponents.forces_z[i] / physicsComponents.masses[i];

		physicsComponents.positions_x[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_x[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_x[i])
			+ (accel_x * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.positions_y[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_y[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_y[i])
			+ (accel_y * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.positions_z[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_z[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_z[i])
			+ (accel_z * physicsComponents.FIXED_TIME_STEP2);

		physicsComponents.oldPositions_x[i] = tempPos_x;
		physicsComponents.oldPositions_y[i] = tempPos_y;
		physicsComponents.oldPositions_z[i] = tempPos_z;

		// impulse only in one frame
		//physicsComponents.forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
		physicsComponents.forces_x[i] = 0.0f;
		physicsComponents.forces_y[i] = 0.0f;
		physicsComponents.forces_z[i] = 0.0f;
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
			//std::async(std::launch::async,
			//	//SoA_VerletResolution
			//	SoA_HandSIMD_VerletResolution
			//	, physicsComponents.positions.data()
			//	, physicsComponents.oldPositions.data()
			//	, physicsComponents.forces.data()
			//	, physicsComponents.masses.data()
			//	, physicsComponents.DAMPING
			//	, physicsComponents.FIXED_TIME_STEP2
			//	, offset
			//	, physicsComponents.numElementsPerThread
			//	, max
			//)
			std::async(std::launch::async,
				SoA_VerletResolution_AllDim
				, physicsComponents.positions_x.data()
				, physicsComponents.positions_y.data()
				, physicsComponents.positions_z.data()
				, physicsComponents.oldPositions_x.data()
				, physicsComponents.oldPositions_y.data()
				, physicsComponents.oldPositions_z.data()
				, physicsComponents.forces_x.data()
				, physicsComponents.forces_y.data()
				, physicsComponents.forces_z.data()
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

#pragma omp simd
	for (int32_t i = 0; i < max; i++)
	{
		// assuming to always add gravity
		physicsComponents.forces_x[i] = 0.0f;
		physicsComponents.forces_y[i] = -9.8f;
		physicsComponents.forces_z[i] = 0.0f;

		//VgMath::Vector3 tempPos = physicsComponents.positions[i];
		float tempPos_x = physicsComponents.positions_x[i];
		float tempPos_y = physicsComponents.positions_y[i];
		float tempPos_z = physicsComponents.positions_z[i];

		//VgMath::Vector3 accel = physicsComponents.forces[i] / physicsComponents.masses[i];
		float accel_x = physicsComponents.forces_x[i] / physicsComponents.masses[i];
		float accel_y = physicsComponents.forces_y[i] / physicsComponents.masses[i];
		float accel_z = physicsComponents.forces_z[i] / physicsComponents.masses[i];

		physicsComponents.positions_x[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_x[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_x[i])
			+ (accel_x * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.positions_y[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_y[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_y[i])
			+ (accel_y * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.positions_z[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_z[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_z[i])
			+ (accel_z * physicsComponents.FIXED_TIME_STEP2);

		physicsComponents.oldPositions_x[i] = tempPos_x;
		physicsComponents.oldPositions_y[i] = tempPos_y;
		physicsComponents.oldPositions_z[i] = tempPos_z;

		// impulse only in one frame
		//physicsComponents.forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
		physicsComponents.forces_x[i] = 0.0f;
		physicsComponents.forces_y[i] = 0.0f;
		physicsComponents.forces_z[i] = 0.0f;

	}
}

void PhysicsEngine::SIMD_ByHand_SoA_ApplyForces(
	ECS& ecs
	, const int32_t max
) {
	PhysicsComponents& physicsComponents = ecs.GetSingletonComponent<PhysicsComponents>();

	__m256 _mOneMinusDamping = _mm256_set1_ps(1.0f - physicsComponents.DAMPING);
	__m256 _mTwoMinusDamping = _mm256_set1_ps(2.0f - physicsComponents.DAMPING);
	__m256 _mFixedTimeStep = _mm256_set1_ps(physicsComponents.FIXED_TIME_STEP);
	__m256 _mFixedTimeStep2 = _mm256_set1_ps(physicsComponents.FIXED_TIME_STEP2);

	//for (int32_t i = 0; i < max; i++)
	int32_t i = 0;
	for (i = 0; i < max - 7; i+=8) // AVX2 -> 256 bits ops -> 8 wide
	{
		// assuming to always add gravity
		__m256 _mForce_x = _mm256_set1_ps(0.0f);
		__m256 _mForce_y = _mm256_set1_ps(-9.8f);
		__m256 _mForce_z = _mm256_set1_ps(0.0f);

		__m256 _mMass = _mm256_set_ps(
			physicsComponents.masses[i]
			, physicsComponents.masses[i+1]
			, physicsComponents.masses[i+2]
			, physicsComponents.masses[i+3]
			, physicsComponents.masses[i+4]
			, physicsComponents.masses[i+5]
			, physicsComponents.masses[i+6]
			, physicsComponents.masses[i+7]
		);

		__m256 _mOldPosition_x = _mm256_set_ps(
			physicsComponents.oldPositions_x[i]
			, physicsComponents.oldPositions_x[i+1]
			, physicsComponents.oldPositions_x[i+2]
			, physicsComponents.oldPositions_x[i+3]
			, physicsComponents.oldPositions_x[i+4]
			, physicsComponents.oldPositions_x[i+5]
			, physicsComponents.oldPositions_x[i+6]
			, physicsComponents.oldPositions_x[i+7]
		);
		__m256 _mOldPosition_y = _mm256_set_ps(
			physicsComponents.oldPositions_y[i]
			, physicsComponents.oldPositions_y[i+1]
			, physicsComponents.oldPositions_y[i+2]
			, physicsComponents.oldPositions_y[i+3]
			, physicsComponents.oldPositions_y[i+4]
			, physicsComponents.oldPositions_y[i+5]
			, physicsComponents.oldPositions_y[i+6]
			, physicsComponents.oldPositions_y[i+7]
		);
		__m256 _mOldPosition_z = _mm256_set_ps(
			physicsComponents.oldPositions_z[i]
			, physicsComponents.oldPositions_z[i+1]
			, physicsComponents.oldPositions_z[i+2]
			, physicsComponents.oldPositions_z[i+3]
			, physicsComponents.oldPositions_z[i+4]
			, physicsComponents.oldPositions_z[i+5]
			, physicsComponents.oldPositions_z[i+6]
			, physicsComponents.oldPositions_z[i+7]
		);

		//float tempPos_x = physicsComponents.positions_x[i];
		__m256 _mPosition_x = _mm256_set_ps(
			physicsComponents.positions_x[i]
			, physicsComponents.positions_x[i+1]
			, physicsComponents.positions_x[i+2]
			, physicsComponents.positions_x[i+3]
			, physicsComponents.positions_x[i+4]
			, physicsComponents.positions_x[i+5]
			, physicsComponents.positions_x[i+6]
			, physicsComponents.positions_x[i+7]
		);
		//float tempPos_y = physicsComponents.positions_y[i];
		__m256 _mPosition_y = _mm256_set_ps(
			physicsComponents.positions_y[i]
			, physicsComponents.positions_y[i+1]
			, physicsComponents.positions_y[i+2]
			, physicsComponents.positions_y[i+3]
			, physicsComponents.positions_y[i+4]
			, physicsComponents.positions_y[i+5]
			, physicsComponents.positions_y[i+6]
			, physicsComponents.positions_y[i+7]
		);
		//float tempPos_z = physicsComponents.positions_z[i];
		__m256 _mPosition_z = _mm256_set_ps(
			physicsComponents.positions_z[i]
			, physicsComponents.positions_z[i+1]
			, physicsComponents.positions_z[i+2]
			, physicsComponents.positions_z[i+3]
			, physicsComponents.positions_z[i+4]
			, physicsComponents.positions_z[i+5]
			, physicsComponents.positions_z[i+6]
			, physicsComponents.positions_z[i+7]
		);

		__m256 _mTempoPos_x = _mPosition_x;
		__m256 _mTempoPos_y = _mPosition_y;
		__m256 _mTempoPos_z = _mPosition_z;

		//VgMath::Vector3 accel = physicsComponents.forces[i] / physicsComponents.masses[i];
		//float accel_x = physicsComponents.forces_x[i] / physicsComponents.masses[i];
		__m256 _mAccel_x = _mm256_div_ps(_mForce_x, _mMass);
		//float accel_y = physicsComponents.forces_y[i] / physicsComponents.masses[i];
		__m256 _mAccel_y = _mm256_div_ps(_mForce_y, _mMass);
		//float accel_z = physicsComponents.forces_z[i] / physicsComponents.masses[i];
		__m256 _mAccel_z = _mm256_div_ps(_mForce_z, _mMass);

		//physicsComponents.positions_x[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_x[i])
		//	- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_x[i])
		//	+ (accel_x * physicsComponents.FIXED_TIME_STEP2);
		//physicsComponents.positions_y[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_y[i])
		//	- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_y[i])
		//	+ (accel_y * physicsComponents.FIXED_TIME_STEP2);
		//physicsComponents.positions_z[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_z[i])
		//	- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_z[i])
		//	+ (accel_z * physicsComponents.FIXED_TIME_STEP2);

		_mPosition_x = _mm256_mul_ps(_mTwoMinusDamping, _mPosition_x);
		_mPosition_y = _mm256_mul_ps(_mTwoMinusDamping, _mPosition_y);
		_mPosition_z = _mm256_mul_ps(_mTwoMinusDamping, _mPosition_z);

		_mOldPosition_x = _mm256_mul_ps(_mOneMinusDamping, _mOldPosition_x);
		_mOldPosition_y = _mm256_mul_ps(_mOneMinusDamping, _mOldPosition_y);
		_mOldPosition_z = _mm256_mul_ps(_mOneMinusDamping, _mOldPosition_z);

		_mAccel_x = _mm256_mul_ps(_mAccel_x, _mFixedTimeStep2);
		_mAccel_y = _mm256_mul_ps(_mAccel_y, _mFixedTimeStep2);
		_mAccel_z = _mm256_mul_ps(_mAccel_z, _mFixedTimeStep2);

		_mPosition_x = _mm256_sub_ps(_mPosition_x, _mOldPosition_x);
		_mPosition_y = _mm256_sub_ps(_mPosition_y, _mOldPosition_y);
		_mPosition_z = _mm256_sub_ps(_mPosition_z, _mOldPosition_z);

		_mPosition_x = _mm256_add_ps(_mPosition_x, _mAccel_x);
		_mPosition_y = _mm256_add_ps(_mPosition_y, _mAccel_y);
		_mPosition_z = _mm256_add_ps(_mPosition_z, _mAccel_z);


		physicsComponents.oldPositions_x[i] = float(_mPosition_x.m256_f32[i]);
		physicsComponents.oldPositions_x[i+1] = float(_mPosition_x.m256_f32[i+1]);
		physicsComponents.oldPositions_x[i+2] = float(_mPosition_x.m256_f32[i+2]);
		physicsComponents.oldPositions_x[i+3] = float(_mPosition_x.m256_f32[i+3]);
		physicsComponents.oldPositions_x[i+4] = float(_mPosition_x.m256_f32[i+4]);
		physicsComponents.oldPositions_x[i+5] = float(_mPosition_x.m256_f32[i+5]);
		physicsComponents.oldPositions_x[i+6] = float(_mPosition_x.m256_f32[i+6]);
		physicsComponents.oldPositions_x[i+7] = float(_mPosition_x.m256_f32[i+7]);

		physicsComponents.oldPositions_y[i] = float(_mPosition_y.m256_f32[i]);
		physicsComponents.oldPositions_y[i+1] = float(_mPosition_y.m256_f32[i+1]);
		physicsComponents.oldPositions_y[i+2] = float(_mPosition_y.m256_f32[i+2]);
		physicsComponents.oldPositions_y[i+3] = float(_mPosition_y.m256_f32[i+3]);
		physicsComponents.oldPositions_y[i+4] = float(_mPosition_y.m256_f32[i+4]);
		physicsComponents.oldPositions_y[i+5] = float(_mPosition_y.m256_f32[i+5]);
		physicsComponents.oldPositions_y[i+6] = float(_mPosition_y.m256_f32[i+6]);
		physicsComponents.oldPositions_y[i+7] = float(_mPosition_y.m256_f32[i+7]);

		physicsComponents.oldPositions_z[i] = float(_mPosition_z.m256_f32[i]);
		physicsComponents.oldPositions_z[i+1] = float(_mPosition_z.m256_f32[i+1]);
		physicsComponents.oldPositions_z[i+2] = float(_mPosition_z.m256_f32[i+2]);
		physicsComponents.oldPositions_z[i+3] = float(_mPosition_z.m256_f32[i+3]);
		physicsComponents.oldPositions_z[i+4] = float(_mPosition_z.m256_f32[i+4]);
		physicsComponents.oldPositions_z[i+5] = float(_mPosition_z.m256_f32[i+5]);
		physicsComponents.oldPositions_z[i+6] = float(_mPosition_z.m256_f32[i+6]);
		physicsComponents.oldPositions_z[i+7] = float(_mPosition_z.m256_f32[i+7]);

		// impulse only in one frame
		//physicsComponents.forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
		physicsComponents.forces_x[i] = 0.0f;
		physicsComponents.forces_x[i+1] = 0.0f;
		physicsComponents.forces_x[i+2] = 0.0f;
		physicsComponents.forces_x[i+3] = 0.0f;
		physicsComponents.forces_x[i+4] = 0.0f;
		physicsComponents.forces_x[i+5] = 0.0f;
		physicsComponents.forces_x[i+6] = 0.0f;
		physicsComponents.forces_x[i+7] = 0.0f;

		physicsComponents.forces_y[i] = 0.0f;
		physicsComponents.forces_y[i+1] = 0.0f;
		physicsComponents.forces_y[i+2] = 0.0f;
		physicsComponents.forces_y[i+3] = 0.0f;
		physicsComponents.forces_y[i+4] = 0.0f;
		physicsComponents.forces_y[i+5] = 0.0f;
		physicsComponents.forces_y[i+6] = 0.0f;
		physicsComponents.forces_y[i+7] = 0.0f;

		physicsComponents.forces_z[i] = 0.0f;
		physicsComponents.forces_z[i+1] = 0.0f;
		physicsComponents.forces_z[i+2] = 0.0f;
		physicsComponents.forces_z[i+3] = 0.0f;
		physicsComponents.forces_z[i+4] = 0.0f;
		physicsComponents.forces_z[i+5] = 0.0f;
		physicsComponents.forces_z[i+6] = 0.0f;
		physicsComponents.forces_z[i+7] = 0.0f;
	}

	// Execute the last remaining ops
	for (i = i; i < max; i ++) // AVX2 -> 256 bits ops -> 8 wide
	{
		// assuming to always add gravity
		physicsComponents.forces_x[i] = 0.0f;
		physicsComponents.forces_y[i] = -9.8f;
		physicsComponents.forces_z[i] = 0.0f;

		//VgMath::Vector3 tempPos = physicsComponents.positions[i];
		float tempPos_x = physicsComponents.positions_x[i];
		float tempPos_y = physicsComponents.positions_y[i];
		float tempPos_z = physicsComponents.positions_z[i];

		//VgMath::Vector3 accel = physicsComponents.forces[i] / physicsComponents.masses[i];
		float accel_x = physicsComponents.forces_x[i] / physicsComponents.masses[i];
		float accel_y = physicsComponents.forces_y[i] / physicsComponents.masses[i];
		float accel_z = physicsComponents.forces_z[i] / physicsComponents.masses[i];

		physicsComponents.positions_x[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_x[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_x[i])
			+ (accel_x * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.positions_y[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_y[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_y[i])
			+ (accel_y * physicsComponents.FIXED_TIME_STEP2);
		physicsComponents.positions_z[i] = ((2.0f - physicsComponents.DAMPING) * physicsComponents.positions_z[i])
			- ((1.0f - physicsComponents.DAMPING) * physicsComponents.oldPositions_z[i])
			+ (accel_z * physicsComponents.FIXED_TIME_STEP2);

		physicsComponents.oldPositions_x[i] = tempPos_x;
		physicsComponents.oldPositions_y[i] = tempPos_y;
		physicsComponents.oldPositions_z[i] = tempPos_z;

		// impulse only in one frame
		//physicsComponents.forces[i] = VgMath::Vector3(0.0, 0.0, 0.0);
		physicsComponents.forces_x[i] = 0.0f;
		physicsComponents.forces_y[i] = 0.0f;
		physicsComponents.forces_z[i] = 0.0f;
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

	int32_t ANum = A.positions_x.size();
	int32_t BNum = B.positions_x.size();

	if (ANum != BNum)
	{
		return bEquals;
	}

	bEquals = true;

	for (size_t i = 0; i < ANum; i++)
	{
		if (VgMath::areEqual(A.oldPositions_x[i], B.oldPositions_x[i])
			&& VgMath::areEqual(A.oldPositions_y[i], B.oldPositions_y[i])
			&& VgMath::areEqual(A.oldPositions_z[i], B.oldPositions_z[i])
			&& VgMath::areEqual(A.positions_x[i], B.positions_x[i])
			&& VgMath::areEqual(A.positions_y[i], B.positions_y[i])
			&& VgMath::areEqual(A.positions_z[i], B.positions_z[i])
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
	for (size_t i = 0; i < A.positions_x.size(); i++)
	{
		A.oldPositions_x[i] = 0.0;
		A.oldPositions_y[i] = 0.0;
		A.oldPositions_z[i] = 0.0;

		A.positions_x[i] = 0.0;
		A.positions_y[i] = 0.0;
		A.positions_z[i] = 0.0;
	}
	//for (size_t i = 0; i < A.positions.size(); i++)
	//{
	//	A.oldPositions[i].x = 0.0;
	//	A.oldPositions[i].y = 0.0;
	//	A.oldPositions[i].z = 0.0;

	//	A.positions[i].x = 0.0;
	//	A.positions[i].y = 0.0;
	//	A.positions[i].z = 0.0;
	//}
}
