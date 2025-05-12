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
        // in the texture, we will save only the depth data of the fragments. 
        // Thus, we specify that we need to render only depth in the first rendering step
        glTexImage2D(
            GL_TEXTURE_2D
            , 0
            , GL_DEPTH_COMPONENT
            , rendererContext.SHADOW_WIDTH
            , rendererContext.SHADOW_HEIGHT
            , 0
            , GL_DEPTH_COMPONENT
            , GL_FLOAT
            , NULL
        );
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

    void Renderer::RenderParticles(
        SceneContext& sceneContext
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
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetProjectionMatrix()));
        glCheckError();

        GLint viewMatrixLocation = glGetUniformLocation(billboardShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetViewMatrix()));
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
                glBindTexture(GL_TEXTURE_2D, (*sceneContext.textuers)[particle.textureID]);
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
        SceneContext& sceneContext
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
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetProjectionMatrix()));
        glCheckError();

        GLint viewMatrixLocation = glGetUniformLocation(billboardShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetViewMatrix()));
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
            glBindTexture(GL_TEXTURE_2D, (*sceneContext.textuers)[billboard.textureID]);
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
        SceneContext& sceneContext
        , Shader* const debugShader
        , std::vector<Line>& drawableLines
    ) {
        debugShader->Use();

        GLint projectionMatrixLocation = glGetUniformLocation(debugShader->Program, "projectionMatrix");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetProjectionMatrix()));
        GLint viewMatrixLocation = glGetUniformLocation(debugShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetViewMatrix()));

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
        SceneContext& sceneContext
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
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetProjectionMatrix()));
        glCheckError();

        GLint viewMatrixLocation = glGetUniformLocation(emptyQuadGeomShader->Program, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneContext.camera->GetViewMatrix()));
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

    void Renderer::CalculateLightMatrix(
        SceneContext& sceneContext
    ) {
        glm::mat4 lightProjection, lightView;
        //glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = -10.0f, far_plane = 10.0f;
        GLfloat frustumSize = 5.0f;
        // for a directional light, the projection is orthographic. For point lights, we should use a perspective projection
        lightProjection = glm::ortho(-frustumSize, frustumSize, -frustumSize, frustumSize, near_plane, far_plane);
        // the light is directional, so technically it has no position. We need a view matrix, so we consider a position on the the direction vector of the light
        lightView = glm::lookAt(sceneContext.lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // transformation matrix for the light
        sceneContext.lightSpaceMatrix = lightProjection * lightView;
    }
    void Renderer::SetupShadowPass(
        SceneContext& sceneContext
        , RendererContext& rendererContext
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
        sceneContext.lightSpaceMatrix = lightSpaceMatrix;

        // We "install" the  Shader Program for the shadow mapping creation
        std::vector<Shader>& shaders = rendererContext.shaders;
        assert(shaders.size() > SHADOW_SHADER);

        Shader& shadow_shader = rendererContext.shaders[SHADOW_SHADER];
        shadow_shader.Use();
        // we pass the transformation matrix as uniform
        glUniformMatrix4fv(
            glGetUniformLocation(
                shadow_shader.Program
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
    }

    void Renderer::DrawMeshForShadow(
        const StaticMeshComponent& meshComponent
        , VgMath::Transform& worldTransform
        , SceneContext& sceneContext
        , RendererContext& rendererContext
    ) {
        // Clear prev error
        glCheckError();

        std::vector<Material>& materials = *sceneContext.materials;
        assert(materials.size() > meshComponent.materialId);
        Material& material = materials[meshComponent.materialId];

        std::vector<Shader>& shaders = rendererContext.shaders;
        assert(shaders.size() > SHADOW_SHADER);
        Shader& shader = shaders[SHADOW_SHADER];
        
        std::vector<Model>& models = *sceneContext.models;
        assert(models.size() > meshComponent.modelId);
        Model& model = models[meshComponent.modelId];

        std::vector<TextureID>& textuers = *sceneContext.textuers;

        for (uint32_t i = 0; i < MAX_USER_TEXTURES; i++)
        {
            TextureSpecification& currTexture = material.textures[i];
            std::string textureName = "";
            std::string repeatName = "";
            if (currTexture.textureId != TEXTURE_NOT_USED)
            {
                // we dont want to override systems textures (from index 20 to 31)
                assert(currTexture.textureId <= MAX_USER_TEXTURES - 1);

                // we pass the needed uniforms
                textureName = "tex_" + std::to_string(i);
                GLint textureLocation = glGetUniformLocation(shader.Program, textureName.c_str());
                glCheckError();

                repeatName = "repeat_" + std::to_string(i);
                GLint repeatLocation = glGetUniformLocation(shader.Program, repeatName.c_str());
                glCheckError();

                // we activate the texture of the plane
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, (textuers).at(currTexture.textureId));
                glUniform1i(textureLocation, i);
                glUniform1f(repeatLocation, currTexture.UVRepeat);
            }
        }

        VgMath::Transform& transform = worldTransform;
        glm::mat4 modelMatrix(1.0f);
        SceneGraph::TransformToMatrix(transform, modelMatrix);

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(sceneContext.camera->GetViewMatrix() * modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glCheckError();

        glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glCheckError();

        model.Draw();
    }

    void Renderer::SetupColorPass(
        SceneContext& sceneContext
        , RendererContext& rendererContext
    ) {
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
    }

    void Renderer::DrawMesh(
        const StaticMeshComponent& meshComponent
        , VgMath::Transform& worldTransform
        , SceneContext& sceneContext
        , RendererContext& rendererContext
    ) {
        std::vector<Model>& models = *sceneContext.models;
        assert(models.size() > meshComponent.modelId);
        Model& model = models[meshComponent.modelId];

        std::vector<Material>& materials = *sceneContext.materials;
        assert(materials.size() > meshComponent.materialId);
        Material& material = materials[meshComponent.materialId];

        std::vector<Shader>& shaders = rendererContext.shaders;
        assert(shaders.size() > meshComponent.materialId);
        Shader& shader = shaders[material.shaderID];

        // We "install" the selected Shader Program as part of the current rendering process. We pass to the shader the light transformation matrix, and the depth map rendered in the first rendering step
        shader.Use();

        // we pass projection and view matrices to the Shader Program
        glUniformMatrix4fv(
            glGetUniformLocation(
                shader.Program
                , "projectionMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(sceneContext.camera->GetProjectionMatrix())
        );
        glUniformMatrix4fv(
            glGetUniformLocation(
                shader.Program
                , "viewMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(sceneContext.camera->GetViewMatrix())
        );
        glUniformMatrix4fv(
            glGetUniformLocation(
                shader.Program
                , "lightSpaceMatrix"
            )
            , 1
            , GL_FALSE
            , glm::value_ptr(sceneContext.lightSpaceMatrix)
        );

        // we determine the position in the Shader Program of the uniform variables
        GLint lightDirLocation = glGetUniformLocation(shader.Program, "lightVector");

        // we assign the value to the uniform variables
        glUniform3fv(lightDirLocation, 1, glm::value_ptr(sceneContext.lightDir));
        glCheckError();

        std::vector<TextureID>& textuers = *sceneContext.textuers;

        {   // without this scope there is an error for the variable "**location" not used
            GLint kdLocation = glGetUniformLocation(shader.Program, "Kd");
            GLint alphaLocation = glGetUniformLocation(shader.Program, "Alpha");
            GLint f0Location = glGetUniformLocation(shader.Program, "F0");

            glUniform1f(kdLocation, material.kd);
            glCheckError();

            glUniform1f(alphaLocation, material.alpha);
            glCheckError();

            glUniform1f(f0Location, material.f0);
            glCheckError();

            //glActiveTexture(GL_TEXTURE2); // old, now we set the system textures from 20th index
            glActiveTexture(SHADOW_MAP_ACTIVE);
            glCheckError();

            glBindTexture(GL_TEXTURE_2D, rendererContext.depthMap);
            glCheckError();

            GLint shadowLocation = glGetUniformLocation(shader.Program, "shadowMap");
            glCheckError();

            glUniform1i(shadowLocation, SHADOW_MAP_1i);
            glCheckError();
        }

        for (uint32_t  i = 0; i < MAX_USER_TEXTURES; i++)
        {
            TextureSpecification& currTexture = material.textures[i];
            std::string textureName = "";
            std::string repeatName = "";
            if (currTexture.textureId != TEXTURE_NOT_USED)
            {
                // we dont want to override systems textures (from index 20 to 31)
                assert(currTexture.textureId <= MAX_USER_TEXTURES - 1);

                // we pass the needed uniforms
                textureName = "tex_" + std::to_string(i);
                GLint textureLocation = glGetUniformLocation(shader.Program, textureName.c_str());
                glCheckError();

                repeatName = "repeat_" + std::to_string(i);
                GLint repeatLocation = glGetUniformLocation(shader.Program, repeatName.c_str());
                glCheckError();

                // we activate the texture of the plane

                glActiveTexture(GL_TEXTURE0 + i);
                glCheckError();

                glBindTexture(GL_TEXTURE_2D, (textuers).at(currTexture.textureId));
                glCheckError();

                glUniform1i(textureLocation, i);
                glCheckError();

                glUniform1f(repeatLocation, currTexture.UVRepeat);
                glCheckError();
            }
        }


        VgMath::Transform& transform = worldTransform;
        glm::mat4 modelMatrix(1.0f);
        SceneGraph::TransformToMatrix(transform, modelMatrix);

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(sceneContext.camera->GetViewMatrix() * modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glCheckError();

        glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glCheckError();

        model.Draw();
    }

//https://learnopengl.com/In-Practice/Debugging
//https://computergraphics.stackexchange.com/questions/23/how-can-i-debug-what-is-being-rendered-to-a-frame-buffer-object-in-opengl
//    // XXX WARNING: Untested code follows
//
//// Blit from fbo...
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
//    // ...to the front buffer.
//    glBindFramebuffer(GL_WRITE_FRAMEBUFFER, GL_FRONT);
//
//    GLsizei HalfWindowWidth = (GLsizei)(WindowWidth / 2.0f);
//    GLsizei HalfWindowHeight = (GLsizei)(WindowHeight / 2.0f);
//
//    // Blit attachment 0 to the lower-left quadrant of the window
//    glReadBuffer(GL_COLOR_ATTACHMENT0);
//    https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBlitFramebuffer.xhtml
//    glBlitFramebuffer(0, 0, FboWidth, FboHeight,
//        0, 0, HalfWindowWidth, HalfWindowHeight,
//        GL_COLOR_BUFFER_BIT, GL_LINEAR);
//
//    // Blit attachment 1 to the lower-right quadrant of the window
//    glReadBuffer(GL_COLOR_ATTACHMENT1);
//    glBlitFramebuffer(0, 0, FboWidth, FboHeight,
//        HalfWindowWidth, 0, WindowWidth, HalfWindowHeight,
//        GL_COLOR_BUFFER_BIT, GL_LINEAR);
//
//    // ...and so on. You can switch FBOs if you have more than one to read from.
}