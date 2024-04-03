/*
Es06a: shadow rendering with shadow mapping technique - PART 2
- swapping (pressing keys from 1 to 3) between basic shadow mapping (with a lot of aliasing/shadow "acne"), adaptive bias to avoid shadow "acne", and PCF to smooth shadow borders
- conclusion of Es05c, with object shaders now using the shadow map computed in the first rendering step

N.B. 1)
In this example we use Shaders Subroutines to do shader swapping:
http://www.geeks3d.com/20140701/opengl-4-shader-subroutines-introduction-3d-programming-tutorial/
https://www.lighthouse3d.com/tutorials/glsl-tutorial/subroutines/
https://www.khronos.org/opengl/wiki/Shader_Subroutine

In other cases, an alternative could be to consider Separate Shader Objects:
https://www.informit.com/articles/article.aspx?p=2731929&seqNum=7
https://www.khronos.org/opengl/wiki/Shader_Compilation#Separate_programs
https://riptutorial.com/opengl/example/26979/load-separable-shader-in-cplusplus

N.B. 2) the application considers only a directional light. In case of more lights, and/or of different nature, the code must be modifies

N.B. 3)
see :
https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/#basic-shadowmap
https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/common-techniques-to-improve-shadow-depth-maps
for further details


author: Davide Gadia

Real-Time Graphics Programming - a.a. 2022/2023
Master degree in Computer Science
Universita' degli Studi di Milano
*/

/*
OpenGL coordinate system (right-handed)
positive X axis points right
positive Y axis points up
positive Z axis points "outside" the screen


                              Y
                              |
                              |
                              |________X
                             /
                            /
                           /
                          Z
*/


// Std. Includes
#include <string>
#include <vector>
#include <iostream>

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

// we load the GLM classes used in the application
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>

// we include the library for images loading
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define stringify( name ) #name

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// classes developed during lab lectures to manage shaders, to load models, and for FPS camera
#include "utils/shader.h"
#include "utils/model.h"
#include "utils/camera.h"
#include "utils/performanceCalculator.h"
#include "rendering/renderer.h"
#include "rendering/renderableObject.h"

#include "math/sceneGraph.h"
#include "math/mat3.h"
#include "math/vector3.h"
#include "math/versor3.h"
#include "math/quaternion.h"

#define UP_DIRECTION glm::vec3(0.0f, 1.0f, 0.0f)
#define DOWN_DIRECTION glm::vec3(0.0f, -1.0f, 0.0f)

#define FORWARD_DIRECTION glm::vec3(0.0f, 0.0f, 1.0f)
#define BACKWARD_DIRECTION glm::vec3(0.0f, 0.0f, -1.0f)

#define RIGHT_DIRECTION glm::vec3(1.0f, 0.0f, 0.0f)
#define LEFT_DIRECTION glm::vec3(-1.0f, 0.0f, 0.0f)

// dimensions of application's window
GLuint screenWidth = 1200, screenHeight = 900;

// the rendering steps used in the application
enum render_passes { SHADOWMAP, RENDER };

// callback functions for keyboard and mouse events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// if one of the WASD keys is pressed, we call the corresponding method of the Camera class
void apply_camera_movements();

// index of the current shader subroutine (= 0 in the beginning)
GLuint current_subroutine = 0;
// a vector for all the shader subroutines names used and swapped in the application
std::vector<std::string> shaders;

// the name of the subroutines are searched in the shaders, and placed in the shaders vector (to allow shaders swapping)
void SetupShader(int shader_program);

// print on console the name of current shader subroutine
void PrintCurrentShader(int subroutine);

// load image from disk and create an OpenGL texture
GLint LoadTexture(const char* path);

void imGuiSetup(GLFWwindow* window);

// we initialize an array of booleans for each keyboard key
bool keys[1024];

// we need to store the previous mouse position to calculate the offset with the current frame
GLfloat lastX, lastY;
// when rendering the first frame, we do not have a "previous state" for the mouse, so we need to manage this situation
bool firstMouse = true;

// parameters for time calculation (for animations)
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastFramerateUpdate = 0.0f;
const GLfloat framerateUpdateTime = 0.5f;
uint32_t framerate;

// rotation angle on Y axis
VgMath::Scalar orientationY = 0.0f;
// rotation speed on Y axis
GLfloat spin_speed = 30.0f;
// boolean to start/stop animated rotation on Y angle
GLboolean spinning = GL_TRUE;

// boolean to activate/deactivate wireframe rendering
GLboolean wireframe = GL_FALSE;

// View matrix: the camera moves, so we just set to indentity now
glm::mat4 view = glm::mat4(1.0f);

// we create a camera. We pass the initial position as a parameter to the constructor. 
// The last boolean tells if we want a camera "anchored" to the ground
Camera camera(glm::vec3(0.0f, 0.0f, 7.0f), GL_FALSE,
    10.0f, 0.5f, 0.25f);

