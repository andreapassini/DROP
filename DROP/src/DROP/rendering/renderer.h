#pragma once

#include <vector>

#include "renderableObject.h"
#include "Drop/particles/particle.h"
#include "Drop/rendering/staticMeshComponent.h"   
#include "drawableBox.h"
#include "line.h"

// Loader for OpenGL extensions
// http://glad.dav1d.de/
// THIS IS OPTIONAL AND NOT REQUIRED, ONLY USE THIS IF YOU DON'T WANT GLAD TO INCLUDE windows.h
// GLAD will include windows.h for APIENTRY if it was not previously defined.
// Make sure you have the correct definition for APIENTRY for platforms which define _WIN32 but don't use __stdcall
#ifdef _WIN32
#define APIENTRY __stdcall
#endif

#define GLFW_INCLUDE_NONE
#include "../dependencies/GLAD/include/GLAD/glad.h"

// GLFW library to create window and to manage I/O
#include "../dependencies/GLFW/include/GLFW/glfw3.h"

// another check related to OpenGL loader
// confirm that GLAD didn't include windows.h
#ifdef _WINDOWS_
#error windows.h was included!
#endif

// Libs includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

// Drop includes
#include "DROP/rendering/shader.h"
#include "DROP/utils/camera.h"
#include "DROP/rendering/billboard.h"
#include "DROP/rendering/model.h"
#include "DROP/rendering/material.h"

#define FULL_COLOR_SHADER 0
#define ILLUMINATION_GGX_SHADER 1
#define SHADOW_SHADER 2
#define EMPTY_QUAD_SHADER 3
#define EMPTY_BOX_SHADER 4
#define BILLBOARD_SHADER 5

namespace Drop
{
    enum EClockWise {
        COUNTER_CLOCKWISE
        , CLOCKWISE
    };

    struct SceneContext {
        Camera* camera;
        //glm::mat4 view;
        //glm::mat4 projection;
        
        glm::mat4 lightSpaceMatrix;
        glm::vec3 lightDir;

        std::vector<Model>* models;
        std::vector<Material>* materials;
        std::vector<TextureID>* textuers;

        int32_t width;
        int32_t height;

        bool wireframe;
        bool vSync = false;
    };

    struct RendererContext {
        GLFWwindow* window = nullptr;

        GLuint depthMapFBO = 0;
        GLuint depthMap = 0;

        // Can we extend shadow to a Cascade Shadow Maps?
        // or even to Virtual texture Shadow Map
        const GLuint SHADOW_WIDTH = 1024;
        const GLuint SHADOW_HEIGHT = 1024;

        glm::vec3 clearColor = glm::vec3(0.26f, 0.46f, 0.98f);

        bool backFaceCulling = true;
        EClockWise clockWiseOrder = EClockWise::COUNTER_CLOCKWISE;

        bool drawDebug = false;

        std::vector<Shader> shaders{};
    };

    namespace Renderer
    {
        void Init(GLFWwindow* window, RendererContext& rendererContext);

        //void RenderScene(
        //    SceneContext& sceneContext
        //    , const RendererContext& rendererContext
        //    , const std::vector<RenderableObject>& renderableObjects
        //    , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
        //    , Shader* const shadow_shader
        //    , Shader* const illumination_shader
        //);

        //void RenderScene(
        //    SceneContext& sceneContext
        //    , const std::vector<RenderableObject>& renderableObjects
        //    , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
        //    , Shader* const illumination_shader
        //);

        void RenderParticles(
            SceneContext& sceneContext
            , std::vector<ParticleEmitter>& particleEmitters
            , Shader* const billboardShader
        );

        void RenderBillboard(
            SceneContext& sceneContext
            , const std::vector<Billboard>& billboards
            , Shader* const billboardShader
        );

        void DrawDebug(
            SceneContext& sceneContext
            , Shader* const debugShader
            , std::vector<Line>& drawableLines
        );

        void DrawParticleEmitterSurface(
            SceneContext& sceneContext
            , Shader* const debugShader
            , std::vector<ParticleEmitter>& drawableParticleEmitter
        );

        void Shutdown();

        void CalculateLightMatrix(
            SceneContext& sceneContext
        );
        
        void SetupShadowPass(
            SceneContext& sceneContext
            , RendererContext& rendererContext
        );        
        void DrawMeshForShadow(
            const StaticMeshComponent& model
            , VgMath::Transform& worldTransform
            , SceneContext& sceneContext
            , RendererContext& rendererContext
        );

        void SetupColorPass(
            SceneContext& sceneContext
            , RendererContext& rendererContext
        );
        void DrawMesh(
            const StaticMeshComponent& model
            , VgMath::Transform& worldTransform
            , SceneContext& sceneContext
            , RendererContext& rendererContext
        );
    };
}