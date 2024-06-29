#pragma once
#include "renderableObject.h"

#include <vector>

// Loader for OpenGL extensions
// http://glad.dav1d.de/
// THIS IS OPTIONAL AND NOT REQUIRED, ONLY USE THIS IF YOU DON'T WANT GLAD TO INCLUDE windows.h
// GLAD will include windows.h for APIENTRY if it was not previously defined.
// Make sure you have the correct definition for APIENTRY for platforms which define _WIN32 but don't use __stdcall
#ifdef _WIN32
#define APIENTRY __stdcall
#endif

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

#include "../utils/shader.h"
#include "../utils/camera.h"

class Renderer
{
public:
	Renderer(
        uint32_t screenWidth_val,
        uint32_t screenHeight_val
    );
    ~Renderer();
	void RenderScene(
        std::vector<RenderableObject>* const renderableObjects,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& lightDir,
        Shader* const shadow_shader,
        Shader* const illumination_shader,
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
public:
    GLFWwindow* m_Window;
    GLuint m_DepthMapFBO;
    GLuint m_DepthMap;
    int m_Width;
    int m_Height;


private:
	const GLuint SHADOW_WIDTH;
	const GLuint SHADOW_HEIGHT;
};
