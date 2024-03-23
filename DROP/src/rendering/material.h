#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <GLAD/glad.h>

#include "TextureParameter.h"

class Material {
public:
	int shaderIndex;

	// Shader parameters
	// weight for the diffusive component
	GLfloat Kd;
	// roughness index for GGX shader
	GLfloat alpha;
	// Fresnel reflectance at 0 degree (Schlik's approximation)
	GLfloat F0;

	bool useTexture;
	GLint textureId;
	std::vector<GLint>* textures;
	// UV repetitions
	GLfloat repeat;

	Material(
		GLfloat Kd_val = 3.0f,
		GLfloat alpha_val = 0.2f,
		GLfloat F0_val = 0.9f) 
		:
		Kd(Kd_val),
		alpha(alpha_val),
		F0(F0_val)
	{};
};