#include "Drop/GameEngine.h"
#include "Drop/EntryPoint.h"

#include "Drop/input/Input.h"
#include "DROP/sceneGraph/sceneGraph.h"

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
            "..\\Drop\\src\\DROP\\shaders\\00_basic.vert"
            , "..\\Drop\\src\\DROP\\shaders\\01_fullcolor.frag"
        );

        // ILLUMINATION_GGX_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            "..\\Drop\\src\\DROP\\shaders\\21_ggx_tex_shadow.vert"
            , "..\\Drop\\src\\DROP\\shaders\\ggx_tex_shadow_noSub.frag"
        );

        // SHADOW_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            "..\\Drop\\src\\DROP\\shaders\\19_shadowmap.vert"
            , "..\\Drop\\src\\DROP\\shaders\\20_shadowmap.frag"
        );

        // EMPTY_QUAD_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            "..\\Drop\\src\\DROP\\shaders\\quadFrustum.vert"
            , "..\\Drop\\src\\DROP\\shaders\\quadFrustum.geom"
            , "..\\Drop\\src\\DROP\\shaders\\quadFrustum.frag"
        );

        // EMPTY_BOX_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            "..\\Drop\\src\\DROP\\shaders\\boxFrustum.vert"
            , "..\\Drop\\src\\DROP\\shaders\\boxFrustum.geom"
            , "..\\Drop\\src\\DROP\\shaders\\boxFrustum.frag"
        );

        // BILLBOARD_SHADER
        gameEngine->m_ECS.GetSingletonComponent<RendererContext>().shaders.emplace_back(
            "..\\Drop\\src\\DROP\\shaders\\billboard.vert"
            , "..\\Drop\\src\\DROP\\shaders\\billboard.geom"
            , "..\\Drop\\src\\DROP\\shaders\\billboard.frag"
        );
