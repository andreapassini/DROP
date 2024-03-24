#pragma once
#include "renderableObject.h"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/shader.h"
#include "../utils/camera.h"

class Renderer
{
public:
	Renderer();
    ~Renderer() {};

	void RenderScene(
        std::vector<RenderableObject>* const renderableObjects,
        glm::mat4 view,
        glm::mat4 projection,
        glm::vec3 lightDir,
        Shader* const shadow_shader,
        Shader* const illumination_shader,
        const GLfloat Kd,
        const GLfloat alpha,
        const GLfloat F0,
        Camera* const camera,
        const GLuint depthMapFBO,
        const GLuint depthMap,
        const GLboolean wireframe,
        // index of the current shader subroutine (= 0 in the beginning)
        const GLuint current_subroutine,
        // a vector for all the shader subroutines names used and swapped in the application
        std::vector<std::string>* const shaders,
        const int width,
        const int height
    );

private:
	const GLuint SHADOW_WIDTH;
	const GLuint SHADOW_HEIGHT;
};
