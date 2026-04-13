#include "RenderingSystem.h"

#include "DROP/rendering/model.h"
#include "DROP/rendering/renderer.h"
#include "DROP/sceneGraph/sceneGraph.h"
#include "DROP/particles/physicsBasedParticle.h"
#include "DROP/terrain/terrainComponent.h"

using namespace Drop;

void RenderingSystem::Update(ECS& ecs, const float deltaTime) {
	
	SceneContext& sceneContext = ecs.GetSingletonComponent<SceneContext>();
	RendererContext& rendererContext = ecs.GetSingletonComponent<RendererContext>();

	std::vector<StaticMeshComponent>& denseMeshComponents = ecs.GetComponentPool<StaticMeshComponent>().Data();
	std::vector<TerrainComponent>& denseTerrainComponents = ecs.GetComponentPool<TerrainComponent>().Data();

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

	// draw all the terrains
	for (size_t i = 0; i < denseTerrainComponents.size(); i++)
	{
		TerrainComponent& terrainComponent = denseTerrainComponents[i];

		TransformComponent& transformComp = ecs.GetSibiling<TerrainComponent, TransformComponent>(i);
		Transform& worldTransform = transformComp.cumulatedTransform;

		// Consider the entity as the center of all terrains
		// 3 should be num row and num col
		int32_t numRowOrCol = (int32_t)sqrt(terrainComponent.numOfTerrains); // since it's a square
		float rowCenter = 0.0;
		rowCenter = (float)numRowOrCol / 2.0f;
		float colCenter = 0.0f;
		colCenter = (float)numRowOrCol / 2.0f;
		float debugDisplacement = 0.1f;
		float displacement = 10.0f + debugDisplacement;

		for (uint32_t j = 0; j < terrainComponent.numOfTerrains; j++) {
			float row = (float)(j / numRowOrCol) - rowCenter;
			float col = (float)(j % numRowOrCol) - colCenter;
			VgMath::Vector3 extraDisplacement;
			extraDisplacement.x = (float)(row) * displacement;
			extraDisplacement.z = (float)(col) * displacement;
			Transform movedTransform = worldTransform;
			movedTransform.translate += extraDisplacement;

			Renderer::DrawTerrain(
				terrainComponent
				, j
				, movedTransform
				, sceneContext
				, rendererContext
			);
		}
	}

	std::vector<ParticleEmitter>& denseParticleEmitters = ecs.GetComponentPool<ParticleEmitter>().Data();
	Renderer::RenderParticles(
		sceneContext
		, denseParticleEmitters
		, &rendererContext.shaders[BILLBOARD_SHADER]
	);

	std::vector<PBParticleEmitter>& densePBParticleEmitters = ecs.GetComponentPool<PBParticleEmitter>().Data();
	Renderer::RenderParticles(
		sceneContext
		, densePBParticleEmitters
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
	
	Renderer::DrawParticleEmitterSurface(
		sceneContext
		, &(rendererContext.shaders[EMPTY_QUAD_SHADER]),
		densePBParticleEmitters
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