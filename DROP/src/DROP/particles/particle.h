#pragma once

#include "../math/mat3.h"

using namespace VgMath;

#define MAX_PARTICLES 100

struct Particle {
	Vector3 position{ 0.0f };
	float size = 1.0f;
	Vector3 speed{ 0.0f };

	float age = 0.0f; // seconds
	float lifeTime = 0.f; // seconds
	float lerpFactor = 0.0f;
	bool isActive = false;

	Vector3 color{ 1.0f, 1.0f, 1.0f };
	float colorAlpha = 1.0f;

	uint32_t textureID = 0;

	float startsize = 1.0f;
	float endsize = 0.0f;
	Vector3 startSpeed{ 1.0f };
	Vector3 endSpeed{ 0.0f };
	Vector3 startColor{ 1.0f, 1.0f, 1.0f };
	Vector3 endColor{ 1.0f, 1.0f, 1.0f };
	float startColorAlpha = 1.0f;
	float endColorAlpha = 1.0f;
};

struct ParticleStartValues {
	Vector3 position{ 0.0f };

	// In Milliseconds
	float lifeTime = 10.0f;

	float startsize = 1.0f;
	float endsize = 1.0f;
	
	Vector3 startSpeed{ 0.0f, 5.0f, 0.0f };
	Vector3 endSpeed{ 0.0f, 0.0f, 0.0f };

	Vector3 startColor{ 1.0f, 1.0f, 1.0f };
	Vector3 endColor{ 1.0f, 1.0f, 1.0f };
	float startColorAlpha = 1.0f;
	float endColorAlpha = 0.0f;

	uint32_t textureID = 0;
};

struct ParticleEmitter {
	Particle particles[MAX_PARTICLES];
	uint32_t numberOfParticles = MAX_PARTICLES;
	uint32_t particleToEmitEachTime = 10;
	uint32_t lastIndex = 0;

	ParticleStartValues spawningValues;
};

void EmitParticles(ParticleEmitter& particleEmitter);

void UpdateParticles(
	Particle* const particles
	, const uint32_t size
	, const float deltaTime
);
