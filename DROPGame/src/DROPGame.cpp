#include "Drop/GameEngine.h"
#include "Drop/EntryPoint.h"

#include "Drop/input/Input.h"
#include "Drop/sceneGraph/sceneGraph.h"
#include "Drop/sceneGraph/scene.h"
#include "Drop/utils/ExecPath.h"
#include "Drop/utils/SceneSerializer.h"
#include "DROP/tags/tag.h"
#include "DROP/utils/Log.h"

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
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            GetFullPath("\\DROP\\src\\Drop\\shaders\\00_basic.vert").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\01_fullcolor.frag").c_str()
        );

        // ILLUMINATION_GGX_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            GetFullPath("\\DROP\\src\\Drop\\shaders\\21_ggx_tex_shadow.vert").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\ggx_tex_shadow_noSub.frag").c_str()
        );

        // SHADOW_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            GetFullPath("\\DROP\\src\\Drop\\shaders\\19_shadowmap.vert").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\20_shadowmap.frag").c_str()
        );

        // EMPTY_QUAD_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            GetFullPath("\\DROP\\src\\Drop\\shaders\\quadFrustum.vert").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\quadFrustum.geom").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\quadFrustum.frag").c_str()
        );

        // EMPTY_BOX_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            GetFullPath("\\DROP\\src\\Drop\\shaders\\boxFrustum.vert").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\boxFrustum.geom").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\boxFrustum.frag").c_str()
        );

        // BILLBOARD_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            GetFullPath("\\DROP\\src\\Drop\\shaders\\billboard.vert").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\billboard.geom").c_str()
            , GetFullPath("\\DROP\\src\\Drop\\shaders\\billboard.frag").c_str()
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

        gameEngine->m_ECS.RegisterComponent<Tag>();
        gameEngine->m_ECS.RegisterComponent<TransformComponent>();

        // #TODO 
        // We will have multiple cameras, but only one active camera 
        //	as a Singleton Component
        //gameEngine->m_ECS.RegisterComponent<Camera, Transform>();

        gameEngine->m_ECS.RegisterComponent<StaticMeshComponent, TransformComponent>();

        gameEngine->m_ECS.RegisterComponent<ParticleEmitter, TransformComponent>();
        //gameEngine->m_ECS.RegisterComponent<Billboard, Transform>();
        //gameEngine->m_ECS.RegisterComponent<PhysicsObject>();

		//// Plane
  //      {
  //          //uint32_t planeSGHandle = gameEngine->m_SceneGraph.AddObject(SceneGraph::ROOT_ID);
  //          bseecs::EntityID id = gameEngine->m_ECS.CreateEntity();

  //          Tag& tag = gameEngine->m_ECS.Add<Tag>(id);
  //          tag.tagName = "Plane";

  //          TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(id);
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

  //          StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(id);
  //          staticMesh.bCastShadow = true;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 3;
  //      }
  //    

  //      // Sphere
  //      bseecs::EntityID sphereId = gameEngine->m_ECS.CreateEntity();
  //      {
  //          Tag& tag = gameEngine->m_ECS.Add<Tag>(sphereId);
  //          tag.tagName = "Sphere";

  //          TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(sphereId);
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

  //          StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(sphereId);
  //          staticMesh.bCastShadow = true;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 1;
  //      }


  //      // Cube
  //      {
  //          bseecs::EntityID id = gameEngine->m_ECS.CreateEntity();

  //          Tag& tag = gameEngine->m_ECS.Add<Tag>(id);
  //          tag.tagName = "Cube";

  //          TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(id);
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

  //          StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(id);
  //          staticMesh.bCastShadow = true;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 0;
  //      }


  //      // Bunny
  //      {
  //          bseecs::EntityID id = gameEngine->m_ECS.CreateEntity();

  //          Tag& tag = gameEngine->m_ECS.Add<Tag>(id);
  //          tag.tagName = "Bunny";

  //          TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(id);
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

  //          StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(id);
  //          staticMesh.bCastShadow = false;
  //          staticMesh.materialId = gameEngine->m_Materials.size() - 1;
  //          staticMesh.modelId = 2;
  //      }

  //      
  //      // Particle Emitter
  //      {
  //          particleEmitterID = gameEngine->m_ECS.CreateEntity();

  //          Tag& tag = gameEngine->m_ECS.Add<Tag>(particleEmitterID);
  //          tag.tagName = "Particle Emitter";

  //          TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(particleEmitterID);
  //          transform.m_LocalTransform.m_Translate = VgMath::Vector3(-5.0f, 0.0f, 0.0f);
  //          transform.m_LocalTransform.m_Scale = 0.48f;
  //          transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
  //              VgMath::Degrees(90.0),
  //              VgMath::Vector3(0.0, 1.0, 0.0).normalized()
  //          );
  //          
  //          ParticleEmitter& particleEmitter = gameEngine->m_ECS.Add<ParticleEmitter, TransformComponent>(particleEmitterID);
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
  //      currScene.ecs = gameEngine->m_ECS;

  //      std::string scenePath = GetRelativeProjectPathWithMarker() + "\\serializedScenes\\" + currScene.sceneName + ".drop";
  //      SceneSerializer::SerializeSceneAsText(
  //          scenePath
  //          , &currScene
  //      );

        Scene currScene;
        currScene.ecs.RegisterComponent<Tag>();
        currScene.ecs.RegisterComponent<TransformComponent>();
        currScene.ecs.RegisterComponent<StaticMeshComponent, TransformComponent>();
        currScene.ecs.RegisterComponent<ParticleEmitter, TransformComponent>();

        std::string scenePath = GetRelativeProjectPathWithMarker() + "\\serializedScenes\\" + currScene.sceneName + ".drop";
        SceneSerializer::DeserializeSceneFromText(
            scenePath
            , &currScene
        );

        // TODO Dont pass it via copy
        gameEngine->m_ECS = currScene.ecs;
        SceneGraph::CalculateWorldTransforms(gameEngine->m_ECS);

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

                ParticleEmitter& particleEmitter = gameEngine->m_ECS.Get<ParticleEmitter>(particleEmitterID);
                TransformComponent& particleEmitterTransform = gameEngine->m_ECS.Get<TransformComponent>(particleEmitterID);
                // this is done inside the Emit function, not super clean
                //particleEmitter.spawningValues.spawningSurface.m_Transform = &particleEmitterTransform.m_CumulatedTransform;
                EmitParticles(particleEmitter, particleEmitterTransform.cumulatedTransform);
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
    float spawnDelay = 0.50f;
    EntityID particleEmitterID = NULL_ENTITY;
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