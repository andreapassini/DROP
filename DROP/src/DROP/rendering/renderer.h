#pragma once

#include <vector>

#include "renderableObject.h"
#include "DROP/particles/particle.h"
#include "DROP/rendering/staticMeshComponent.h"   
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
#include <glad/glad.h>

// GLFW library to create window and to manage I/O
#include <glfw/glfw3.h>

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

#define FULL_COLOR_SHADER 0
#define ILLUMINATION_GGX_SHADER 1
#define SHADOW_SHADER 2
#define EMPTY_QUAD_SHADER 3
#define BILLBOARD_SHADER 4

namespace Drop
{
    struct SceneContext {
        // removed const otherwise we could not change the camera
        glm::mat4& view;
        glm::mat4& projection;
        glm::vec3& lightDir;

        std::vector<Model>& models;
        std::vector<Material>& materials;
        std::vector<TextureID>& textuers;

        int width;
        int height;

        GLboolean wireframe;

        std::unordered_map<uint32_t, VgMath::Transform> m_CumulatedTransforms;
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

        bool drawDebug = false;

        std::vector<Shader> shaders{
            //FULL_COLOR_SHADER
            Shader( 
                "DROP\\DROP\\src\\DROP\\shaders\\00_basic.vert",
                "DROP\\DROP\\src\\DROP\\shaders\\01_fullcolor.frag")
            
            , // ILLUMINATION_GGX_SHADER
                Shader( 
                "DROP\\DROP\\src\\DROP\\shaders\\21_ggx_tex_shadow.vert",
                "DROP\\DROP\\src\\DROP\\shaders\\ggx_tex_shadow_noSub.frag")

            , // SHADOW_SHADER
                Shader( 
                "DROP\\DROP\\src\\DROP\\shaders\\19_shadowmap.vert",
                "DROP\\DROP\\src\\DROP\\shaders\\20_shadowmap.frag")
            
            , // EMPTY_QUAD_SHADER
                Shader( 
                "DROP\\DROP\\src\\DROP\\shaders\\quadFrustum.vert",
                "DROP\\DROP\\src\\DROP\\shaders\\quadFrustum.geom",
                "DROP\\DROP\\src\\DROP\\shaders\\quadFrustum.frag")
            
            , // EMPTY_BOX_SHADER
                Shader( 
                "DROP\\DROP\\src\\DROP\\shaders\\boxFrustum.vert",
                "DROP\\DROP\\src\\DROP\\shaders\\boxFrustum.geom",
                "DROP\\DROP\\src\\DROP\\shaders\\boxFrustum.frag")

            , // BILLBOARD_SHADER
                Shader(
                "DROP\\DROP\\src\\DROP\\shaders\\billboard.vert",
                "DROP\\DROP\\src\\DROP\\shaders\\billboard.geom",
                "DROP\\DROP\\src\\DROP\\shaders\\billboard.frag")
        };

        // USE FULL_COLOR_SHADER
        //Shader m_DebugShader = Shader(
        //    "DROP\\DROP\\src\\DROP\\shaders\\00_basic.vert",
        //    "DROP\\DROP\\src\\DROP\\shaders\\01_fullcolor.frag");

        // USE ILLUMINATION_GGX_SHADER
        //Shader m_LightShader = Shader(
        //    "DROP\\DROP\\src\\DROP\\shaders\\21_ggx_tex_shadow.vert",
        //    "DROP\\DROP\\src\\DROP\\shaders\\ggx_tex_shadow_noSub.frag");

        // USE SHADOW_SHADER
        //Shader m_ShadowShader = Shader(
        //    "DROP\\DROP\\src\\DROP\\shaders\\19_shadowmap.vert",
        //    "DROP\\DROP\\src\\DROP\\shaders\\20_shadowmap.frag");

        // USE EMPTY_QUAD_SHADER
        //Shader m_EmptyQuadShader = Shader(
        //    "DROP\\DROP\\src\\DROP\\shaders\\quadFrustum.vert",
        //    "DROP\\DROP\\src\\DROP\\shaders\\quadFrustum.geom",
        //    "DROP\\DROP\\src\\DROP\\shaders\\quadFrustum.frag");

        // USE EMPTY_BOX_SHADER
        //Shader m_EmptyBoxShader = Shader(
        //    "DROP\\DROP\\src\\DROP\\shaders\\boxFrustum.vert",
        //    "DROP\\DROP\\src\\DROP\\shaders\\boxFrustum.geom",
        //    "DROP\\DROP\\src\\DROP\\shaders\\boxFrustum.frag");

        // USE BILLBOARD_SHADER
        //Shader m_BillboardShader = Shader(
        //    "DROP\\DROP\\src\\DROP\\shaders\\billboard.vert",
        //    "DROP\\DROP\\src\\DROP\\shaders\\billboard.geom",
        //    "DROP\\DROP\\src\\DROP\\shaders\\billboard.frag");
    };

    namespace Renderer
    {
        void Init(GLFWwindow* window, RendererContext& rendererContext);

        void RenderScene(
            const SceneContext& sceneContext
            , const RendererContext& rendererContext
            , const std::vector<RenderableObject>& renderableObjects
            , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
            , Shader* const shadow_shader
            , Shader* const illumination_shader
        );

        void RenderScene(
            const SceneContext& sceneContext
            , const std::vector<RenderableObject>& renderableObjects
            , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
            , Shader* const illumination_shader
        );

        void RenderParticles(
            const SceneContext& sceneContext
            , std::vector<ParticleEmitter>& particleEmitters
            , Shader* const billboardShader
        );

        void RenderBillboard(
            const SceneContext& sceneContext
            , const std::vector<Billboard>& billboards
            , Shader* const billboardShader
        );

        void DrawDebug(
            const SceneContext& sceneContext
            , Shader* const debugShader
            , std::vector<Line>& drawableLines
        );

        void DrawParticleEmitterSurface(
            const SceneContext& sceneContext
            , Shader* const debugShader
            , std::vector<ParticleEmitter>& drawableParticleEmitter
        );

        void Shutdown();

        void Draw_ShadowPass(
            const StaticMeshComponent& staticMeshComponent
            , const VgMath::Transform& worldTransform
            , const SceneContext& sceneContext
            , const RendererContext& rendererContext
        );

        void Draw_IlluminationPass(
            const StaticMeshComponent& staticMeshComponent
            , const VgMath::Transform& worldTransform
            , const SceneContext& sceneContext
            , const RendererContext& rendererContext
        );


    };
}