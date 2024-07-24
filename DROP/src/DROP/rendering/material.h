#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <GLAD/glad.h>

#include "TextureParameter.h"

struct Material {
	// Shader parameters
	// weight for the diffusive component
	GLfloat Kd = 3.0f;
	// roughness index for GGX shader
	GLfloat Alpha = 0.2f;
	// Fresnel reflectance at 0 degree (Schlik's approximation)
	GLfloat F0 = 0.9f;

	bool CastShadow = false;

	bool UseTexture = false;
	uint32_t TextureId = 0;
	// UV repetitions
	float Repeat = 1.0f;
};