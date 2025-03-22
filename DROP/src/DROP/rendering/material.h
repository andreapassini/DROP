#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <GLAD/glad.h>

typedef uint32_t TextureID;
typedef uint32_t ShaderID;

struct Material {
	ShaderID shaderID = 0;

	// Illumination shader parameters
	// weight for the diffusive component
	GLfloat kd = 3.0f;
	// roughness index for GGX shader
	GLfloat alpha = 0.2f;
	// Fresnel reflectance at 0 degree (Schlik's approximation)
	GLfloat f0 = 0.9f;

	// Shadow parameters
	bool bCastShadow = false;

	// Texture parameters
	bool bUseTexture = false;
	TextureID textureId = 0;
	float UVRepeat = 1.0f;
};