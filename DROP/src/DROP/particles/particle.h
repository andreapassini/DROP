#pragma once

#include "DROP/math/mat3.h"
#include "DROP/math/vector4.h"
#include "DROP/math/surface.h"

#include "DROP/rendering/material.h"

using namespace VgMath;

#define MAX_PARTICLES 1000

struct Particle {
	Point3 position{ 0.0f };
	float size = 1.0f;
	Vector3 speed{ 0.0f };

	float age = 0.0f; // seconds
	float lifeTime = 0.f; // seconds
	float lerpFactor = 0.0f;
	bool isActive = false;

	Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };

	TextureID textureID = 0;

	float startsize = 1.0f;
	float endsize = 0.0f;
	Vector3 startSpeed{ 1.0f };
	Vector3 endSpeed{ 0.0f };

	Vector4 startColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 endColor{ 1.0f, 1.0f, 1.0f, 0.0f };
};

struct ParticleStartValues {
	Surface spawningSurface;

	// In seconds
	float lifeTime = 10.0f;

	float startsize = 1.0f;
	float endsize = 1.0f;
	
	Vector3 startSpeed{ 0.0f, 5.0f, 0.0f };
	Vector3 endSpeed{ 0.0f, 0.0f, 0.0f };

	Vector4 startColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 endColor{ 1.0f, 1.0f, 1.0f, 0.0f };

	TextureID textureID = 0;
};

struct ParticleEmitter {
	//  If we keep a particle pool for each particleEmitter, 
	//	 we can operate only on those specific particles, like terminating them
	//	 maybe we can have a particle arena in order to have different particle pool size 
	//	 depending on the different effects we want
	Particle particles[MAX_PARTICLES]; 
	uint32_t numberOfParticles = MAX_PARTICLES;
	uint32_t particleToEmitEachTime = 10;
	uint32_t lastIndex = 0;

	ParticleStartValues spawningValues;
};

void EmitParticles(
	ParticleEmitter& particleEmitter
	, Transform& transform
);

void UpdateParticles(
	Particle* const particles
	, const uint32_t size
	, const float deltaTime
);
