#include "Drop/GameEngine.h"
#include "Drop/EntryPoint.h"

#include "Drop/input/Input.h"
#include "Drop/sceneGraph/sceneGraph.h"
#include "Drop/sceneGraph/scene.h"
#include "Drop/utils/ExecPath.h"
#include "Drop/utils/SceneSerializer.h"
#include "DROP/tags/tag.h"
#include "DROP/utils/Log.h"
#include "Drop/particles/particle.h"
#include "Drop/particles/physicsBasedParticle.h"
#include "DROP/terrain/terrainComponent.h"
#include "DROP/terrain/terrainSystem.h"

#define UV_GRID_SIM_DIFFUSE_MAP 0
#define CRACKED_SOIL_DIFFUSE_MAP 1
#define WIDOWS_LOGO_DIFFUSE_MAP 2
#define GRASS_DIFFUSE_MAP 3

using namespace Drop;

class ExampleGame : public Drop::Game
{
public:
	ExampleGame()
	{
        GameEngine* gameEngine = GameEngine::GetInstance();

#pragma region ShaderSetup
        //FULL_COLOR_SHADER
        std::string path_00_basic_vert =     GetFullPath("/DROP/src/Drop/shaders/00_basic.vert");
        std::string path_01_fullcolor_frag = GetFullPath("/DROP/src/Drop/shaders/01_fullcolor.frag");
        gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            path_00_basic_vert.c_str(), path_00_basic_vert.size() + 1
            , path_01_fullcolor_frag.c_str(), path_01_fullcolor_frag.size() + 1
        );

        // ILLUMINATION_GGX_SHADER
        std::string path_21_ggx_tex_shadow =    GetFullPath("/DROP/src/Drop/shaders/21_ggx_tex_shadow.vert");
        std::string path_ggx_tex_shadow_noSub = GetFullPath("/DROP/src/Drop/shaders/ggx_tex_shadow_noSub.frag");
        gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            path_21_ggx_tex_shadow.c_str(), path_21_ggx_tex_shadow.size() + 1
            , path_ggx_tex_shadow_noSub.c_str(), path_ggx_tex_shadow_noSub.size() + 1
        );

        // SHADOW_SHADER
        std::string path_19_shadowmap = GetFullPath("/DROP/src/Drop/shaders/19_shadowmap.vert");
        std::string path_20_shadowmap = GetFullPath("/DROP/src/Drop/shaders/20_shadowmap.frag");
        gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            path_19_shadowmap.c_str(), path_19_shadowmap.size() + 1
            , path_20_shadowmap.c_str(), path_20_shadowmap.size() + 1
        );

        // EMPTY_QUAD_SHADER
        std::string path_quadFrustum_vert = GetFullPath("/DROP/src/Drop/shaders/quadFrustum.vert");
        std::string path_quadFrustum_geom = GetFullPath("/DROP/src/Drop/shaders/quadFrustum.geom");
        std::string path_quadFrustum_frag = GetFullPath("/DROP/src/Drop/shaders/quadFrustum.frag");
        gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            path_quadFrustum_vert.c_str(), path_quadFrustum_vert.size() + 1
            , path_quadFrustum_geom.c_str(), path_quadFrustum_geom.size() + 1
            , path_quadFrustum_frag.c_str(), path_quadFrustum_frag.size() + 1
        );

        // EMPTY_BOX_SHADER
        std::string path_boxFrustum_vert = GetFullPath("/DROP/src/Drop/shaders/boxFrustum.vert");
        std::string path_boxFrustum_geom = GetFullPath("/DROP/src/Drop/shaders/boxFrustum.geom");
        std::string path_boxFrustum_frag = GetFullPath("/DROP/src/Drop/shaders/boxFrustum.frag");
        gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            path_boxFrustum_vert.c_str(), path_boxFrustum_vert.size() + 1
            , path_boxFrustum_geom.c_str(), path_boxFrustum_geom.size() + 1
            , path_boxFrustum_frag.c_str(), path_boxFrustum_frag.size() + 1
        );

        // BILLBOARD_SHADER
        std::string path_billboard_vert = GetFullPath("/DROP/src/Drop/shaders/billboard.vert");
        std::string path_billboard_geom = GetFullPath("/DROP/src/Drop/shaders/billboard.geom");
        std::string path_billboard_frag = GetFullPath("/DROP/src/Drop/shaders/billboard.frag");
        gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            path_billboard_vert.c_str(), path_billboard_vert.size() + 1
            , path_billboard_geom.c_str(), path_billboard_geom.size() + 1
            , path_billboard_frag.c_str(), path_billboard_frag.size() + 1
        );

        // TERRAIN_SHADER
        std::string path_terrain_vert = GetFullPath("/DROP/src/Drop/shaders/terrain.vert");
        std::string path_terrain_frag = GetFullPath("/DROP/src/Drop/shaders/terrain.frag");
        gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            path_terrain_vert.c_str(), path_terrain_vert.size() + 1
            , path_terrain_frag.c_str(), path_terrain_frag.size() + 1
        );
