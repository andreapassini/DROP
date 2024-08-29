#include "renderer.h"

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

#include "colors.h"

#define UNLOCK_FRAMERTE

namespace Drop
{
    Renderer::Renderer(
        uint32_t screenWidth_val,
        uint32_t screenHeight_val
    ) :
        SHADOW_WIDTH(1024),
        SHADOW_HEIGHT(1024)
    {
    }

    void Renderer::Init(GLFWwindow* window)
    {
        // We are in Drop/Drop!!!
        // 
        m_Window = window;

        // we enable Z test
        glEnable(GL_DEPTH_TEST);

        //the "clear" color for the frame buffer
        glClearColor(DEFAULT_CLEAR_COLOR.r, DEFAULT_CLEAR_COLOR.g, DEFAULT_CLEAR_COLOR.b, 1.0f);

        /////////////////// CREATION OF BUFFER FOR THE  DEPTH MAP /////////////////////////////////////////
        // buffer dimension: too large -> performance may slow down if we have many lights; too small -> strong aliasing
        //GLuint depthMapFBO;
        // we create a Frame Buffer Object: the first rendering step will render to this buffer, and not to the real frame buffer
        glGenFramebuffers(1, &m_DepthMapFBO);
        // we create a texture for the depth map
        //GLuint depthMap;
        glGenTextures(1, &m_DepthMap);
        glBindTexture(GL_TEXTURE_2D, m_DepthMap);
        // in the texture, we will save only the depth data of the fragments. Thus, we specify that we need to render only depth in the first rendering step
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // we set to clamp the uv coordinates outside [0,1] to the color of the border
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // outside the area covered by the light frustum, everything is rendered in shadow (because we set GL_CLAMP_TO_BORDER)
        // thus, we set the texture border to white, so to render correctly everything not involved by the shadow map
        //*************
        GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // we bind the depth map FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
        // we set that we are not calculating nor saving color data
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ///////////////////////////////////////////////////////////////////

    }

    void Renderer::RenderScene(
        const std::vector<RenderableObject>& renderableObjects,
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
    ) const
    {
        /////////////////// STEP 1 - SHADOW MAP: RENDERING OF SCENE FROM LIGHT POINT OF VIEW ////////////////////////////////////////////////
        // we set view and projection matrix for the rendering using light as a camera
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = -10.0f, far_plane = 10.0f;
        GLfloat frustumSize = 5.0f;
        // for a directional light, the projection is orthographic. For point lights, we should use a perspective projection
        lightProjection = glm::ortho(-frustumSize, frustumSize, -frustumSize, frustumSize, near_plane, far_plane);
        // the light is directional, so technically it has no position. We need a view matrix, so we consider a position on the the direction vector of the light
        lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // transformation matrix for the light
        lightSpaceMatrix = lightProjection * lightView;
        /// We "install" the  Shader Program for the shadow mapping creation
        shadow_shader->Use();
        // we pass the transformation matrix as uniform
        glUniformMatrix4fv(glGetUniformLocation(shadow_shader->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        // we set the viewport for the first rendering step = dimensions of the depth texture
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        // we activate the FBO for the depth map rendering
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // we render the scene, using the shadow shader
        size_t size = renderableObjects.size();
        for (size_t i = 0; i < size; i++)
        {
            renderableObjects[i].Draw(
                *shadow_shader,
                textures,
                models,
                materials,
                cumulatedTransforms,
                view,
                (renderableObjects)[i].SHADOWMAP,
                depthMap
            );
        }

        /////////////////// STEP 2 - SCENE RENDERING FROM CAMERA ////////////////////////////////////////////////

        // we activate back the standard Frame Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // we "clear" the frame and z buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // we set the rendering mode
        if (wireframe)
            // Draw in wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // we set the viewport for the final rendering step
        glViewport(0, 0, width, height);

        // We "install" the selected Shader Program as part of the current rendering process. We pass to the shader the light transformation matrix, and the depth map rendered in the first rendering step
        illumination_shader->Use();

        // we pass projection and view matrices to the Shader Program
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader->Program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader->Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // we determine the position in the Shader Program of the uniform variables
        GLint lightDirLocation = glGetUniformLocation(illumination_shader->Program, "lightVector");

        // we assign the value to the uniform variables
        glUniform3fv(lightDirLocation, 1, glm::value_ptr(lightDir));

        // we render the scene
        size = renderableObjects.size();
        for (size_t i = 0; i < size; i++)
        {
            (renderableObjects)[i].Draw(
                *illumination_shader,
                textures,
                models,
                materials,
                cumulatedTransforms,
                view,
                (renderableObjects)[i].RENDER,
                depthMap
            );
        }

    }

    void Renderer::DrawDebug(
        const glm::mat4& view,
        const glm::mat4& projection,
        Shader* const debugShader,
        std::vector<Line>& drawableLines,
        const int width,
        const int height
    ) const
    {
        debugShader->Use();

        GLint projectionMatrixLocation = glGetUniformLocation(debugShader->Program, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
        GLint viewMatrixLocation = glGetUniformLocation(debugShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));

        for (size_t i = 0; i < drawableLines.size(); i++)
        {
            GLint modelMatrixLocation = glGetUniformLocation(debugShader->Program, "modelMatrix");
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(drawableLines[i].m_ModelMatrix));
            GLint colorLocation = glGetUniformLocation(debugShader->Program, "colorIn");
            glUniform3fv(colorLocation, 1, glm::value_ptr(drawableLines[i].m_LineColor));

            // VAO is made "active"
            glBindVertexArray(drawableLines[i].GetVAO());
            // rendering of data in the VAO
            glDrawArrays(GL_LINES, 0, 2);   
            // VAO is "detached"
            glBindVertexArray(0);
        }
    }

    Renderer::~Renderer()
    {
    }
}