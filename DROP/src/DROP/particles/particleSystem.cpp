#include "particleSystem.h"
#include "DROP/particles/particle.h"
#include "DROP/particles/physicsBasedParticle.h"

using namespace Drop;

void ParticleSystem::Update(ECS& ecs, const float deltaTime) {

	std::vector<ParticleEmitter>& particleEmitters = ecs.GetComponentPool<ParticleEmitter>().Data();
	ParticleSystem::UpdateParticles(
		particleEmitters
		, deltaTime
	);
}
void ParticleSystem::UpdatePB(ECS& ecs, const float deltaTime) {

	std::vector<PBParticleEmitter>& particleEmitters = ecs.GetComponentPool<PBParticleEmitter>().Data();
	ParticleSystem::UpdatePBParticles(
		particleEmitters
		, deltaTime
	);
}


void ParticleSystem::UpdateParticles(
	std::vector<ParticleEmitter>& particleEmitters
	, const float deltaTime
) {
	for (size_t i = 0; i < particleEmitters.size(); i++) {
		ParticleEmitter& particleEmitter = particleEmitters[i];

		UpdateParticles(
			particleEmitter.particles
			, particleEmitter.numberOfParticles
			, deltaTime
		);
	}
}

void ParticleSystem::UpdatePBParticles(
	std::vector<PBParticleEmitter>& particleEmitters
	, const float deltaTime
) {
	for (size_t i = 0; i < particleEmitters.size(); i++) {
		PBParticleEmitter& particleEmitter = particleEmitters[i];

		UpdateParticles(
			particleEmitter.particles
			, particleEmitter.numberOfParticles
			, deltaTime
		);
	}
}