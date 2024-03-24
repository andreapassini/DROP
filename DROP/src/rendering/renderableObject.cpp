#include "renderableObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/shader.h"

void RenderableObject::Draw(
    const Shader& shader, 
    const glm::mat4& view,
    const GLint render_pass, 
    const GLuint depthMap)
{
    // For the second rendering step -> we pass the shadow map to the shaders
    if (render_pass == RENDER)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        GLint shadowLocation = glGetUniformLocation(shader.Program, "shadowMap");
        glUniform1i(shadowLocation, 2);
    }
    // we pass the needed uniforms
    GLint textureLocation = glGetUniformLocation(shader.Program, "tex");
    GLint repeatLocation = glGetUniformLocation(shader.Program, "repeat");


	if (this->textureParameter.useTexture) {
        // we activate the texture of the plane
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, (this->textureParameter.textures)->at(this->textureParameter.textureId));
        glUniform1i(textureLocation, 0);
        glUniform1f(repeatLocation, this->textureParameter.repeat);
    }

    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(view * *this->modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(*this->modelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    // we render the plane
    this->model->Draw();

}