#pragma endregion

        LOG_TRACE("GetExecutableDir: " + GetExecutableDir());
        LOG_TRACE("GetExecutablePath: " + GetExecutablePath());
        LOG_TRACE("GetRelativeProjectPath: " + GetRelativeProjectPath());
                
#pragma region Assets
        gameEngine->m_Models.emplace_back(GetFullPath("\\models\\cube.obj"));
        gameEngine->m_Models.emplace_back(GetFullPath("\\models\\sphere.obj"));
        gameEngine->m_Models.emplace_back(GetFullPath("\\models\\bunny_lp.obj"));
        gameEngine->m_Models.emplace_back(GetFullPath("\\models\\plane.obj"));

        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture(
            GetFullPath("\\textures\\UV_Grid_Sm.png").c_str()));
        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture(
            GetFullPath("\\textures\\SoilCracked.png").c_str()));

        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture(
            GetFullPath("\\textures\\windowsLogo\\diffuse.png").c_str()));
        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture(
            GetFullPath("\\textures\\grass\\diffuse.png").c_str()));
#pragma endregion

#pragma region Materials
        {
            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = CRACKED_SOIL_DIFFUSE_MAP;
            material.textures[0].UVRepeat = 80.0f;
            material.shaderID = TERRAIN_SHADER;
            gameEngine->m_Materials.push_back(material);
        }
        {
            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = UV_GRID_SIM_DIFFUSE_MAP;
            material.textures[0].UVRepeat = 1.0f;
            material.shaderID = ILLUMINATION_GGX_SHADER;
            gameEngine->m_Materials.push_back(material);

        }
        {
            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = UV_GRID_SIM_DIFFUSE_MAP;
            material.textures[0].UVRepeat = 1.0f;
            material.shaderID = ILLUMINATION_GGX_SHADER;
            gameEngine->m_Materials.push_back(material);
        }
        {
            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = UV_GRID_SIM_DIFFUSE_MAP;
            material.textures[0].UVRepeat = 1.0f;
            material.shaderID = ILLUMINATION_GGX_SHADER;
            gameEngine->m_Materials.push_back(material);
        }
