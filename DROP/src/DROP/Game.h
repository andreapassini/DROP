#pragma once

#include "rendering/renderer.h"
#include "rendering/renderableObject.h"
#include "rendering/material.h"
#include "rendering/TextureParameter.h"
#include "utils/model.h"
#include "utils/camera.h"
#include "utils/performanceCalculator.h"
#include "utils/shader.h"

#include "physics/physicsObject.h"

#include "math/sceneGraph.h"

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
			glm::vec3(0.0f, 0.0f, 6.0f),
			45.0f,
			0.1f,
			100.0f,
			10.0f,
			0.75f);

		Shader m_ShadowShader = Shader(
			"..\\Drop\\src\\Drop\\shaders\\19_shadowmap.vert", 
			"..\\Drop\\src\\Drop\\shaders\\20_shadowmap.frag");
		Shader m_LightShader = Shader(
			"..\\Drop\\src\\Drop\\shaders\\21_ggx_tex_shadow.vert", 
			"..\\Drop\\src\\Drop\\shaders\\22_ggx_tex_shadow.frag");

		uint32_t m_CurrentSubroutine = 0;

		bool m_Wireframe = false;
		glm::vec3 m_LightDir = glm::vec3(1.0f, 1.0f, 1.0f);
	};
}
