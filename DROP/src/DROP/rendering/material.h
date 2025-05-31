#pragma once

#include <vector>

#include "../dependencies/Glad/include/GLAD/glad.h"
#include <glm/glm.hpp>

// Arbitrary chose, from glad we could use up to 32 textures
// but some of them I will like to make them system texture index 20 - 31
// used for rendering pass only
#define MAX_USER_TEXTURES 20
#define TEXTURE_NOT_USED UINT32_MAX

// System textures
#define SHADOW_MAP_ACTIVE GL_TEXTURE20 + (GL_TEXTURE0 - GL_TEXTURE2)
#define SHADOW_MAP_1i MAX_USER_TEXTURES + GL_TEXTURE0 - GL_TEXTURE2

typedef uint32_t TextureID;
typedef uint32_t ShaderID;

struct TextureSpecification {
	TextureID textureId = TEXTURE_NOT_USED;
	float UVRepeat = 1.0f;

	// We could use a string here for the name
	// maybe arr char with fixed size
	// or a better string with hashing

	// here we could specify the filter to use
};

struct Material {
	ShaderID shaderID = 0;

	// Illumination shader parameters
	float kd = 3.0f;		// weight for the diffusive component
	float alpha = 0.2f;	// roughness index for GGX shader
	float f0 = 0.9f;		// Fresnel reflectance at 0 degree (Schlik's approximation)

	// -- 
	float alphaCutOut = 0.1f;

	// Texture parameters
	TextureSpecification textures[MAX_USER_TEXTURES];
};