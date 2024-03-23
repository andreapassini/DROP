#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "renderableObject.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void RenderScene(const std::vector<RenderableObject> &renderableObjects, glm::mat4 view, int shaderId);

private:
	const GLuint SHADOW_WIDTH;
	const GLuint SHADOW_HEIGHT;
};
