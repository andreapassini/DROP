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
	TerrainsContext& terrainsContext = ecs.GetSingletonComponent<TerrainsContext>();

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

	// Draw terrain target


	// draw all the terrains
	for (size_t i = 0; i < denseTerrainComponents.size(); i++)
	{
		TerrainComponent& terrainComponent = denseTerrainComponents[i];

		TransformComponent& transformComp = ecs.GetSibiling<TerrainComponent, TransformComponent>(i);
		Transform& localTransform = transformComp.localTransform;

		// Consider the entity as the center of all terrains
		// 3 should be num row and num col
		int32_t numRowOrCol = (int32_t)sqrt(terrainComponent.numOfTerrains); // since it's a square

		float debugDisplacement = 0.0f;
		float displacement = TERRAIN_EDGE_SIZE;

		for (uint32_t j = 0; j < terrainComponent.numOfTerrains; j++) {
			float row = (float)(j / numRowOrCol);
			float col = (float)(j % numRowOrCol);

			VgMath::Vector3 extraDisplacement;
			extraDisplacement.x = ((float)(col) * displacement) + ((float)(col) * debugDisplacement);
			// Z minus since pos is towards camera
			extraDisplacement.z = - ((float)(row) * displacement) - ((float)(row) * debugDisplacement);
			Transform movedTransform = localTransform;
			movedTransform.translate += extraDisplacement;

			Renderer::DrawTerrain(
				terrainComponent
				, j
				, movedTransform
				, sceneContext
				, rendererContext
				, terrainsContext
			);

			if (rendererContext.drawDebug)
			{
				movedTransform.translate.y += 2.5f;
				bool bInsideTargetRange = terrainsContext.terrainToDisplacementMappings[j] != TERRAIN_INDEX_NULL ? true : false;
				Renderer::DrawTerrainCell(
					terrainComponent
					, j
					, row
					, col
					, (float)(numRowOrCol)
					, bInsideTargetRange
					, - 5.25f
					, movedTransform
					, sceneContext
					, rendererContext
					, terrainsContext
					, &rendererContext.shaders[EMPTY_QUAD_SHADER]
				);
			}
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