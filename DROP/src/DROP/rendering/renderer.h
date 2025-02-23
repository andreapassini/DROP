#pragma once

#include "renderableObject.h"
#include "../particles/particle.h"

#include <vector>
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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../rendering/shader.h"
#include "../utils/camera.h"
#include "../rendering/billboard.h"

namespace Drop
{
    struct SceneContext {
        const glm::mat4& view;
        const glm::mat4& projection;
        const glm::vec3& lightDir;

        const std::vector<int>& textures;
        const std::vector<Model>& models;
        const std::vector<Material>& materials;

        int width;
        int height;

        GLboolean wireframe;
    };

    struct RendererContext {
        GLFWwindow* window = nullptr;

        GLuint depthMapFBO = 0;
        GLuint depthMap = 0;

        const GLuint SHADOW_WIDTH = 1024;
        const GLuint SHADOW_HEIGHT = 1024;

        glm::vec3 clearColor = glm::vec3(0.26f, 0.46f, 0.98f);
    };

    class Renderer
    {
    public:
        Renderer();
        void Init(GLFWwindow* window, RendererContext& rendererContext);
        ~Renderer();
        void RenderScene(
            const SceneContext& sceneContext
            , const RendererContext& rendererContext
            , const std::vector<RenderableObject>& renderableObjects
            , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
            , Shader* const shadow_shader
            , Shader* const illumination_shader
        ) const;

        void RenderScene(
            const SceneContext& sceneContext
            , const std::vector<RenderableObject>& renderableObjects
            , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
            , Shader* const illumination_shader
        ) const;

        void RenderParticles(
            const SceneContext& sceneContext
            , const Particle* const particles
            , const uint32_t numberOfParticles
            , Shader* const billboardShader
        ) const;

        void RenderBillboard(
            const SceneContext& sceneContext
            , const std::vector<Billboard>& billboards
            , Shader* const billboardShader
        ) const;

        void DrawDebug(
            const SceneContext& sceneContext
            , Shader* const debugShader
            , std::vector<Line>& drawableLines
        ) const;

        void DrawParticleEmitterSurface(
            const SceneContext& sceneContext
            , Shader* const debugShader
            , std::vector<ParticleEmitter>& drawableParticleEmitter
        ) const;

        void Shutdown();
    };
}