#pragma endregion

        gameEngine->g_activeScene->ecs.RegisterComponent<Tag>();
        gameEngine->g_activeScene->ecs.RegisterComponent<TransformComponent>();

        // #TODO 
        // We will have multiple cameras, but only one active camera 
        //	as a Singleton Component
        //gameEngine->g_activeScene->ecs.RegisterComponent<Camera, Transform>();

        gameEngine->g_activeScene->ecs.RegisterComponent<StaticMeshComponent, TransformComponent>();
        gameEngine->g_activeScene->ecs.RegisterComponent<TerrainComponent, TransformComponent>();

        gameEngine->g_activeScene->ecs.RegisterComponent<ParticleEmitter, TransformComponent>();
        gameEngine->g_activeScene->ecs.RegisterComponent<PBParticleEmitter, TransformComponent>();
        //gameEngine->g_activeScene->ecs.RegisterComponent<Billboard, Transform>();
        //gameEngine->g_activeScene->ecs.RegisterComponent<PhysicsObject>();

		//// Plane
  //      {
  //          //uint32_t planeSGHandle = gameEngine->m_SceneGraph.AddObject(SceneGraph::ROOT_ID);
  //          bseecs::EntityID id = gameEngine->g_activeScene->ecs.CreateEntity();

  //          Tag& tag = gameEngine->g_activeScene->ecs.Add<Tag>(id);
  //          tag.tagName = "Plane";

  //          TransformComponent& transform = gameEngine->g_activeScene->ecs.Add<TransformComponent>(id);
  //          transform.m_LocalTransform.m_Translate = VgMath::Vector3(0.0f, -1.0f, 0.0f);
  //          transform.m_LocalTransform.m_Scale = 10.0f;
  //          transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
  //              VgMath::Degrees(90.0),
  //              VgMath::Vector3(0.0, 1.0, 0.0).normalized()
  //          );

  //          Material material;
  //          material.kd = 3.0f;
  //          material.alpha = 0.2f;
  //          material.f0 = 0.9f;
  //          material.textures[0].textureId = CRACKED_SOIL_DIFFUSE_MAP;
  //          material.textures[0].UVRepeat = 80.0f;
  //          material.shaderID = ILLUMINATION_GGX_SHADER;
  //          gameEngine->m_Materials.push_back(material);

  //          StaticMeshComponent& staticMesh = gameEngine->g_activeScene->ecs.Add<StaticMeshComponent, TransformComponent>(id);
  //          staticMesh.bCastShadow = true;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 3;
  //      }
  //    

  //      // Sphere
  //      bseecs::EntityID sphereId = gameEngine->g_activeScene->ecs.CreateEntity();
  //      {
  //          Tag& tag = gameEngine->g_activeScene->ecs.Add<Tag>(sphereId);
  //          tag.tagName = "Sphere";

  //          TransformComponent& transform = gameEngine->g_activeScene->ecs.Add<TransformComponent>(sphereId);
  //          transform.m_LocalTransform.m_Translate = VgMath::Vector3(-3.0f, 1.0f, 0.0f);
  //          transform.m_LocalTransform.m_Scale = 1.0f;
  //          transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
  //              VgMath::Degrees(90.0),
  //              VgMath::Vector3(0.0, 1.0, 0.0).normalized()
  //          );

  //          Material material;
  //          material.kd = 3.0f;
  //          material.alpha = 0.2f;
  //          material.f0 = 0.9f;
  //          material.textures[0].textureId = UV_GRID_SIM_DIFFUSE_MAP;
  //          material.textures[0].UVRepeat = 1.0f;
  //          material.shaderID = ILLUMINATION_GGX_SHADER;
  //          gameEngine->m_Materials.push_back(material);

  //          StaticMeshComponent& staticMesh = gameEngine->g_activeScene->ecs.Add<StaticMeshComponent, TransformComponent>(sphereId);
  //          staticMesh.bCastShadow = true;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 1;
  //      }


  //      // Cube
  //      {
  //          bseecs::EntityID id = gameEngine->g_activeScene->ecs.CreateEntity();

  //          Tag& tag = gameEngine->g_activeScene->ecs.Add<Tag>(id);
  //          tag.tagName = "Cube";

  //          TransformComponent& transform = gameEngine->g_activeScene->ecs.Add<TransformComponent>(id);
  //          transform.m_Parent = sphereId;
  //          transform.m_LocalTransform.m_Translate = VgMath::Vector3(7.0f, 1.0f, 0.0f);
  //          transform.m_LocalTransform.m_Scale = 0.48f;
  //          transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
  //              VgMath::Degrees(90.0),
  //              VgMath::Vector3(0.0, 1.0, 0.0).normalized()
  //          );

  //          Material material;
  //          material.kd = 3.0f;
  //          material.alpha = 0.2f;
  //          material.f0 = 0.9f;
  //          material.textures[0].textureId = UV_GRID_SIM_DIFFUSE_MAP;
  //          material.textures[0].UVRepeat = 1.0f;
  //          material.shaderID = ILLUMINATION_GGX_SHADER;
  //          gameEngine->m_Materials.push_back(material);

  //          StaticMeshComponent& staticMesh = gameEngine->g_activeScene->ecs.Add<StaticMeshComponent, TransformComponent>(id);
  //          staticMesh.bCastShadow = true;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 0;
  //      }


  //      // Bunny
  //      {
  //          bseecs::EntityID id = gameEngine->g_activeScene->ecs.CreateEntity();

  //          Tag& tag = gameEngine->g_activeScene->ecs.Add<Tag>(id);
  //          tag.tagName = "Bunny";

  //          TransformComponent& transform = gameEngine->g_activeScene->ecs.Add<TransformComponent>(id);
  //          transform.m_Parent = sphereId;
  //          transform.m_LocalTransform.m_Translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
  //          transform.m_LocalTransform.m_Scale = 0.48f;
  //          transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
  //              VgMath::Degrees(90.0),
  //              VgMath::Vector3(0.0, 1.0, 0.0).normalized()
  //          );

  //          Material material;
  //          material.kd = 3.0f;
  //          material.alpha = 0.2f;
  //          material.f0 = 0.9f;
  //          material.textures[0].textureId = UV_GRID_SIM_DIFFUSE_MAP;
  //          material.textures[0].UVRepeat = 1.0f;
  //          material.shaderID = ILLUMINATION_GGX_SHADER;
  //          gameEngine->m_Materials.push_back(material);

  //          StaticMeshComponent& staticMesh = gameEngine->g_activeScene->ecs.Add<StaticMeshComponent, TransformComponent>(id);
  //          staticMesh.bCastShadow = false;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 2;
  //      }

  //      
  //      // Particle Emitter
  //      {
  //          particleEmitterID = gameEngine->g_activeScene->ecs.CreateEntity();

  //          Tag& tag = gameEngine->g_activeScene->ecs.Add<Tag>(particleEmitterID);
  //          tag.tagName = "Particle Emitter";

  //          TransformComponent& transform = gameEngine->g_activeScene->ecs.Add<TransformComponent>(particleEmitterID);
  //          transform.m_LocalTransform.m_Translate = VgMath::Vector3(-5.0f, 0.0f, 0.0f);
  //          transform.m_LocalTransform.m_Scale = 0.48f;
  //          transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
  //              VgMath::Degrees(90.0),
  //              VgMath::Vector3(0.0, 1.0, 0.0).normalized()
  //          );
  //          
  //          ParticleEmitter& particleEmitter = gameEngine->g_activeScene->ecs.Add<ParticleEmitter, TransformComponent>(particleEmitterID);
  //          particleEmitter.particleToEmitEachTime = 25;
  //          particleEmitter.spawningValues.textureID = WIDOWS_LOGO_DIFFUSE_MAP;
  //          particleEmitter.spawningValues.spawningSurface.m_Size.x = 10.0f;
  //          particleEmitter.spawningValues.spawningSurface.m_Size.y = 10.0f;
  //          particleEmitter.spawningValues.startsize = 1.0f;
  //          particleEmitter.spawningValues.endsize = 0.0f;

  //      }

  //              
  //      Scene currScene;
  //      currScene.sceneName = "FirstScene";
  //      currScene.ecs = gameEngine->g_activeScene->ecs;

  //      std::string scenePath = GetRelativeProjectPathWithMarker() + "\\serializedScenes\\" + currScene.sceneName + ".drop";
  //      SceneSerializer::SerializeSceneAsText(
  //          scenePath
  //          , &currScene
  //      );

        Scene* currScene = gameEngine->g_activeScene;
        currScene->sceneName = "FirstScene";

        std::string scenePath = GetRelativeProjectPathWithMarker() + "\\serializedScenes\\" + currScene->sceneName + ".drop";
        SceneSerializer::DeserializeSceneFromText(
            scenePath
            , currScene
        );

        SceneGraph::CalculateWorldTransforms(gameEngine->g_activeScene->ecs);

