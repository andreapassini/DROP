#include "Particle.h"
#include "../math/sceneGraph.h"
#include "../utils/Log.h"

using namespace VgMath;

void ParticleSystem::Spawn(Vector3* const pos)
{
	if (m_LastIndex == MAX_PARTICLES)
	{
		m_LastIndex = 0;
	}

	Particle& particle = m_Particles[m_LastIndex];
	particle.m_IsActive = true;

	float lifeTimeDiff = m_ParticleSetup.m_MaxLifeTime - m_ParticleSetup.m_MinLifeTime;
	particle.m_LifeTime = 0.0f;
	particle.m_MaxLifeTime = 
		m_ParticleSetup.m_MinLifeTime 
		+ (lifeTimeDiff * (std::rand() / RAND_MAX));

	Versor3 velDirection(
		(2 * (float)std::rand() / (float)RAND_MAX) - 1
		, (2 * (float)std::rand() / (float)RAND_MAX) - 1
		, (2 * (float)std::rand() / (float)RAND_MAX) - 1
	);

	particle.m_Position = pos;
	float speedDiff = m_ParticleSetup.m_MaxSpeed - m_ParticleSetup.m_MinSpeed;
	float speed = m_ParticleSetup.m_MinSpeed + (speedDiff * (std::rand() / RAND_MAX));
	particle.m_Velocity = velDirection * speed;

	LOG_CORE_INFO("Spawned particle n: {0}", m_LastIndex);

	m_LastIndex++;
}

void ParticleSystem::Update(float deltaTime)
{
	for (uint32_t i = 0; i < m_LastIndex; i++)
	{
		Particle& particle = m_Particles[i];
		if (!particle.m_IsActive) continue;

		particle.m_LifeTime += deltaTime;
		if (particle.m_LifeTime >= particle.m_MaxLifeTime * 1000.f)
		{
			// Particle is dying
			particle.m_IsActive = false;
			particle.m_LifeTime = 0.0f;
			m_LastIndex--;

			continue;
		}

		*particle.m_Position += particle.m_Velocity * deltaTime;
	}
}