// in this example, we consider a directional light. We pass the direction of incoming light as an uniform to the shaders
glm::vec3 lightDir0 = glm::vec3(1.0f, 1.0f, 1.0f);

// vector for the textures IDs
std::vector<GLint> textureID;

//#define UNLOCK_FRAMERTE
//#define VISIBLE_MOUSE

/////////////////// MAIN function ///////////////////////
int main()
{
    // We are in DROP/DROP!!!

    Renderer renderer(
        screenWidth,
        screenHeight,
        key_callback,
        mouse_callback
    );

    // we create the Shader Program for the creation of the shadow map
    Shader shadow_shader("src\\shaders\\19_shadowmap.vert", "src\\shaders\\20_shadowmap.frag");
    // we create the Shader Program used for objects (which presents different subroutines we can switch)
    Shader illumination_shader = Shader("src\\shaders\\21_ggx_tex_shadow.vert", "src\\shaders\\22_ggx_tex_shadow.frag");

    // we parse the Shader Program to search for the number and names of the subroutines.
    // the names are placed in the shaders vector
    SetupShader(illumination_shader.Program);
    // we print on console the name of the first subroutine used
    PrintCurrentShader(current_subroutine);

    // we load the images and store them in a vector
    textureID.push_back(LoadTexture("../textures/UV_Grid_Sm.png"));
    textureID.push_back(LoadTexture("../textures/SoilCracked.png"));

    // we load the model(s) (code of Model class is in include/utils/model.h)
    Model cubeModel("../models/cube.obj");
    Model sphereModel("../models/sphere.obj");
    Model bunnyModel("../models/bunny_lp.obj");
    Model planeModel("../models/plane.obj");

    imGuiSetup(renderer.window);

    SceneGraph sceneGraph(150);

    sceneGraph.gameObjects[SceneGraph::ROOT_ID].localTransform.rotate =
        VgMath::Quaternion::angleAxis(VgMath::Degrees(0.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    // Projection matrix of the camera: FOV angle, aspect ratio, near and far planes
    glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 10'000.0f);

    std::vector<RenderableObject> rendereableObjects;

    uint32_t planeSGHandle = sceneGraph.AddObject(SceneGraph::ROOT_ID);
    sceneGraph.gameObjects[planeSGHandle].localTransform.translate = VgMath::Vector3(0.0f, -1.0f, 0.0f);
    sceneGraph.gameObjects[planeSGHandle].localTransform.scale = 10.0;
    sceneGraph.gameObjects[planeSGHandle].localTransform.rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(90.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter planeTextureParameter(
        true,
        1,
        &textureID,
        80.0);
    Material planeMaterial(
        3.0f,
        0.2f,
        0.9f,
        false
    );

    // Sphere
    uint32_t sphereSGHandle = sceneGraph.AddObject(SceneGraph::ROOT_ID);
    sceneGraph.gameObjects[sphereSGHandle].localTransform.translate = VgMath::Vector3(-3.0f, 1.0f, 0.0f);
    sceneGraph.gameObjects[sphereSGHandle].localTransform.scale = 1.0;
    sceneGraph.gameObjects[sphereSGHandle].localTransform.rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(90.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter sphereTextureParameter(
        true,
        0,
        &textureID );
    Material sphereMaterial(
        3.0f,
        0.2f,
        0.9f,
        true
    );

    // Cube
    uint32_t cubeSGHandle = sceneGraph.AddObject(sphereSGHandle);
    sceneGraph.gameObjects[cubeSGHandle].localTransform.translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
    sceneGraph.gameObjects[cubeSGHandle].localTransform.scale = 0.48;
    sceneGraph.gameObjects[cubeSGHandle].localTransform.rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(90.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter cubeTextureParameter(
        true,
        0,
        &textureID);
    Material cubeMaterial(
        3.0f,
        0.2f,
        0.9f,
        false
    );

    // Bunny
    uint32_t bunnySGHandle = sceneGraph.AddObject(SceneGraph::ROOT_ID);
    sceneGraph.gameObjects[bunnySGHandle].localTransform.translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
    sceneGraph.gameObjects[bunnySGHandle].localTransform.scale = 0.3;
    sceneGraph.gameObjects[bunnySGHandle].localTransform.rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(0.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter bunnyTextureParameter(
        true,
        0,
        &textureID);
    Material bunnyMaterial(
        3.0f,
        0.2f,
        0.9f,
        true
    );

    PerformanceCalculator framerateCalculator(100, 25);

    // Initialize cumulated transform hash map
    std::unordered_map<uint32_t, VgMath::Transform> cumulatedTransforms;
    cumulatedTransforms.reserve( sceneGraph.gameObjects.size());

    std::unordered_map<uint32_t, glm::mat4> modelMatrices;
    modelMatrices.reserve(sceneGraph.gameObjects.size());

    // Now the 2 maps have all the keys as the SceneGraph
    for (auto& it : sceneGraph.gameObjects) {
        cumulatedTransforms[it.first] = VgMath::Transform();
        modelMatrices[it.first] = glm::mat4(1.0f);
    }

    RenderableObject plane(
        &modelMatrices[planeSGHandle],
        &planeModel,
        &planeMaterial,
        planeTextureParameter);

    rendereableObjects.push_back(plane);

    RenderableObject sphere(
        &modelMatrices[sphereSGHandle],
        &sphereModel,
        &sphereMaterial,
        sphereTextureParameter);

    rendereableObjects.push_back(sphere);

    RenderableObject cube(
        &modelMatrices[cubeSGHandle],
        &cubeModel,
        &cubeMaterial,
        cubeTextureParameter);

    rendereableObjects.push_back(cube);

    RenderableObject bunny(
        &modelMatrices[bunnySGHandle],
        &bunnyModel,
        &bunnyMaterial,
        bunnyTextureParameter);

    rendereableObjects.push_back(bunny);

    // Rendering loop: this code is executed at each frame
    while (!glfwWindowShouldClose(renderer.window))
    {
        // we determine the time passed from the beginning
        // and we calculate time difference between current frame rendering and the previous one
        GLfloat currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        framerateCalculator.Step(deltaTime);
        if (currentTime - lastFramerateUpdate >= framerateUpdateTime) {
            framerate = (uint32_t)framerateCalculator.framerate;
            lastFramerateUpdate = currentTime;
        }

        // Check is an I/O event is happening
        glfwPollEvents();
        // we apply FPS camera movements
        apply_camera_movements();

        // render your GUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controls");

        ImGui::Text("DROP");
        ImGui::NewLine;
        ImGui::Text("FPS: %d", framerate);
        ImGui::NewLine;
        ImGui::End();

        if (spinning)
            orientationY += (deltaTime * spin_speed);

        //sceneGraph.gameObjects[sphereSGHandle].localTransform.translate +=
        //    VgMath::Vector3(1.0, 0.0, 0.0) * deltaTime;
        
        sceneGraph.gameObjects[sphereSGHandle].localTransform.rotate =
            VgMath::Quaternion::angleAxis(VgMath::Degrees(orientationY), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

        sceneGraph.CalculateWorldTransforms(
            cumulatedTransforms,
            modelMatrices
        );

        //// TO BE REMOVED
        //for (auto& it : sceneGraph.gameObjects) {
        //    it.second.cumulativeTransform = cumulatedTransforms[it.first];
        //    it.second.modelMatrix = modelMatrices[it.first];
        //}

        ////////////////////////////////////////
        
        renderer.RenderScene(
            &rendereableObjects,
            view,
            projection,
            lightDir0,
            &shadow_shader,
            &illumination_shader,
            &camera,
            renderer.depthMapFBO,
            renderer.depthMap,
            wireframe,
            current_subroutine,
            &shaders,
            renderer.width,
            renderer.height
        );

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swapping back and front buffers
        glfwSwapBuffers(renderer.window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // when I exit from the graphics loop, it is because the application is closing
    // we delete the Shader Programs
    illumination_shader.Delete();
    shadow_shader.Delete();
    // chiudo e cancello il contesto creato
    glfwTerminate();
    return 0;
}

//////////////////////////////////////////
// we load the image from disk and we create an OpenGL texture
GLint LoadTexture(const char* path)
{
    GLuint textureImage;
    int w, h, channels;
    unsigned char* image;
    image = stbi_load(path, &w, &h, &channels, STBI_rgb);

    if (image == nullptr)
        std::cout << "Failed to load texture!" << std::endl;

    glGenTextures(1, &textureImage);
    glBindTexture(GL_TEXTURE_2D, textureImage);
    // 3 channels = RGB ; 4 channel = RGBA
    if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    // we set how to consider UVs outside [0,1] range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // we set the filtering for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // we free the memory once we have created an OpenGL texture
    stbi_image_free(image);

    // we set the binding to 0 once we have finished
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureImage;
}

///////////////////////////////////////////
// The function parses the content of the Shader Program, searches for the Subroutine type names,
// the subroutines implemented for each type, print the names of the subroutines on the terminal, and add the names of
// the subroutines to the shaders vector, which is used for the shaders swapping
void SetupShader(int program)
{
    int maxSub, maxSubU, countActiveSU;
    GLchar name[256];
    int len, numCompS;

    // global parameters about the Subroutines parameters of the system
    glGetIntegerv(GL_MAX_SUBROUTINES, &maxSub);
    glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubU);
    std::cout << "Max Subroutines:" << maxSub << " - Max Subroutine Uniforms:" << maxSubU << std::endl;

    // get the number of Subroutine uniforms (only for the Fragment shader, due to the nature of the exercise)
    // it is possible to add similar calls also for the Vertex shader
    glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &countActiveSU);

    // print info for every Subroutine uniform
    for (int i = 0; i < countActiveSU; i++) {

        // get the name of the Subroutine uniform (in this example, we have only one)
        glGetActiveSubroutineUniformName(program, GL_FRAGMENT_SHADER, i, 256, &len, name);
        // print index and name of the Subroutine uniform
        std::cout << "Subroutine Uniform: " << i << " - name: " << name << std::endl;

        // get the number of subroutines
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);

        // get the indices of the active subroutines info and write into the array s
        int* s = new int[numCompS];
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_COMPATIBLE_SUBROUTINES, s);
        std::cout << "Compatible Subroutines:" << std::endl;

        // for each index, get the name of the subroutines, print info, and save the name in the shaders vector
        for (int j = 0; j < numCompS; ++j) {
            glGetActiveSubroutineName(program, GL_FRAGMENT_SHADER, s[j], 256, &len, name);
            std::cout << "\t" << s[j] << " - " << name << "\n";
            shaders.push_back(name);
        }
        std::cout << std::endl;

        delete[] s;
    }
}

/////////////////////////////////////////
// we print on console the name of the currently used shader subroutine
void PrintCurrentShader(int subroutine)
{
    std::cout << "Current shader subroutine: " << shaders[subroutine] << std::endl;
}

//////////////////////////////////////////
// If one of the WASD keys is pressed, the camera is moved accordingly (the code is in utils/camera.h)
void apply_camera_movements()
{
    // if a single WASD key is pressed, then we will apply the full value of velocity v in the corresponding direction.
    // However, if two keys are pressed together in order to move diagonally (W+D, W+A, S+D, S+A), 
    // then the camera will apply a compensation factor to the velocities applied in the single directions, 
    // in order to have the full v applied in the diagonal direction  
    // the XOR on A and D is to avoid the application of a wrong attenuation in the case W+A+D or S+A+D are pressed together.  
    GLboolean diagonal_movement = (keys[GLFW_KEY_W] ^ keys[GLFW_KEY_S]) && (keys[GLFW_KEY_A] ^ keys[GLFW_KEY_D]);
    camera.SetMovementCompensation(diagonal_movement);

    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_LEFT_CONTROL])
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (keys[GLFW_KEY_SPACE])
        camera.ProcessKeyboard(UP, deltaTime);
}

//////////////////////////////////////////
// callback for keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    GLuint new_subroutine;

    // if ESC is pressed, we close the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    //// if P is pressed, we start/stop the animated rotation of models
    //if (key == GLFW_KEY_P && action == GLFW_PRESS)
    //    spinning = !spinning;

    // if L is pressed, we activate/deactivate wireframe rendering of models
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        wireframe = !wireframe;

    // pressing a key number, we change the shader applied to the models
    // if the key is between 1 and 9, we proceed and check if the pressed key corresponds to
    // a valid subroutine
    if ((key >= GLFW_KEY_1 && key <= GLFW_KEY_9) && action == GLFW_PRESS)
    {
        // "1" to "9" -> ASCII codes from 49 to 59
        // we subtract 48 (= ASCII CODE of "0") to have integers from 1 to 9
        // we subtract 1 to have indices from 0 to 8
        new_subroutine = (key - '0' - 1);
        // if the new index is valid ( = there is a subroutine with that index in the shaders vector),
        // we change the value of the current_subroutine variable
        // NB: we can just check if the new index is in the range between 0 and the size of the shaders vector,
        // avoiding to use the std::find function on the vector
        if (new_subroutine < shaders.size())
        {
            current_subroutine = new_subroutine;
            PrintCurrentShader(current_subroutine);
        }
    }

    // we keep trace of the pressed keys
    // with this method, we can manage 2 keys pressed at the same time:
    // many I/O managers often consider only 1 key pressed at the time (the first pressed, until it is released)
    // using a boolean array, we can then check and manage all the keys pressed at the same time
    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
}

//////////////////////////////////////////
// callback for mouse events
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // we move the camera view following the mouse cursor
    // we calculate the offset of the mouse cursor from the position in the last frame
    // when rendering the first frame, we do not have a "previous state" for the mouse, so we set the previous state equal to the initial values (thus, the offset will be = 0)
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // offset of mouse cursor position
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    // the new position will be the previous one for the next frame
    lastX = xpos;
    lastY = ypos;

    // we pass the offset to the Camera class instance in order to update the rendering
    camera.ProcessMouseMovement(xoffset, yoffset);

}

void imGuiSetup(GLFWwindow* window)
{
    // ImGui SETUP
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}