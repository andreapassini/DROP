#pragma once
#include "renderableObject.h"

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

namespace Drop
{
    class Renderer
    {
    public:
        Renderer(
            uint32_t screenWidth_val,
            uint32_t screenHeight_val
        );
        void Init(GLFWwindow* window);
        ~Renderer();
        void RenderScene(
            const std::vector<RenderableObject>& m_RenderableObjects,
            const std::vector<int>& textures,
            const std::vector<Model>& models,
            const std::vector<Material>& materials,
            const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec3& lightDir,
            Shader* const shadow_shader,
            Shader* const illumination_shader,
            const GLuint depthMapFBO,
            const GLuint depthMap,
            const GLboolean wireframe,
            const int width,
            const int height
        ) const ;
        void DrawDebug(
            const glm::mat4& view,
            const glm::mat4& projection,
            Shader* const debugShader,
            std::vector<Line>& drawableLines,
            const int width,
            const int height
        ) const;
    public:
        GLFWwindow* m_Window;
        GLuint m_DepthMapFBO;
        GLuint m_DepthMap;
        glm::vec3 m_clearColor = glm::vec3(0.26f, 0.46f, 0.98f);

        // For Debug - TO BE REMOVED
        float m_ColorIncrement = 0.001f;

    private:
        const GLuint SHADOW_WIDTH;
        const GLuint SHADOW_HEIGHT;
    };
}