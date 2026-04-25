#pragma once

#include "rendering/renderer.h"
#include "rendering/renderableObject.h"
#include "rendering/material.h"
#include "rendering/model.h"
#include "utils/camera.h"
#include "utils/performanceCalculator.h"
#include "rendering/shader.h"

#include "physics/physicsObject.h"

#include "sceneGraph/sceneGraph.h"

#define RESERVE 150

namespace Drop
{

	class Game
	{
	public:
		Game() {};
		virtual ~Game() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnFixedUpdate(float fixedDeltaTime) {}
		virtual void OnUIRender() {}
	public:
		Camera m_Camera = Camera(
			glm::vec3(19.0f, 23.0f, 54.5f),
			45.0f,
			0.1f,
			1000.0f,
			10.0f,
			0.75f);


		bool m_Wireframe = false;
		bool m_VSync = false;

		glm::vec3 m_LightDir = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::mat4 m_lightSpaceMatrix;
	};
}
