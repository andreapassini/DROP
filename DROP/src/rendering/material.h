#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <GLAD/glad.h>

#include "TextureParameter.h"

struct Material {
	//Material(
	//	GLfloat Kd_val = 3.0f,
	//	GLfloat alpha_val = 0.2f,
	//	GLfloat F0_val = 0.9f,
	//	bool castShadow_val = false)
	//	:
	//	m_Kd(Kd_val),
	//	m_Alpha(alpha_val),
	//	m_F0(F0_val),
	//	m_CastShadow(castShadow_val)
	//{};
	// 
	// Shader parameters
	// weight for the diffusive component
	GLfloat Kd = 3.0f;
	// roughness index for GGX shader
	GLfloat Alpha = 0.2f;
	// Fresnel reflectance at 0 degree (Schlik's approximation)
	GLfloat F0 = 0.9f;

	bool CastShadow = false;
};