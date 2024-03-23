#pragma once

#include <glm/glm.hpp>
#include <GLAD/glad.h>

#include "../utils/TextureParameter.h"

class Material {
public:
	int shaderIndex;

	// Shader parameters
	// weight for the diffusive component
	GLfloat Kd = 3.0f;
	// roughness index for GGX shader
	GLfloat alpha = 0.2f;
	// Fresnel reflectance at 0 degree (Schlik's approximation)
	GLfloat F0 = 0.9f;


	GLint textureId;
	// UV repetitions
	GLfloat repeat = 1.0;

	Material(int shaderIndex_val = 0,
		GLfloat Kd_val = 3.0f,
		GLfloat alpha_val = 0.2f,
		GLfloat F0_val = 0.9f) :
		shaderIndex(shaderIndex_val),
		Kd(Kd_val),
		alpha(alpha_val),
		F0(F0_val)
	{};
};