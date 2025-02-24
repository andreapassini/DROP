#include "Drop/GameEngine.h"
#include "Drop/EntryPoint.h"

#include "Drop/input/Input.h"

using namespace Drop;

class ExampleGame : public Drop::Game
{
public:
	ExampleGame()
	{
		//m_ShadowShader = Shader(
		//	"..\\DROP\\src\\DROP\\shaders\\19_shadowmap.vert",
		//	"..\\DROP\\src\\DROP\\shaders\\20_shadowmap.frag"
  //      );

		//m_LightShader = Shader(
		//	"..\\DROP\\src\\DROP\\shaders\\21_ggx_tex_shadow.vert",
		//	"..\\DROP\\src\\DROP\\shaders\\ggx_tex_shadow_noSub.frag"
  //      );

        GameEngine* gameEngine = GameEngine::GetInstance();

		gameEngine->m_Models.emplace_back("..\\models\\cube.obj");
        gameEngine->m_Models.emplace_back("..\\models\\sphere.obj");
        gameEngine->m_Models.emplace_back("..\\models\\bunny_lp.obj");
        gameEngine->m_Models.emplace_back("..\\models\\plane.obj");

        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture("..\\textures\\UV_Grid_Sm.png"));
        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture("..\\textures\\SoilCracked.png"));

		gameEngine->m_SceneGraph.m_GameObjects[SceneGraph::ROOT_ID].m_LocalTransform.m_Rotate =
			VgMath::Quaternion::angleAxis(VgMath::Degrees(0.0),
            VgMath::Vector3(0.0, 1.0, 0.0).normalized());


		// Plane
        uint32_t planeSGHandle = gameEngine->m_SceneGraph.AddObject(SceneGraph::ROOT_ID);
        gameEngine->m_SceneGraph.m_GameObjects[planeSGHandle].m_LocalTransform.m_Translate = 
            VgMath::Vector3(0.0f, -1.0f, 0.0f);
        gameEngine->m_SceneGraph.m_GameObjects[planeSGHandle].m_LocalTransform.m_Scale = 10.0f;
        gameEngine->m_SceneGraph.m_GameObjects[planeSGHandle].m_LocalTransform.m_Rotate =
            VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0), 
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );
                    
        Material materialPlane;
        materialPlane.Kd = 3.0f;
        materialPlane.Alpha = 0.2f;
        materialPlane.F0 = 0.9f;
        materialPlane.CastShadow = false;
        materialPlane.UseTexture = true;
        materialPlane.TextureId = 1;
        materialPlane.Repeat = 80.0f;

        gameEngine->m_Materials.push_back(materialPlane);

        RenderableObject renderableObjectPlane = RenderableObject{
                planeSGHandle,
                3,
                (uint32_t)(gameEngine->m_Materials.size() - 1)
        };
        gameEngine->m_RendereableObjects.push_back(renderableObjectPlane);
        


        // Sphere
        uint32_t sphereSGHandle = gameEngine->m_SceneGraph.AddObject(SceneGraph::ROOT_ID);
        gameEngine->m_SceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Translate = 
            VgMath::Vector3(-3.0f, 1.0f, 0.0f);
        gameEngine->m_SceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Scale = 1.0f;
        gameEngine->m_SceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Rotate =
            VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0), 
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );
        
        {
            Material material = Material{
                3.0f,   // Kd 
                0.2f,   // Alpha
                0.9f,   
                true,
                true,   // UseTexture
                0,      // TextureId
                1.0f    // Repeat
            };
            gameEngine->m_Materials.push_back(material);

            RenderableObject renderableObject = RenderableObject{
                sphereSGHandle,
                1,
                (uint32_t)(gameEngine->m_Materials.size() - 1)
            };
            gameEngine->m_RendereableObjects.push_back(renderableObject);
        }


        // Cube
        uint32_t cubeSGHandle = gameEngine->m_SceneGraph.AddObject(sphereSGHandle);
        gameEngine->m_SceneGraph.m_GameObjects[cubeSGHandle].m_LocalTransform.m_Translate = 
            VgMath::Vector3(3.0f, 1.0f, 0.0f);
        gameEngine->m_SceneGraph.m_GameObjects[cubeSGHandle].m_LocalTransform.m_Scale = 0.48f;
        gameEngine->m_SceneGraph.m_GameObjects[cubeSGHandle].m_LocalTransform.m_Rotate =
            VgMath::Quaternion::angleAxis(
                VgMath::Degrees(90.0), 
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );

        {
            Material material = Material{
                3.0f,   // Kd  
                0.2f,   // Alpha
                0.9f,   // F0
                false,  // CastShadow
                true,   // UseTexture
                0,      // TextureId
                1.0f    // Repeat
            };
            gameEngine->m_Materials.push_back(material);

            RenderableObject renderableObject = RenderableObject{
                cubeSGHandle,
                0,
                (uint32_t)(gameEngine->m_Materials.size() - 1)
            };
            gameEngine->m_RendereableObjects.push_back(renderableObject);
        }


        // Bunny
        uint32_t bunnySGHandle = gameEngine->m_SceneGraph.AddObject(SceneGraph::ROOT_ID);
        gameEngine->m_SceneGraph.m_GameObjects[bunnySGHandle].m_LocalTransform.m_Translate = 
            VgMath::Vector3(3.0f, 1.0f, 0.0f);
        gameEngine->m_SceneGraph.m_GameObjects[bunnySGHandle].m_LocalTransform.m_Scale = 0.3f;
        gameEngine->m_SceneGraph.m_GameObjects[bunnySGHandle].m_LocalTransform.m_Rotate =
            VgMath::Quaternion::angleAxis(
                VgMath::Degrees(0.0), 
                VgMath::Vector3(0.0, 1.0, 0.0).normalized()
            );

        {
            Material material = Material{
                3.0f,
                0.2f,
                0.9f,
                true,
                true,           // UseTexture
                0,              // TextureId
                1.0f           // Repeat
            };
            gameEngine->m_Materials.push_back(material);

            RenderableObject renderableObject = RenderableObject{
                bunnySGHandle,
                2,
                (uint32_t)(gameEngine->m_Materials.size() - 1)
            };
            gameEngine->m_RendereableObjects.push_back(renderableObject);
        }

        gameEngine->m_CumulatedTransforms.reserve(gameEngine->m_SceneGraph.m_GameObjects.size());

		// Now the 2 maps have all the keys as the SceneGraph
		for (auto& it : gameEngine->m_SceneGraph.m_GameObjects)
		{
            gameEngine->m_CumulatedTransforms[it.first] = VgMath::Transform();
		}

        {
            // when emplace_back at back, it will call destructor
            // on the last element to use the new back
            // https://www.youtube.com/watch?v=FdaYlWOV084
            //gameEngine->m_DrawableLines.reserve(3);
            // To avoid resizing we cancel the copy constructor

            gameEngine->m_DrawableLines.emplace_back(
                glm::vec3(0),
                glm::vec3(1)
                , DEFAULT_HIT_COLOR
            );

            gameEngine->m_DrawableLines.emplace_back(
           glm::vec3(1.1),
                glm::vec3(2)
                , COLOR_AMBER
            );
            
            gameEngine->m_DrawableLines.emplace_back(
           glm::vec3(2.1),
                glm::vec3(3)
            );
        }

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
        Renderer& renderer = gameEngine->m_Renderer;

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