#pragma endregion


        gameEngine->m_Models.emplace_back("..\\models\\cube.obj");
        gameEngine->m_Models.emplace_back("..\\models\\sphere.obj");
        gameEngine->m_Models.emplace_back("..\\models\\bunny_lp.obj");
        gameEngine->m_Models.emplace_back("..\\models\\plane.obj");

        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture("..\\textures\\UV_Grid_Sm.png"));
        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture("..\\textures\\SoilCracked.png"));

        // Testing ECS
        gameEngine->m_ECS.RegisterComponent<TransformComponent>();

        // #TODO 
        // We will have multiple cameras, but only one active camera 
        //	as a Singleton Component
        //gameEngine->m_ECS.RegisterComponent<Camera, Transform>();

        gameEngine->m_ECS.RegisterComponent<StaticMeshComponent, TransformComponent>();

        // #TODO to this later, for now focus on simple mesh rendering
        gameEngine->m_ECS.RegisterComponent<ParticleEmitter, TransformComponent>();
        //gameEngine->m_ECS.RegisterComponent<Billboard, Transform>();
        //gameEngine->m_ECS.RegisterComponent<PhysicsObject>();

		// Plane
        {
            //uint32_t planeSGHandle = gameEngine->m_SceneGraph.AddObject(SceneGraph::ROOT_ID);
            bseecs::EntityID id = gameEngine->m_ECS.CreateEntity();

            TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(id);
            transform.m_LocalTransform.m_Translate = VgMath::Vector3(0.0f, -1.0f, 0.0f);
            transform.m_LocalTransform.m_Scale = 10.0f;
            transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0),
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );

            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = 1;
            material.textures[0].UVRepeat = 80.0f;
            material.shaderID = ILLUMINATION_GGX_SHADER;
            gameEngine->m_Materials.push_back(material);

            StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(id);
            staticMesh.bCastShadow = true;
            staticMesh.materialId = gameEngine->m_Materials.size() - 1;
            staticMesh.modelId = 3;
        }
      

        // Sphere
        bseecs::EntityID sphereId = gameEngine->m_ECS.CreateEntity();
        {
            TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(sphereId);
            transform.m_LocalTransform.m_Translate = VgMath::Vector3(-3.0f, 1.0f, 0.0f);
            transform.m_LocalTransform.m_Scale = 1.0f;
            transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0),
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );

            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = 0;
            material.textures[0].UVRepeat = 1.0f;
            material.shaderID = ILLUMINATION_GGX_SHADER;
            gameEngine->m_Materials.push_back(material);

            StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(sphereId);
            staticMesh.bCastShadow = true;
            staticMesh.materialId = gameEngine->m_Materials.size() - 1;
            staticMesh.modelId = 1;
        }


        // Cube
        {
            bseecs::EntityID id = gameEngine->m_ECS.CreateEntity();

            TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(id);
            transform.m_Parent = sphereId;
            transform.m_LocalTransform.m_Translate = VgMath::Vector3(7.0f, 1.0f, 0.0f);
            transform.m_LocalTransform.m_Scale = 0.48f;
            transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0),
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );

            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = 0;
            material.textures[0].UVRepeat = 1.0f;
            material.shaderID = ILLUMINATION_GGX_SHADER;
            gameEngine->m_Materials.push_back(material);

            StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(id);
            staticMesh.bCastShadow = true;
            staticMesh.materialId = gameEngine->m_Materials.size() - 1;
            staticMesh.modelId = 0;
        }


        // Bunny
        {
            bseecs::EntityID id = gameEngine->m_ECS.CreateEntity();

            TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(id);
            transform.m_Parent = sphereId;
            transform.m_LocalTransform.m_Translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
            transform.m_LocalTransform.m_Scale = 0.48f;
            transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0),
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );

            Material material;
            material.kd = 3.0f;
            material.alpha = 0.2f;
            material.f0 = 0.9f;
            material.textures[0].textureId = 0;
            material.textures[0].UVRepeat = 1.0f;
            material.shaderID = ILLUMINATION_GGX_SHADER;
            gameEngine->m_Materials.push_back(material);

            StaticMeshComponent& staticMesh = gameEngine->m_ECS.Add<StaticMeshComponent, TransformComponent>(id);
            staticMesh.bCastShadow = false;
            staticMesh.materialId = gameEngine->m_Materials.size() - 1;
            staticMesh.modelId = 2;
        }

        
        // Particle Emitter
        {
            particleEmitterID = gameEngine->m_ECS.CreateEntity();

            TransformComponent& transform = gameEngine->m_ECS.Add<TransformComponent>(particleEmitterID);
            transform.m_LocalTransform.m_Translate = VgMath::Vector3(-5.0f, 0.0f, 0.0f);
            transform.m_LocalTransform.m_Scale = 0.48f;
            transform.m_LocalTransform.m_Rotate = VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0),
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );
            
            ParticleEmitter& particleEmitter = gameEngine->m_ECS.Add<ParticleEmitter, TransformComponent>(particleEmitterID);
            particleEmitter.particleToEmitEachTime = 10;
            particleEmitter.spawningValues.textureID = 0;
            particleEmitter.spawningValues.spawningSurface.m_Size.x = 10.0f;
            particleEmitter.spawningValues.spawningSurface.m_Size.y = 10.0f;
            particleEmitter.spawningValues.startsize = 1.0f;
            particleEmitter.spawningValues.endsize = 0.0f;

        }


        SceneGraph::CalculateWorldTransforms(gameEngine->m_ECS);

		//// Now the 2 maps have all the keys as the SceneGraph
		//for (auto& it : gameEngine->m_SceneGraph.m_GameObjects)
		//{
  //          gameEngine->m_CumulatedTransforms[it.first] = VgMath::Transform();
		//}

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
        if (waitTime < gameEngine->m_CurrentTime) {
        	waitTime = gameEngine->m_CurrentTime + spawnDelay;

            ParticleEmitter& particleEmitter = gameEngine->m_ECS.Get<ParticleEmitter>(particleEmitterID);
            TransformComponent& particleEmitterTransform = gameEngine->m_ECS.Get<TransformComponent>(particleEmitterID);
            // this is done inside the Emit function, not super clean
            //particleEmitter.spawningValues.spawningSurface.m_Transform = &particleEmitterTransform.m_CumulatedTransform;
            EmitParticles(particleEmitter, particleEmitterTransform.m_CumulatedTransform);
            std::cout << "Emit" << std::endl;
        }

        if (m_StopEachFrame)
        {
            std::cin.get();
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