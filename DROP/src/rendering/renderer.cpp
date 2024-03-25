#include "renderer.h"

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

Renderer::Renderer(
    GLuint screenWidth_val,
    GLuint screenHeight_val,
    void (*key_callback)(GLFWwindow*, int, int, int, int),
    void (*mouse_callback)(GLFWwindow*, double, double)
):
    width(screenWidth_val),
    height(screenHeight_val),
    SHADOW_WIDTH(1024),
    SHADOW_HEIGHT(1024)
{
    // We are in DROP/DROP!!!

    // Initialization of OpenGL context using GLFW
    glfwInit();
    // We set OpenGL specifications required for this application
    // In this case: 4.1 Core
    // If not supported by your graphics HW, the context will not be created and the application will close
    // N.B.) creating GLAD code to load extensions, try to take into account the specifications and any extensions you want to use,
    // in relation also to the values indicated in these GLFW commands
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // we set if the window is resizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // we create the application's window
    window = glfwCreateWindow(width, height, "DROP", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        assert(false);
        //return -1;
        return;
    }
    glfwMakeContextCurrent(window);

    // we put in relation the window and the callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // we disable the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD tries to load the context set by GLFW
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        assert(false);
        //return -1;
        return;
    }

    // we define the viewport dimensions
    //int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // we enable Z test
    glEnable(GL_DEPTH_TEST);

    //the "clear" color for the frame buffer
    glClearColor(0.26f, 0.46f, 0.98f, 1.0f);

    /////////////////// CREATION OF BUFFER FOR THE  DEPTH MAP /////////////////////////////////////////
    // buffer dimension: too large -> performance may slow down if we have many lights; too small -> strong aliasing
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    //GLuint depthMapFBO;
    // we create a Frame Buffer Object: the first rendering step will render to this buffer, and not to the real frame buffer
    glGenFramebuffers(1, &depthMapFBO);
    // we create a texture for the depth map
    //GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
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
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // we set that we are not calculating nor saving color data
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ///////////////////////////////////////////////////////////////////
}

void Renderer::RenderScene(
    std::vector<RenderableObject>* const renderableObjects, 
    glm::mat4 view, 
    glm::mat4 projection,
    glm::vec3 lightDir ,
    Shader * const shadow_shader,
    Shader * const illumination_shader,
    Camera * const camera,
    const GLuint depthMapFBO,
    const GLuint depthMap,
    const GLboolean wireframe,
    // index of the current shader subroutine (= 0 in the beginning)
    const GLuint current_subroutine,
    // a vector for all the shader subroutines names used and swapped in the application
    std::vector<std::string>* const shaders,
    const int width,
    const int height
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
    //RenderObjects(shadow_shader, planeModel, cubeModel, sphereModel, bunnyModel, SHADOWMAP, depthMap);
    size_t size = renderableObjects->size();
    for (size_t i = 0; i < size; i++) {
        (*renderableObjects)[i].Draw(
            *shadow_shader,
            view,
            (*renderableObjects)[i].SHADOWMAP,
            depthMap
            );
    }

    /////////////////// STEP 2 - SCENE RENDERING FROM CAMERA ////////////////////////////////////////////////

    // we get the view matrix from the Camera class
    view = camera->GetViewMatrix();

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
    // we search inside the Shader Program the name of the subroutine currently selected, and we get the numerical index
    GLuint index = glGetSubroutineIndex(illumination_shader->Program, GL_FRAGMENT_SHADER, (*shaders)[current_subroutine].c_str());
    // we activate the subroutine using the index (this is where shaders swapping happens)
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index);

    // we pass projection and view matrices to the Shader Program
    glUniformMatrix4fv(glGetUniformLocation(illumination_shader->Program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(illumination_shader->Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(illumination_shader->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // we determine the position in the Shader Program of the uniform variables
    GLint lightDirLocation = glGetUniformLocation(illumination_shader->Program, "lightVector");
    // Set this for each rebnderableObject material
    //GLint kdLocation = glGetUniformLocation(illumination_shader->Program, "Kd");
    //GLint alphaLocation = glGetUniformLocation(illumination_shader->Program, "alpha");
    //GLint f0Location = glGetUniformLocation(illumination_shader->Program, "F0");

    // we assign the value to the uniform variables
    glUniform3fv(lightDirLocation, 1, glm::value_ptr(lightDir));
    // Set this for each rebnderableObject material
    //glUniform1f(kdLocation, Kd);  // Moving it to the single drawing calls
    //glUniform1f(alphaLocation, alpha);
    //glUniform1f(f0Location, F0);

    // we render the scene
    //RenderObjects(illumination_shader, planeModel, cubeModel, 
    // sphereModel, bunnyModel, RENDER, depthMap);
    size = renderableObjects->size();
    for (size_t i = 0; i < size; i++) {
        (*renderableObjects)[i].Draw(
            *illumination_shader,
            view,
            (*renderableObjects)[i].RENDER,
            depthMap
        );
    }

}


Renderer::~Renderer() {
};
