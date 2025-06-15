#pragma once

#include "DROP/ECS/beecs.h"


using namespace bseecs;

struct  ParticleEmitter;
struct  PBParticleEmitter;

namespace Drop {

namespace ParticleSystem {

void Update(
	ECS& ecs
	, const float deltaTime
);

void UpdatePB(
	ECS& ecs
	, const float deltaTime
);

void UpdateParticles(
	std::vector<ParticleEmitter>& particleEmitters
	, const float deltaTime
);

void UpdatePBParticles(
	std::vector<PBParticleEmitter>& particleEmitters
	, const float deltaTime
);
	
}

}