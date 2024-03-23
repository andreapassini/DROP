#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "renderableObject.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void RenderScene(const std::vector<RenderableObject> &renderableObjects, glm::mat4 view);

private:

};
