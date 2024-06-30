#pragma once

#include "rendering/renderer.h"
#include "rendering/renderableObject.h"
#include "utils/camera.h"
#include "utils/performanceCalculator.h"
#include "utils/shader.h"

#include "math/sceneGraph.h"

#define RESERVE 150

namespace DROP
{

	class Game
	{
	public:
		virtual ~Game() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float ts) {}
		virtual void OnUIRender() {}
	public:
		SceneGraph m_SceneGraph = (RESERVE);
		std::vector<std::string> m_Shaders;
		std::vector<int> m_TextureId;
		std::vector<RenderableObject> m_RendereableObjects;

	private:
		std::unordered_map<uint32_t, VgMath::Transform> m_CumulatedTransforms;
		std::unordered_map<uint32_t, glm::mat4> m_ModelMatrices;
	};

}