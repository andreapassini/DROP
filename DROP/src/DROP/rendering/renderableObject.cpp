#include "renderableObject.h"

#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../rendering/shader.h"
#include "DROP/math/sceneGraph.h"


void RenderableObject::Draw(
    const Shader& shader, 
    const std::vector<int>& textuers,
    const std::vector<Model>& models,
    const std::vector<Material>& materials,
    std::unordered_map<uint32_t, VgMath::Transform> m_CumulatedTransforms,

    const glm::mat4& view,
    const GLint render_pass, 
    const GLuint depthMap) const
{
    switch (render_pass)
    {
    default:
    case render_passes::RENDER:
        {   // without this scope there is an error for the variable "**location" not used
            GLint kdLocation = glGetUniformLocation(shader.Program, "Kd");
            GLint alphaLocation = glGetUniformLocation(shader.Program, "alpha");
            GLint f0Location = glGetUniformLocation(shader.Program, "F0");

            glUniform1f(kdLocation, materials[m_MaterialId].Kd);
            glUniform1f(alphaLocation, materials[m_MaterialId].Alpha);
            glUniform1f(f0Location, materials[m_MaterialId].F0);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            GLint shadowLocation = glGetUniformLocation(shader.Program, "shadowMap");
            glUniform1i(shadowLocation, 2);
        }
        break;
    case render_passes::SHADOWMAP:
        if (materials[m_MaterialId].CastShadow == false)
            return;

        break;
    }

    // we pass the needed uniforms
    GLint textureLocation = glGetUniformLocation(shader.Program, "tex");
    GLint repeatLocation = glGetUniformLocation(shader.Program, "repeat");


	if (materials[m_MaterialId].UseTexture) {
        // we activate the texture of the plane
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, (textuers).at(materials[m_MaterialId].TextureId));
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, materials[m_MaterialId].Repeat);
    }

    VgMath::Transform& transform = m_CumulatedTransforms.find(m_ModelMatrixId)->second;
    glm::mat4 modelMatrix(1.0f);
    SceneGraph::TransformToMatrix(transform, modelMatrix);

    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(view * modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    // we render the plane
    models[m_ModelId].Draw();
}
