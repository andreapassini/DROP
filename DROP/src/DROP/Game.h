#pragma once

#include "rendering/renderer.h"
#include "rendering/renderableObject.h"
#include "rendering/material.h"
#include "rendering/TextureParameter.h"
#include "rendering/model.h"
#include "utils/camera.h"
#include "utils/performanceCalculator.h"
#include "rendering/shader.h"

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
			"..\\DROP\\src\\DROP\\shaders\\19_shadowmap.vert", 
			"..\\DROP\\src\\DROP\\shaders\\20_shadowmap.frag");

		Shader m_LightShader = Shader(
			"..\\DROP\\src\\DROP\\shaders\\21_ggx_tex_shadow.vert", 
			"..\\DROP\\src\\DROP\\shaders\\ggx_tex_shadow_noSub.frag");

		Shader m_DebugShader = Shader(
			"..\\DROP\\src\\DROP\\shaders\\00_basic.vert",
			"..\\DROP\\src\\DROP\\shaders\\01_fullcolor.frag");

		Shader m_EmptyQuadShader = Shader(
			"..\\DROP\\src\\DROP\\shaders\\quadFrustum.vert",
			"..\\DROP\\src\\DROP\\shaders\\quadFrustum.geom",
			"..\\DROP\\src\\DROP\\shaders\\quadFrustum.frag");

		Shader m_EmptyBoxShader = Shader(
			"..\\DROP\\src\\DROP\\shaders\\boxFrustum.vert",
			"..\\DROP\\src\\DROP\\shaders\\boxFrustum.geom",
			"..\\DROP\\src\\DROP\\shaders\\boxFrustum.frag");



		bool m_Wireframe = false;
		bool m_VSync = false;

		glm::vec3 m_LightDir = glm::vec3(1.0f, 1.0f, 1.0f);
	};
}
