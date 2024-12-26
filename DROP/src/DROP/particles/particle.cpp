#include "../particles/particle.h"

void EmitParticles(ParticleEmitter& particleEmitter)
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

		particle.position = spawningValues.position;

		particle.lifeTime = spawningValues.lifeTime;

		particle.startsize = spawningValues.startsize;
		particle.endsize = spawningValues.endsize;
		particle.startSpeed = spawningValues.startSpeed;
		particle.endSpeed = spawningValues.endSpeed;
		particle.startColor = spawningValues.startColor;
		particle.endColor = spawningValues.endColor;
		particle.startColorAlpha = spawningValues.startColorAlpha;
		particle.endColorAlpha = spawningValues.endColorAlpha;

		particle.textureID = spawningValues.textureID;
	}
}

void UpdateParticles(Particle* const particles, uint32_t size, const float deltaTime){
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

		particle.position += (particle.speed * deltaTime);

		particle.lerpFactor = particle.age / particle.lifeTime;
		particle.size = particle.startsize + particle.lerpFactor * (particle.endsize - particle.startsize);
		particle.speed = particle.startSpeed + particle.lerpFactor * (particle.endSpeed - particle.startSpeed);
		particle.color = particle.startColor + particle.lerpFactor * (particle.endColor - particle.startColor);
		particle.colorAlpha = particle.startColorAlpha + particle.lerpFactor * (particle.endColorAlpha - particle.startColorAlpha);
	}
}