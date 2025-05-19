#include "DROP/particles/particle.h"

void EmitParticles(
	ParticleEmitter& particleEmitter
	, Transform& transform)
{
	for (uint32_t i = 0; i < particleEmitter.particleToEmitEachTime; i++)
	{
		particleEmitter.lastIndex++;
		if (particleEmitter.lastIndex >= particleEmitter.numberOfParticles)
		{
			particleEmitter.lastIndex = 0;
		}

		Particle& particle = particleEmitter.particles[particleEmitter.lastIndex];
		ParticleStartValues& spawningValues = particleEmitter.spawningValues;

		particle.age = 0.0f;
		particle.isActive = true;

		spawningValues.spawningSurface.m_Transform = &transform;
		particle.position = spawningValues.spawningSurface.RandomPointOnSurface();

		// this could be done better, but this is simpler 
		particle.lifeTime = (0.5f * spawningValues.lifeTime) + (0.5f * spawningValues.lifeTime) * RandomBetween0and1();

		particle.startsize = spawningValues.startsize;
		particle.endsize = spawningValues.endsize;
		particle.startSpeed = spawningValues.startSpeed;
		particle.endSpeed = spawningValues.endSpeed;
		particle.startColor = spawningValues.startColor;
		particle.endColor = spawningValues.endColor;

		particle.textureID = spawningValues.textureID;
	}
}

void UpdateParticles(
	Particle* const particles
	, uint32_t size
	, const float deltaTime
){
	for (uint32_t i = 0; i < size; i++) {
		Particle& particle = particles[i];
		
		if (!particle.isActive) {
			continue;
		}

		particle.age += deltaTime;

		if (particle.age > particle.lifeTime) {
			particle.isActive = false;
			continue;
		}

		// For now Euclidean interpolation, move this to Verlet as in the Physics system
		particle.position = particle.position + (particle.speed * deltaTime);

		particle.lerpFactor = particle.age / particle.lifeTime;
		particle.size = particle.startsize + particle.lerpFactor * (particle.endsize - particle.startsize);
		particle.speed = particle.startSpeed + particle.lerpFactor * (particle.endSpeed - particle.startSpeed);
		particle.color = particle.startColor + particle.lerpFactor * (particle.endColor - particle.startColor);
	}
}