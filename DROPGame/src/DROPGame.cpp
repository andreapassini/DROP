#include "Drop/GameEngine.h"
#include "Drop/EntryPoint.h"

#include "Drop/input/Input.h"

using namespace Drop;

class ExampleGame : public Drop::Game
{
public:
	ExampleGame()
	{
		m_ShadowShader = Shader(
			"..\\Drop\\src\\Drop\\shaders\\19_shadowmap.vert",
			"..\\Drop\\src\\Drop\\shaders\\20_shadowmap.frag"
        );

		m_LightShader = Shader(
			"..\\Drop\\src\\Drop\\shaders\\21_ggx_tex_shadow.vert",
			"..\\Drop\\src\\Drop\\shaders\\22_ggx_tex_shadow.frag"
        );

        GameEngine* gameEngine = GameEngine::GetInstance();

		gameEngine->m_Models.emplace_back("../models/cube.obj");
        gameEngine->m_Models.emplace_back("../models/sphere.obj");
        gameEngine->m_Models.emplace_back("../models/bunny_lp.obj");
        gameEngine->m_Models.emplace_back("../models/plane.obj");

        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture("../textures/UV_Grid_Sm.png"));
        gameEngine->m_TextureIds.push_back(GameEngine::LoadTexture("../textures/SoilCracked.png"));

		gameEngine->m_SceneGraph.m_GameObjects[SceneGraph::ROOT_ID].m_LocalTransform.m_Rotate =
			VgMath::Quaternion::angleAxis(VgMath::Degrees(0.0),
            VgMath::Vector3(0.0, 1.0, 0.0).normalized());


		// Plane
        uint32_t planeSGHandle = gameEngine->m_SceneGraph.AddObject(SceneGraph::ROOT_ID);
        gameEngine->m_SceneGraph.m_GameObjects[planeSGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(0.0f, -1.0f, 0.0f);
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
        gameEngine->m_SceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(-3.0f, 1.0f, 0.0f);
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
        gameEngine->m_SceneGraph.m_GameObjects[cubeSGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
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
        gameEngine->m_SceneGraph.m_GameObjects[bunnySGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
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
        gameEngine->m_ModelMatrices.reserve(gameEngine->m_SceneGraph.m_GameObjects.size());

		// Now the 2 maps have all the keys as the SceneGraph
		for (auto& it : gameEngine->m_SceneGraph.m_GameObjects)
		{
            gameEngine->m_CumulatedTransforms[it.first] = VgMath::Transform();
            gameEngine->m_ModelMatrices[it.first] = glm::mat4(1.0f);
		}
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Hello");	
		ImGui::Button("Button");

        ImGui::Separator();
        ImGui::Text("Camera pos: \n\t%.3f, \n\t%.3f, \n\t%.3f", m_Camera.m_Position.x, m_Camera.m_Position.y, m_Camera.m_Position.z);

		ImGui::End();

		//ImGui::ShowDemoWindow();
	}

    virtual void OnUpdate(float deltaTime) {
        Renderer& const renderer = GameEngine::GetInstance()->m_Renderer;
        renderer.m_clearColor += renderer.m_ColorIncrement;
        if ((renderer.m_clearColor.x >= 1.0f
            || renderer.m_clearColor.y >= 1.0f
            || renderer.m_clearColor.z >= 1.0f)
            ||
            (renderer.m_clearColor.x <= 0.0f
                || renderer.m_clearColor.y <= 0.0f
                || renderer.m_clearColor.z <= 0.0f)
            )
        {
            renderer.m_ColorIncrement *= -1;
        }

        SubroutineKeyCallback();
    }

    //////////////////////////////////////////
    // callback for keyboard events
    void SubroutineKeyCallback()
    {
        const auto const gameEngine = GameEngine::GetInstance();

        //// if ESC is pressed, we close the application
        //if (Input::IsKeyPressed(Key::Escape))
        //    glfwSetWindowShouldClose(window, GL_TRUE);

        // if L is pressed, we activate/deactivate wireframe rendering of models
        if (Input::IsKeyDown(Key::L))
            m_Wireframe = true;
        else if (Input::IsKeyReleased(Key::L))
            m_Wireframe = false;

        GLuint new_subroutine;
        for (int i = 0; i < 9; i++)
        {
            int key = (int)Key::D0 + i;
            if (Input::IsKeyPressed(KeyCode(key)))
            {
                // "1" to "9" -> ASCII codes from 49 to 59
                // we subtract 48 (= ASCII CODE of "0") to have integers from 1 to 9
                // we subtract 1 to have indices from 0 to 8
                new_subroutine = (key - '0' - 1);
                if (new_subroutine < gameEngine->m_Shaders.size())
                {
                    m_CurrentSubroutine = new_subroutine;
                    gameEngine->PrintCurrentShader(m_CurrentSubroutine);
                }
            }

        }
    }

};

Drop::GameEngine* Drop::CreateGameEngine(int argc, char** argv)
{
	Drop::GameEngineSpecification spec;
	spec.Name = "Drop Example";
    spec.Width = 1600;
    spec.Height = 900;

	Drop::GameEngine* gameEngine = new Drop::GameEngine(spec);
    std::cout << gameEngine << std::endl; // TO BE REMOVED
	//gameEngine->PushGame<ExampleGame>();
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