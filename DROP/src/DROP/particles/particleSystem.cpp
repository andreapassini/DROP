#include "particleSystem.h"
#include "../particles/particle.h"

using namespace Drop;

void ParticleSystem::Update(ECS& ecs, const float deltaTime) {

	std::vector<ParticleEmitter>& particleEmitters = ecs.GetComponentPool<ParticleEmitter>().Data();

	for (size_t i = 0; i < particleEmitters.size(); i++) {
		ParticleEmitter& particleEmitter = particleEmitters[i];

		UpdateParticles(
			particleEmitter.particles
			, particleEmitter.numberOfParticles
			, deltaTime
		);
	}
}