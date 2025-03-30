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

#include "DROP/rendering/shader.h"
#include "DROP/utils/camera.h"

#include "colors.h"
#include "DROP/sceneGraph/sceneGraph.h"
#include "DROP/utils/Log.h"

#define UNLOCK_FRAMERTE

#ifdef DROP_DEBUG
// https://learnopengl.com/In-Practice/Debugging
// glGetError() provide only one flag at the time
//  so we need to pump it until empty
GLenum glCheckErrorExt(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckErrorExt(__FILE__, __LINE__) 
#else
#define glCheckError()
#endif // #ifdef DROP_DEBUG

namespace Drop
{
    void Renderer::Init(GLFWwindow* window, RendererContext& rendererContext) {
        // We are in Drop/Drop!!!
        // 
        rendererContext.window = window;

        // we enable Z test
        glEnable(GL_DEPTH_TEST);

        //the "clear" color for the frame buffer
        glClearColor(DEFAULT_CLEAR_COLOR.r, DEFAULT_CLEAR_COLOR.g, DEFAULT_CLEAR_COLOR.b, 1.0f);

        /////////////////// CREATION OF BUFFER FOR THE  DEPTH MAP /////////////////////////////////////////
        // buffer dimension: too large -> performance may slow down if we have many lights; too small -> strong aliasing
        //GLuint depthMapFBO;
        // we create a Frame Buffer Object: the first rendering step will render to this buffer, and not to the real frame buffer
        glGenFramebuffers(1, &rendererContext.depthMapFBO);
        // we create a texture for the depth map
        //GLuint depthMap;
        glGenTextures(1, &rendererContext.depthMap);
        glBindTexture(GL_TEXTURE_2D, rendererContext.depthMap);
        // in the texture, we will save only the depth data of the fragments. Thus, we specify that we need to render only depth in the first rendering step
        glTexImage2D(
            GL_TEXTURE_2D
            , 0
            , GL_DEPTH_COMPONENT
            , rendererContext.SHADOW_WIDTH
            , rendererContext.SHADOW_HEIGHT
            , 0
            , GL_DEPTH_COMPONENT
            , GL_FLOAT
            , NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // we set to clamp the uv coordinates outside [0,1] to the color of the border
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // outside the area covered by the light frustum, everything is rendered in shadow (because we set GL_CLAMP_TO_BORDER)
        //*************
        GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // we bind the depth map FBO
        glBindFramebuffer(GL_FRAMEBUFFER, rendererContext.depthMapFBO);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER
            , GL_DEPTH_ATTACHMENT
            , GL_TEXTURE_2D
            , rendererContext.depthMap
            , 0);
        // we set that we are not calculating nor saving color data
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ///////////////////////////////////////////////////////////////////
    }

    void Renderer::RenderScene(
        const SceneContext& sceneContext
        , const RendererContext& rendererContext
        , const std::vector<RenderableObject>& renderableObjects
        , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
        , Shader* const shadow_shader
        , Shader* const illumination_shader
    ) {
        /////////////////// STEP 1 - SHADOW MAP: RENDERING OF SCENE FROM LIGHT POINT OF VIEW ////////////////////////////////////////////////
        // we set view and projection matrix for the rendering using light as a camera
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = -10.0f, far_plane = 10.0f;
        GLfloat frustumSize = 5.0f;
        // for a directional light, the projection is orthographic. For point lights, we should use a perspective projection
        lightProjection = glm::ortho(-frustumSize, frustumSize, -frustumSize, frustumSize, near_plane, far_plane);
        // the light is directional, so technically it has no position. We need a view matrix, so we consider a position on the the direction vector of the light
        lightView = glm::lookAt(sceneContext.lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // transformation matrix for the light
        lightSpaceMatrix = lightProjection * lightView;
        /// We "install" the  Shader Program for the shadow mapping creation
        shadow_shader->Use();
        // we pass the transformation matrix as uniform
        glUniformMatrix4fv(
            glGetUniformLocation(
                shadow_shader->Program
                , "lightSpaceMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(lightSpaceMatrix)
        );
        // we set the viewport for the first rendering step = dimensions of the depth texture
        glViewport(0, 0, rendererContext.SHADOW_WIDTH, rendererContext.SHADOW_HEIGHT);
        // we activate the FBO for the depth map rendering
        glBindFramebuffer(GL_FRAMEBUFFER, rendererContext.depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // we render the scene, using the shadow shader
        size_t size = renderableObjects.size();
        for (size_t i = 0; i < size; i++)
        {
            renderableObjects[i].Draw(
                *shadow_shader,
                sceneContext.textures,
                sceneContext.models,
                sceneContext.materials,
                cumulatedTransforms,
                sceneContext.view,
                (renderableObjects)[i].SHADOWMAP,
                rendererContext.depthMap
            );
        }

        /////////////////// STEP 2 - SCENE RENDERING FROM CAMERA ////////////////////////////////////////////////

        // we activate back the standard Frame Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // we "clear" the frame and z buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // we set the rendering mode
        if (sceneContext.wireframe)
            // Draw in wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // we set the viewport for the final rendering step
        glViewport(0, 0, sceneContext.width, sceneContext.height);

        // We "install" the selected Shader Program as part of the current rendering process. We pass to the shader the light transformation matrix, and the depth map rendered in the first rendering step
        illumination_shader->Use();

        // we pass projection and view matrices to the Shader Program
        glUniformMatrix4fv(
            glGetUniformLocation(
                illumination_shader->Program
                , "projectionMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(sceneContext.projection)
        );
        glUniformMatrix4fv(
            glGetUniformLocation(
                illumination_shader->Program
                , "viewMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(sceneContext.view)
        );
        glUniformMatrix4fv(
            glGetUniformLocation(
                illumination_shader->Program
                , "lightSpaceMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(lightSpaceMatrix)
        );

        // we determine the position in the Shader Program of the uniform variables
        GLint lightDirLocation = glGetUniformLocation(illumination_shader->Program, "lightVector");

        // we assign the value to the uniform variables
        glUniform3fv(lightDirLocation, 1, glm::value_ptr(sceneContext.lightDir));

        // we render the scene
        size = renderableObjects.size();
        for (size_t i = 0; i < size; i++)
        {
            (renderableObjects)[i].Draw(
                *illumination_shader,
                sceneContext.textures,
                sceneContext.models,
                sceneContext.materials,
                cumulatedTransforms,
                sceneContext.view,
                (renderableObjects)[i].RENDER,
                rendererContext.depthMap
            );
        }

    }

    void Renderer::RenderScene(
        const SceneContext& sceneContext
        , const std::vector<RenderableObject>& renderableObjects
        , const std::unordered_map<uint32_t, VgMath::Transform>& cumulatedTransforms
        , Shader* const illumination_shader
    ) {
        // we activate back the standard Frame Buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //// we "clear" the frame and z buffer
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // we set the rendering mode
        if (sceneContext.wireframe)
            // Draw in wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // we set the viewport for the final rendering step
        glViewport(0, 0, sceneContext.width, sceneContext.height);

        // We "install" the selected Shader Program as part of the current rendering process. 
        // We pass to the shader the light transformation matrix, 
        // and the depth map rendered in the first rendering step
        illumination_shader->Use();

        // we pass projection and view matrices to the Shader Program
        glUniformMatrix4fv(
            glGetUniformLocation(
                illumination_shader->Program
                , "projectionMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(sceneContext.projection)
        );
        glUniformMatrix4fv(
            glGetUniformLocation(
                illumination_shader->Program
                , "viewMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(sceneContext.view)
        );

        // we render the scene
        size_t size = renderableObjects.size();
        for (size_t i = 0; i < size; i++)
        {
            (renderableObjects)[i].Draw(
                *illumination_shader,
                sceneContext.textures,
                sceneContext.models,
                sceneContext.materials,
                cumulatedTransforms,
                sceneContext.view
            );
        }
    }

    void Renderer::RenderParticles(
        const SceneContext& sceneContext
        , std::vector<ParticleEmitter>& particleEmitters
        , Shader* const billboardShader
    ) {
        // Clear errors
        glCheckError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // we set the viewport for the final rendering step
        glViewport(0, 0, sceneContext.width, sceneContext.height);
        billboardShader->Use();

        glCheckError();

        // we pass projection and view matrices to the Shader Program
        GLint projectionMatrixLocation = glGetUniformLocation(billboardShader->Program, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.projection));
        glCheckError();

        GLint viewMatrixLocation = glGetUniformLocation(billboardShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.view));
        glCheckError();

        //// VAO is made "active"
        //glBindVertexArray(m_billboardVAO);
        //glCheckError();

        for (ParticleEmitter& particleEmitter : particleEmitters) {
            uint32_t activeParticle = 0;
            for (uint32_t i = 0; i < particleEmitter.numberOfParticles; i++)
            {
                const Particle& particle = particleEmitter.particles[i];

                if (!particle.isActive) {
                    continue;
                }
                activeParticle++;

                //GLint positionLocation = glGetUniformLocation(billboardShader->Program, "position");
                //glCheckError();
                //glUniform3fv(positionLocation, 1, glm::value_ptr(
                //    glm::vec3(particle.position.x
                //        , particle.position.y
                //        , particle.position.z
                //    )
                //));
                //glCheckError();

                glm::mat4 modelMatrix(1.0f);
                VgMath::Transform testTransform;
                testTransform.m_Translate = particle.position.asVector3();
                SceneGraph::TransformToMatrix(testTransform, modelMatrix);
                GLint modelMatrixLocation = glGetUniformLocation(billboardShader->Program, "modelMatrix");
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glCheckError();

                GLint particleSizeLocation = glGetUniformLocation(billboardShader->Program, "particle_size");
                glCheckError();
                glUniform1f(particleSizeLocation, particle.size);
                glCheckError();

                GLint colorLocation = glGetUniformLocation(billboardShader->Program, "colorIn");
                glUniform4fv(colorLocation, 1, glm::value_ptr(
                    glm::vec4( 
                        particle.color.r
                        , particle.color.g
                        , particle.color.b
                        , particle.colorAlpha
                    )
                    //glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
                ));
                glCheckError();

                // we activate the texture of the plane
                GLint textureLocation = glGetUniformLocation(billboardShader->Program, "tex0");
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, sceneContext.textures[particle.textureID]);
                glUniform1i(textureLocation, 0);
                glCheckError();

                // rendering data
                glDrawArrays(GL_POINTS, 0, 1);
                glCheckError();
            }
        }

        // VAO is "detached"
        glBindVertexArray(0);
        glCheckError();
    }

    void Renderer::RenderBillboard(
        const SceneContext& sceneContext
        , const std::vector<Billboard>& billboards
        , Shader* const billboardShader
    ) {
        // Clear errors
        glCheckError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // we set the viewport for the final rendering step
        glViewport(0, 0, sceneContext.width, sceneContext.height);
        billboardShader->Use();

        glCheckError();

        // we pass projection and view matrices to the Shader Program
        GLint projectionMatrixLocation = glGetUniformLocation(billboardShader->Program, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.projection));
        glCheckError();

        GLint viewMatrixLocation = glGetUniformLocation(billboardShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.view));
        glCheckError();

        //// VAO is made "active"
        //glBindVertexArray(m_billboardVAO);
        //glCheckError();

        for (uint32_t i = 0; i < billboards.size(); i++)
        {
            const Billboard& billboard = billboards[i];

            //GLint positionLocation = glGetUniformLocation(billboardShader->Program, "position");
            //glCheckError();
            //glUniform3fv(positionLocation, 1, glm::value_ptr(
            //    glm::vec3(billboard.position.x
            //        , billboard.position.y
            //        , billboard.position.z
            //    )
            //));
            //glCheckError();

            glm::mat4 modelMatrix(1.0f);
            billboard.transform->m_Translate = billboard.transform->m_Translate * billboard.postion;
            SceneGraph::TransformToMatrix(*billboard.transform, modelMatrix);
            GLint modelMatrixLocation = glGetUniformLocation(billboardShader->Program, "modelMatrix");
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glCheckError();

            GLint particleSizeLocation = glGetUniformLocation(billboardShader->Program, "particle_size");
            glCheckError();
            glUniform1f(particleSizeLocation, billboard.scale);
            glCheckError();

            //GLint colorLocation = glGetUniformLocation(billboardShader->Program, "colorIn");
            //glUniform4fv(colorLocation, 1, glm::value_ptr(
            //    glm::vec4(
            //        billboard.color.r
            //        , billboard.color.g
            //        , billboard.color.b
            //        , billboard.colorAlpha
            //    )
            //    //glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
            //));
            //glCheckError();

            // we activate the texture of the plane
            GLint textureLocation = glGetUniformLocation(billboardShader->Program, "tex0");
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sceneContext.textures[billboard.textureID]);
            glUniform1i(textureLocation, 0);
            glCheckError();

            // rendering data
            glDrawArrays(GL_POINTS, 0, 1);
            glCheckError();
        }

        // VAO is "detached"
        glBindVertexArray(0);
        glCheckError();
    }

    void Renderer::DrawDebug(
        const SceneContext& sceneContext
        , Shader* const debugShader
        , std::vector<Line>& drawableLines
    ) {
        debugShader->Use();

        GLint projectionMatrixLocation = glGetUniformLocation(debugShader->Program, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.projection));
        GLint viewMatrixLocation = glGetUniformLocation(debugShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.view));

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

    void Renderer::DrawParticleEmitterSurface(
        const SceneContext& sceneContext
        , Shader* const emptyQuadGeomShader
        , std::vector<ParticleEmitter>& drawableParticleEmitter
    ) {
        // Clear errors
        glCheckError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // we set the viewport for the final rendering step
        glViewport(0, 0, sceneContext.width, sceneContext.height);
        emptyQuadGeomShader->Use();

        glCheckError();

        // we pass projection and view matrices to the Shader Program
        GLint projectionMatrixLocation = glGetUniformLocation(emptyQuadGeomShader->Program, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.projection));
        glCheckError();

        GLint viewMatrixLocation = glGetUniformLocation(emptyQuadGeomShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.view));
        glCheckError();

        //// VAO is made "active"
        //glBindVertexArray(m_billboardVAO);
        //glCheckError();

        for (uint32_t i = 0; i < drawableParticleEmitter.size(); i++)
        {
            const ParticleEmitter& particleEmitter = drawableParticleEmitter[i];

            //GLint positionLocation = glGetUniformLocation(emptyQuadGeomShader->Program, "position");
            //glCheckError();
            //glUniform3fv(positionLocation, 1, glm::value_ptr(
            //    glm::vec3(particleEmitter.position.x
            //        , particleEmitter.position.y
            //        , particleEmitter.position.z
            //    )
            //));
            //glCheckError();

            glm::mat4 modelMatrix(1.0f);
            const VgMath::Transform& testTransform = *(particleEmitter.spawningValues.spawningSurface.m_Transform);
            SceneGraph::TransformToMatrix(testTransform, modelMatrix);
            GLint modelMatrixLocation = glGetUniformLocation(emptyQuadGeomShader->Program, "modelMatrix");
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glCheckError();

            GLint particleSizeLocation = glGetUniformLocation(emptyQuadGeomShader->Program, "size");
            glCheckError();
            glUniform2fv(
                particleSizeLocation
                , 1
                , glm::value_ptr(
                    glm::vec2(
                        particleEmitter.spawningValues.spawningSurface.m_Size.x
                        , particleEmitter.spawningValues.spawningSurface.m_Size.y
                    )
                )
            );
            glCheckError();

            GLint colorLocation = glGetUniformLocation(emptyQuadGeomShader->Program, "colorIn");
            glUniform4fv(colorLocation, 1, glm::value_ptr(
                glm::vec4(
                    DEFAULT_LINE_COLOR.r
                    , DEFAULT_LINE_COLOR.g
                    , DEFAULT_LINE_COLOR.b
                    , 1.0f
                )
            ));
            glCheckError();

            // rendering data
            glDrawArrays(GL_POINTS, 0, 1);
            glCheckError();
        }

        // VAO is "detached"
        glBindVertexArray(0);
        glCheckError();
    }

    void Renderer::Shutdown() {}

    void Renderer::Draw_ShadowPass(
        const StaticMeshComponent& staticMeshComponent
        , const VgMath::Transform& worldTransform
        , const SceneContext& sceneContext
        , const RendererContext& rendererContext
    ) {
        std::vector<Shader>& shaders = rendererContext.shaders;
        std::vector<Material>& materials = sceneContext.materials;
        std::vector<Model>& models = sceneContext.models;
        std::vector<TextureID>& textuers = sceneContext.textuers;
    }
    void Renderer::Draw_IlluminationPass(const StaticMeshComponent& staticMeshComponent, const VgMath::Transform& worldTransform, const SceneContext& sceneContext, const RendererContext& rendererContext)
    {

    }
}