//      // #TODO Add line component
//      {
//          // when emplace_back at back, it will call destructor
//          // on the last element to use the new back
//          // https://www.youtube.com/watch?v=FdaYlWOV084
//          //gameEngine->m_DrawableLines.reserve(3);
//          // To avoid resizing we cancel the copy constructor

//          gameEngine->m_DrawableLines.emplace_back(
//              glm::vec3(0),
//              glm::vec3(1)
//              , DEFAULT_HIT_COLOR
//          );

//          gameEngine->m_DrawableLines.emplace_back(
//         glm::vec3(1.1),
//              glm::vec3(2)
//              , COLOR_AMBER
//          );
//          
//          gameEngine->m_DrawableLines.emplace_back(
//         glm::vec3(2.1),
//              glm::vec3(3)
//          );
//      }

        m_VSync = gameEngine->GetWindowHandle().IsVSync();
	}

	virtual void OnUIRender() override
	{
        GameEngine* gameEngine = GameEngine::GetInstance();

		ImGui::Begin("Drop Rendering");	

        ImGui::Text("Fps: %d", (int)(1.0f/m_DebugDeltaTime));
        ImGui::Text("Average Fps: %d\n\tevery %.2f sec ", m_AverageFPS, m_FrameAverageCalulationDuration);

        ImGui::Separator();
        ImGui::Checkbox("Pause Particle Update", &m_PauseParticleUpdate);

        ImGui::Separator();
        ImGui::Checkbox("VSync", &m_VSync);

        ImGui::Separator();
        ImGui::Checkbox("Wireframe", &m_Wireframe);

        ImGui::Separator();
        ImGui::Checkbox("Draw Debug Lines", &m_DrawDebug);

        ImGui::Separator();
        if (ImGui::Button("Recompile all shaders \nin materials (R as shortcut)")) {
            Renderer::RecompileAllShadersInMaterials(
                gameEngine->g_activeScene->ecs.GetSingletonComponent<SceneContext>()
                , gameEngine->g_activeScene->ecs.GetSingletonComponent<RendererContext>()
            );
        }
        
        ImGui::Begin("Drop Scene");
        ImGui::Separator();
        ImGui::Text("Camera pos: \n\t%.3f, \n\t%.3f, \n\t%.3f", m_Camera.m_Position.x, m_Camera.m_Position.y, m_Camera.m_Position.z);
            
		ImGui::End();

		//ImGui::ShowDemoWindow();
	}

    virtual void OnUpdate(float deltaTime) {

        GameEngine* gameEngine = GameEngine::GetInstance();
        //Renderer& renderer = gameEngine->m_Renderer;

        gameEngine->GetWindowHandle().SetVSync(m_VSync);

        gameEngine->SetDrawDebug(m_DrawDebug);
        gameEngine->m_PauseParticleUpdate = m_PauseParticleUpdate;

        Callbacks();

        m_DebugDeltaTime = deltaTime;
        
        // full performance window, time to output the calculated data
        if (m_FrameAverageCalulationDuration <= gameEngine->GetTime() - m_StartingTimeFrameAverage)
        {
            m_AverageFPS = (uint32_t)(1.0f / averageDeltaTime);

            // reset the performance window data
            m_Frames = 1;
            m_SumDeltaTime = averageDeltaTime;
            m_StartingTimeFrameAverage = gameEngine->GetTime();
        }

        m_SumDeltaTime += m_DebugDeltaTime;
        m_Frames += 1;
        averageDeltaTime = m_SumDeltaTime / m_Frames;

        // Particle logic
        if (!m_PauseParticleUpdate)
        {
            if (waitTime < gameEngine->m_CurrentTime) {
        	    waitTime = gameEngine->m_CurrentTime + spawnDelay;

                ParticleEmitter& particleEmitter = gameEngine->g_activeScene->ecs.Get<ParticleEmitter>(particleEmitterID);
                TransformComponent& particleEmitterTransform = gameEngine->g_activeScene->ecs.Get<TransformComponent>(particleEmitterID);
                // this is done inside the Emit function, not super clean
                //particleEmitter.spawningValues.spawningSurface.m_Transform = &particleEmitterTransform.m_CumulatedTransform;
                EmitParticles(particleEmitter, particleEmitterTransform.cumulatedTransform);

                PBParticleEmitter& pbParticleEmitter = gameEngine->g_activeScene->ecs.Get<PBParticleEmitter>(pbParticleEmitterID);
                TransformComponent& pbParticleEmitterTransform = gameEngine->g_activeScene->ecs.Get<TransformComponent>(pbParticleEmitterID);
                EmitParticles(pbParticleEmitter, pbParticleEmitterTransform.cumulatedTransform);
            }
        }
    }

    void Callbacks()
    {
        const auto gameEngine = GameEngine::GetInstance();

        // if ESC is pressed, we close the application
        if (Input::IsKeyPressed(Key::Escape))
            GameEngine::GetInstance()->GetWindowHandle().SetShouldClose(true);
    }

    float m_DebugDeltaTime = 0.0f;

    float m_StartingTimeFrameAverage = 0.0f;
    float m_FrameAverageCalulationDuration = 1.0f;

    float averageDeltaTime = 0.0f;
    uint32_t m_AverageFPS = 0;
    float m_SumDeltaTime = 0.0f;
    uint64_t m_Frames = 0;

    bool m_DrawDebug = true;
    bool m_PauseParticleUpdate = false;

    // Particles Logic
    float waitTime = 0.0f;
    float spawnDelay = 10.0f;
    //EntityID particleEmitterID = NULL_ENTITY;
    EntityID particleEmitterID = 4;
    EntityID pbParticleEmitterID = 5;
};

Drop::GameEngine* Drop::CreateGameEngine(int argc, char** argv)
{
	Drop::GameEngineSpecification spec;
	spec.Name = "Drop Example";
    spec.Width = 1600;
    spec.Height = 900;

	Drop::GameEngine* gameEngine = new Drop::GameEngine(spec);

    gameEngine->SetGame<ExampleGame>();
	gameEngine->SetMenubarCallback([gameEngine]()
 	{
 		if (ImGui::BeginMenu("File"))
 		{
 			if (ImGui::MenuItem("Exit"))
 			{
 				gameEngine->Close();
 			}
 			ImGui::EndMenu();
 		}
 	});

	return gameEngine;
}