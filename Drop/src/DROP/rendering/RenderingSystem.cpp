#include "RenderingSystem.h"

#include "DROP/rendering/model.h"
#include "DROP/rendering/renderer.h"
#include "DROP/sceneGraph/sceneGraph.h"

using namespace Drop;

void RenderingSystem::Update(ECS& ecs, const float deltaTime) {
	
	SceneContext& sceneContext = ecs.GetSingletonComponent<SceneContext>();
	RendererContext& rendererContext = ecs.GetSingletonComponent<RendererContext>();

	std::vector<StaticMeshComponent>& denseMeshComponents = ecs.GetComponentPool<StaticMeshComponent>().Data();

	// enable depth -- done in renderer init
	
	// Shadow pass
	Renderer::SetupShadowPass(
		sceneContext
		, rendererContext
	);
	// draw all the meshes
	for (size_t i = 0; i < denseMeshComponents.size(); i++) {
		StaticMeshComponent& meshComponent = denseMeshComponents[i];
		if (!meshComponent.bCastShadow) continue;

		TransformComponent& transformComp = ecs.GetSibiling<StaticMeshComponent, TransformComponent>(i);
		Transform& worldTransform = transformComp.cumulatedTransform;

		Renderer::DrawMeshForShadow(
			meshComponent
			, worldTransform
			, sceneContext
			, rendererContext
		);
	}
	
	
	// set the color buffer
	Renderer::SetupColorPass(
		sceneContext
		, rendererContext
	);

	// draw all the mashes
	for (size_t i = 0; i < denseMeshComponents.size(); i++)
	{
		StaticMeshComponent& meshComponent = denseMeshComponents[i];

		TransformComponent& transformComp = ecs.GetSibiling<StaticMeshComponent, TransformComponent>(i);
		Transform& worldTransform = transformComp.cumulatedTransform;

		Renderer::DrawMesh(
			meshComponent
			, worldTransform
			, sceneContext
			, rendererContext
		);
	}

	std::vector<ParticleEmitter>& denseParticleEmitters = ecs.GetComponentPool<ParticleEmitter>().Data();
	Renderer::RenderParticles(
		sceneContext
		, denseParticleEmitters
		, &rendererContext.shaders[BILLBOARD_SHADER]
	);

	//if (rendererContext.m_DrawDebug)
	//{
	//	//Renderer::DrawDebug(
	//	//	sceneContext,
	//	//	&(rendererContext.m_DebugShader),
	//	//	sceneContext.drawableLines
	//	//);

	Renderer::DrawParticleEmitterSurface(
		sceneContext
		, &(rendererContext.shaders[EMPTY_QUAD_SHADER]),
		denseParticleEmitters
	);
	
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