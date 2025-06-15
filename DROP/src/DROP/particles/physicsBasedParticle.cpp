#include "physicsBasedParticle.h"

void EmitParticle(
	PBParticleEmitter& particleEmitter
	, Transform& emitterTransform
) {
	for (uint32_t i = 0; i < particleEmitter.particleToEmitEachTime; i++) {
		// Ring buffer
		uint32_t currentIndex = particleEmitter.lastIndex;
		particleEmitter.lastIndex++;
		if (particleEmitter.lastIndex >= particleEmitter.numberOfParticles) {
			particleEmitter.lastIndex = 0;
		}

		PhysicsBasedParticle& particle = particleEmitter.particles[currentIndex];
		PBParticleSpawningValue& spawningValues = particleEmitter.spawningValues;
		spawningValues.spawningSurface.m_Transform = &emitterTransform;
		particle.oldPosition = (Vector3)spawningValues.spawningSurface.RandomPointOnSurface();
		particle.position = particle.oldPosition;

		particle.mass = lerp(
			spawningValues.minMass
			, spawningValues.maxMass
			, RandomBetween0and1()
		);
		particle.size = particle.mass;

		// #TODO add starting force if needed

		particle.age = 0.0f;
		particle.bIsActive = true;
		particle.lifetime = 
			(0.5f * spawningValues.lifeTime) 
			+ (0.5f * spawningValues.lifeTime) 
			* RandomBetween0and1();

		particle.color = spawningValues.startingColor;
		particle.textureID = spawningValues.textureID;
	}
}

void UpdateParticle(
	PhysicsBasedParticle* const particles
	, const uint32_t size
	, const float deltaTime
) {
	for (uint32_t i = 0; i < size; i++) {
		PhysicsBasedParticle& particle = particles[i];
		if (!particle.bIsActive) {
			continue;
		}

		particle.age += deltaTime;
		if (particle.age > particle.lifetime) {
			particle.bIsActive = false;
			continue;
		}

		// update particle position
		if (particle.bIsStatic)
		{
			particle.force = Vector3(0.0, 0.0, 0.0);
			return;
		}

		Vector3 tempPos = particle.position;
		Vector3 accel = particle.force / particle.mass;
		particle.position = 
			((2.0f - particle.DAMPING) * particle.position)
			- ((1.0f - particle.DAMPING) * particle.oldPosition)
			+ (accel * particle.FIXED_TIME_STEP2);

		particle.oldPosition = tempPos;

		particle.force = VgMath::Vector3(0.0, 0.0, 0.0);
		particle.force.y = (GRAVITY_ACCELERATION * particle.mass);
	}
}
