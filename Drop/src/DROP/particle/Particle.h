#pragma once

#include "../math/mat3.h"
#include "../rendering/colors.h"
#include "../rendering/renderableObject.h"
#include "../rendering/model.h"

constexpr uint32_t MAX_PARTICLES = 1'000;

using namespace VgMath;

struct ParticleConfiguration
{
	float m_MinLifeTime = 0.0f;
	float m_MaxLifeTime = 0.0f;
	float m_MinSpeed = 0.0f;
	float m_MaxSpeed = 0.0f;
};

struct Particle
{
	Vector3* m_Position;
	Vector3 m_Velocity{};
	float m_MaxLifeTime = 0.0f;
	float m_LifeTime = 0.0f;
	bool m_IsActive = false;
};

class ParticleSystem
{
public:
	ParticleConfiguration m_ParticleSetup;
	Particle m_Particles[MAX_PARTICLES];
	std::vector<RenderableObject>& m_RenderableObjects;
	uint32_t m_LastIndex = 0;

	ParticleSystem(std::vector<RenderableObject>& renderableObjects)
	{
		
	};

	void Spawn(
		Vector3* const pos
	);

	void Update(
		float deltaTime		
	);
};