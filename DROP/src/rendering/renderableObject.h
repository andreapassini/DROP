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
	glm::mat4* const m_ModelMatrix;	// in world coordinates
	// Mesh
	Model* m_Model;
	Material* m_Material;
	// Material
	TextureParameter m_TextureParameter;
	
	enum render_passes { SHADOWMAP, RENDER };

	RenderableObject(
		glm::mat4* const modelMatrix_val = nullptr,
		Model* model_val = nullptr,
		Material* material_val = nullptr,
		TextureParameter textureParameter_val = TextureParameter()
	) :
		m_ModelMatrix(modelMatrix_val),
		m_Model(model_val),
		m_Material(material_val),
		m_TextureParameter(textureParameter_val)
	{};

	void Draw(const Shader& shader, const glm::mat4& view,
		const GLint render_pass, const GLuint depthMap);
};

