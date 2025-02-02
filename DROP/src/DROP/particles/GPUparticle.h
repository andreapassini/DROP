#pragma once

#include "../math/mat3.h"
#include "../math/surface.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "../rendering/shader.h"

using namespace VgMath;

#define MAX_GPU_PARTICLES 1'000'000
#define SQR3_GPU_PARTICLES 100

struct Particles
{
	GLuint vao = 0;
	GLuint posBuf = 0;
	GLuint velBuf = 0;

	GLuint bhVao = 0;
	GLuint bhBuf = 0;

	glm::vec4 bh1, bh2;

	float speed, angle;

	Shader particleComputeShader;

	GLuint blackHole1Loc = 0;
	GLuint blackHole2Loc = 0;
};

// init as a cube
void StartingPosition(glm::vec4* const positions)
{
	// Center of the cube
	glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

	float d = 2.0f / (float)SQR3_GPU_PARTICLES;

	uint32_t particleIndex = 0;
	for (uint32_t x = 0; x < SQR3_GPU_PARTICLES; x++)
	{
		for (uint32_t y = 0; y < SQR3_GPU_PARTICLES; y++)
		{
			for (uint32_t z = 0; z < SQR3_GPU_PARTICLES; z++)
			{
				p.x = d * x;
				p.y = d * y;
				p.z = d * z;
				p.w = 1.0f;
				positions[particleIndex] = p;
				particleIndex++;
			}
		}
	}
}

void InitBuffers(Particles& particles)
{
	glm::vec4 positions[MAX_GPU_PARTICLES];

	StartingPosition(positions);

	glCreateBuffers(1, &particles.posBuf);
	glCreateBuffers(1, &particles.m_velBuf);

	GLuint BuffSize = (uint32_t)MAX_GPU_PARTICLES * sizeof(positions[0]);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particles.posBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BuffSize, &positions[0], GL_DYNAMIC_DRAW);

	glm::vec4 velocities[MAX_GPU_PARTICLES];
	for (uint32_t i = 0; i < MAX_GPU_PARTICLES; i++){
		velocities[i] = glm::vec4(0);
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particles.m_velBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BuffSize, &velocities[0], GL_DYNAMIC_COPY);

	glGenVertexArrays(1, &particles.particles.vao);
	glBindVertexArray(particles.particles.vao);

	glBindBuffer(GL_ARRAY_BUFFER, particles.posBuf);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glCreateBuffers(1, &particles.particles.bhBuf);
	glBindBuffer(GL_ARRAY_BUFFER, particles.particles.bhBuf);
	GLfloat data[] = { 
		particles.bh1.x
		, particles.bh1.y
		, particles.bh1.z
		, particles.bh1.w
		, particles.particles.bh2.x
		, particles.particles.bh2.y
		, particles.particles.bh2.z
		, particles.particles.bh2.w 
	};
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &particles.particles.bhVao);
	glBindVertexArray(particles.particles.bhVao);

	glBindBuffer(GL_ARRAY_BUFFER, particles.particles.bhBuf);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

}

void InitComputeShader(Particles& particles) {
	particles.blackHole1Loc = GetUniformLocation("BlackHolePos1");
	particles.blackHole2Loc = GetUniformLocation("BlackHolePos2");
}

void ExecuteParticleComputeShader(
	Particles& particles
	, const glm::vec3& Pos1
	, const glm::vec3& Pos2
) {

	glUniform3f(particles.blackHole1Loc, Pos1.x, Pos1.y, Pos1.z);
	glUniform3f(particles.blackHole2Loc, Pos2.x, Pos2.y, Pos2.z);

}

void Render(Particles& particles, Transform& transform) {
	// Rotate the attractors ("black holes")
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(particles.angle), glm::vec3(0, 0, 1));
	glm::vec3 BlackHolePos1(glm::vec3(rot * particles.bh1));
	glm::vec3 BlackHolePos2(glm::vec3(rot * particles.bh2));

	ExecuteParticleComputeShader(particles, BlackHolePos1, BlackHolePos2);

	RenderParticles(particles, BlackHolePos1, BlackHolePos2, transform);
}

void RenderParticles(
	Particles& particles
	, const glm::vec3& BlackHolePos1
	, const glm::vec3& BlackHolePos2
	, Transform& transform
) {
	// Draw the scene
	particles.particleComputeShader.Use();
	glUniformMatrix4fv(m_WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw the particles
	glPointSize(2.0f);

	m_colorTech.SetColor(Vector4f(0.0f, 0.0f, 0.0f, 0.2f));

	glBindVertexArray(particles.vao);
	glDrawArrays(GL_POINTS, 0, MAX_GPU_PARTICLES);
	glBindVertexArray(0);

	// Draw the black holes
	glPointSize(15.0f);
	GLfloat data[] = { BlackHolePos1.x, BlackHolePos1.y, BlackHolePos1.z, 1.0f, BlackHolePos2.x, BlackHolePos2.y, BlackHolePos2.z, 1.0f };
	glBindBuffer(GL_ARRAY_BUFFER, particles.bhBuf);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), data);

	m_colorTech.SetColor(Vector4f(1, 0, 0, 1.0f));

	glBindVertexArray(particles.bhVao);
	glDrawArrays(GL_POINTS, 0, 2);
	glBindVertexArray(0);
}