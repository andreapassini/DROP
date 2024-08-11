#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../rendering/model.h"
#include "material.h"
#include "TextureParameter.h"

#include "../rendering/shader.h"
#include "Drop/math/transform.h"

#include <unordered_map>

class RenderableObject
{
public:

	// ModelMatrix
	uint32_t m_ModelMatrixId = 0;	// in world coordinates
	// Mesh
	uint32_t m_ModelId = 0;
	uint32_t m_MaterialId = 0;
	
	enum render_passes { SHADOWMAP, RENDER };

	RenderableObject(
		uint32_t modelMatrixId,
		uint32_t modelId,
		uint32_t materialId
	) :
		m_ModelMatrixId(modelMatrixId),
		m_ModelId(modelId),
		m_MaterialId(materialId)
	{};

	void Draw(
		const Shader& shader,
		const std::vector<int>& textuers,
		const std::vector<Model>& models,
		const std::vector<Material>& materials,
		std::unordered_map<uint32_t, VgMath::Transform> m_CumulatedTransforms,

		const glm::mat4& view,
		const GLint render_pass, 
		const GLuint depthMap) const ;
};

