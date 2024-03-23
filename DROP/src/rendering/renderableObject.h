#pragma once

#include <glm/glm.hpp>

#include "../utils/model.h"
#include "material.h"

class RenderableObject
{
public:

	// ModelMatrix
	glm::mat4 ModelMatrix;	// in world coordinates
	// Mesh
	Model* mesh;
	// Material
	Material* material;

};

