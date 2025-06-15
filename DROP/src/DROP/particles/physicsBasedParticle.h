#pragma once

#include "DROP/math/mat3.h"
#include "DROP/math/vector4.h"
#include "DROP/math/surface.h"

#include "DROP/rendering/material.h"
#include "DROP/rendering/staticMeshComponent.h"

using namespace VgMath;

#define MAX_PB_PARTICLES 1000
#define GRAVITY_ACCELERATION -9.8

// Using verlet integration method
// as in physicsObject
struct PhysicsBasedParticle {
	Vector3 oldPosition;
	Vector3 position;
	Vector3 force;
	float mass = 1.0f;

	bool bIsStatic = false;

	static constexpr float DAMPING = 0.01f; // how much to damp the simulation each frame
	static constexpr float FIXED_TIME_STEP = (1.0f / 30.0f);
	static constexpr float FIXED_TIME_STEP2 = (FIXED_TIME_STEP * FIXED_TIME_STEP);

	// lifetime 
	float lifetime = 0.0f;
	float age = 0.0f;
	bool bIsActive = false;

	// for the rendering use a simple billboard
	Vector4 color{ 1.0f }; // tint
	TextureID textureID = 0;
	float size = 1.0f; // set it to mass
};

struct PBParticleSpawningValue {
	// the surface will give us the starting pos
	Surface spawningSurface;
	float lifeTime = 10.0f;

	float minMass;
	float maxMass;

	Vector4 startingColor{1.0f};
	TextureID textureID = 0;
};

struct PBParticleEmitter {
	PhysicsBasedParticle particles[MAX_PB_PARTICLES];
	uint32_t numberOfParticles = MAX_PB_PARTICLES;
	uint32_t particleToEmitEachTime = 10;
	uint32_t lastIndex = 0;

	PBParticleSpawningValue spawningValues;
};

void EmitParticle(
	PBParticleEmitter& particleEmitter
	, Transform& emitterTransform
);

void UpdateParticle(
	PhysicsBasedParticle* const particles
	, const uint32_t size
	, const float deltaTime
);