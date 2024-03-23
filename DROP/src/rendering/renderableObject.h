#pragma once

#include <glm/glm.hpp>

#include "../utils/model.h"
#include "material.h"
#include "TextureParameter.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/shader.h"

class RenderableObject
{
public:

	// ModelMatrix
	glm::mat4 modelMatrix;	// in world coordinates
	// Mesh
	Model* model;
	// Material
	TextureParameter textureParameter;
	
	RenderableObject(
		glm::mat4 modelMatrix_val = glm::mat4(1.0f),
		Model* model_val = nullptr,
		TextureParameter textureParameter_val = TextureParameter()) :
		modelMatrix(modelMatrix_val),
		model(model_val),
		textureParameter(textureParameter_val)
	{};

	void Draw(const Shader& shader, const glm::mat4& view,
		const GLint render_pass, const GLuint depthMap);
};

