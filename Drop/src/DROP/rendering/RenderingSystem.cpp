#include "RenderingSystem.h"

#include "../rendering/model.h"
#include "../rendering/renderer.h"

using namespace Drop;

void RenderingSystem::Update(ECS& ecs, const float deltaTime) {
	
	SceneContext& sceneContext = ecs.GetSingletonComponent<SceneContext>();
	RendererContext& rendererContext = ecs.GetSingletonComponent<RendererContext>();

	std::vector<RenderableObject>& rendereableObjects = ecs.GetComponentPool<RenderableObject>().Data();
	std::vector<Model>& denseModels = ecs.GetComponentPool<Model>().Data();

	Renderer::RenderScene(
		sceneContext
		, rendererContext
		, rendereableObjects
		, sceneContext.m_CumulatedTransforms
		, &(rendererContext.m_ShadowShader)
		, &(rendererContext.m_LightShader)
	);

	// Do we need to make it more general and divide more the 2 systems?
	std::vector<ParticleEmitter>& denseParticleEmitters = ecs.GetComponentPool<ParticleEmitter>().Data();

	Renderer::RenderParticles(
		sceneContext
		,denseParticleEmitters
		, &rendererContext.m_BillboardShader
	);

	if (rendererContext.m_DrawDebug)
	{
		//Renderer::DrawDebug(
		//	sceneContext,
		//	&(rendererContext.m_DebugShader),
		//	sceneContext.drawableLines
		//);

		Renderer::DrawParticleEmitterSurface(
			sceneContext
			, &(rendererContext.m_EmptyQuadShader),
			denseParticleEmitters
		);
	}

}
