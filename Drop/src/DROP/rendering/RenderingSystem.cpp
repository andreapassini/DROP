#include "RenderingSystem.h"

#include "DROP/rendering/model.h"
#include "DROP/rendering/renderer.h"

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

//// Move this function into the RenderingSystem::Update function
//Renderer::RenderScene(
//	sceneContext
//	, m_renderingContext
//	, m_RendereableObjects
//	, m_CumulatedTransforms
//	, &(m_Game->m_ShadowShader)
//	, &(m_Game->m_LightShader)
//);

//// Draw Normal as vectors
//m_Renderer.RenderScene(
//	sceneContext,
//	m_RendereableObjects,
//	m_CumulatedTransforms,
//	&(displayNormalShader)
//);

//Renderer::RenderParticles(
//	sceneContext
//	, particleEmitter.particles
//	, particleEmitter.numberOfParticles
//	, &billboardShader
//);

//if (m_DrawDebug)
//{
//	Renderer::DrawDebug(
//		sceneContext, 
//		&(m_Game->m_DebugShader),
//		m_DrawableLines
//	);

//	std::vector<ParticleEmitter> ParticleEmitters;
//	ParticleEmitters.push_back(particleEmitter);
//	Renderer::DrawParticleEmitterSurface(
//		sceneContext
//		, &(m_Game->m_EmptyQuadShader),
//		ParticleEmitters
//	);
//}