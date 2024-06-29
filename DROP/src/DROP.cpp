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
#include "input/Input.h"
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

#include "physics/physicsEngine.h"

using namespace DROP;

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
void SubroutineKeyCallback(GLFWwindow* window);

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

// when rendering the first frame, we do not have a "previous state" for the mouse, so we need to manage this situation
bool firstMouse = true;

// parameters for time calculation (for animations)
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastFramerateUpdate = 0.0f;
const GLfloat framerateUpdateTime = 0.5f;
uint32_t framerate;

// rotation angle on Y axis[
VgMath::Scalar orientationY = 0.0f;
// rotation speed on Y axis
GLfloat spin_speed = 30.0f;
// boolean to start/stop animated rotation on Y angle
GLboolean spinning = GL_TRUE;

// boolean to activate/deactivate wireframe rendering
GLboolean wireframe = GL_FALSE;

// View matrix: the camera moves, so we just set to indentity now
glm::mat4 view = glm::mat4(1.0f);

// in this example, we consider a directional light. We pass the direction of incoming light as an uniform to the shaders
glm::vec3 lightDir0 = glm::vec3(1.0f, 1.0f, 1.0f);

// vector for the textures IDs
std::vector<GLint> textureID;

//#define UNLOCK_FRAMERTE
//#define VISIBLE_MOUSE

int main()
{
    // We are in DROP/DROP!!!
    Renderer renderer(
        screenWidth,
        screenHeight
    );

    Camera camera(
        glm::vec3(0.0f, 0.0f, 6.0f),
        45.0f,
        0.1f,
        100.0f,
        10.0f,
        0.75f);

    Input::m_WindowHandle = renderer.m_Window;

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

    imGuiSetup(renderer.m_Window);

    SceneGraph sceneGraph(150);

    sceneGraph.m_GameObjects[SceneGraph::ROOT_ID].m_LocalTransform.m_Rotate =
        VgMath::Quaternion::angleAxis(VgMath::Degrees(0.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    // Projection matrix of the camera: FOV angle, aspect ratio, near and far planes
    glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 10'000.0f);

    std::vector<RenderableObject> rendereableObjects;

    uint32_t planeSGHandle = sceneGraph.AddObject(SceneGraph::ROOT_ID);
    sceneGraph.m_GameObjects[planeSGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(0.0f, -1.0f, 0.0f);
    sceneGraph.m_GameObjects[planeSGHandle].m_LocalTransform.m_Scale = 10.0;
    sceneGraph.m_GameObjects[planeSGHandle].m_LocalTransform.m_Rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(90.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter planeTextureParameter;
    planeTextureParameter.UseTexture = true;
    planeTextureParameter.TextureId = 1;
    planeTextureParameter.Textures = &textureID;
    planeTextureParameter.Repeat = 80.0f;

    Material planeMaterial;
    planeMaterial.Kd = 3.0f;
    planeMaterial.Alpha = 0.2f;
    planeMaterial.F0 = 0.9f;
    planeMaterial.CastShadow = false;

    // Sphere
    uint32_t sphereSGHandle = sceneGraph.AddObject(SceneGraph::ROOT_ID);
    sceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(-3.0f, 1.0f, 0.0f);
    sceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Scale = 1.0;
    sceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(90.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter sphereTextureParameter;
    sphereTextureParameter.UseTexture = true;
    sphereTextureParameter.TextureId = 0;
    sphereTextureParameter.Textures = &textureID;

    Material sphereMaterial;
    sphereMaterial.Kd = 3.0f;
    sphereMaterial.Alpha = 0.2f;
    sphereMaterial.F0 = 0.9f;
    sphereMaterial.CastShadow = true;

    // Cube
    uint32_t cubeSGHandle = sceneGraph.AddObject(sphereSGHandle);
    sceneGraph.m_GameObjects[cubeSGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
    sceneGraph.m_GameObjects[cubeSGHandle].m_LocalTransform.m_Scale = 0.48;
    sceneGraph.m_GameObjects[cubeSGHandle].m_LocalTransform.m_Rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(90.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter cubeTextureParameter;
    cubeTextureParameter.UseTexture = true;
    cubeTextureParameter.TextureId = 0;
    cubeTextureParameter.Textures = &textureID;

    Material cubeMaterial;
    cubeMaterial.Kd = 3.0f;
    cubeMaterial.Alpha = 0.2f;
    cubeMaterial.F0 = 0.9f;
    cubeMaterial.CastShadow = false;


    // Bunny
    uint32_t bunnySGHandle = sceneGraph.AddObject(SceneGraph::ROOT_ID);
    sceneGraph.m_GameObjects[bunnySGHandle].m_LocalTransform.m_Translate = VgMath::Vector3(3.0f, 1.0f, 0.0f);
    sceneGraph.m_GameObjects[bunnySGHandle].m_LocalTransform.m_Scale = 0.3;
    sceneGraph.m_GameObjects[bunnySGHandle].m_LocalTransform.m_Rotate = 
        VgMath::Quaternion::angleAxis(VgMath::Degrees(0.0), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

    TextureParameter bunnyTextureParameter;
    bunnyTextureParameter.UseTexture = true;
    bunnyTextureParameter.TextureId = 0;
    bunnyTextureParameter.Textures = &textureID;

    Material bunnyMaterial;
    bunnyMaterial.Kd = 3.0f;
    bunnyMaterial.Alpha = 0.2f;
    bunnyMaterial.F0 = 0.9f;
    bunnyMaterial.CastShadow = true;

    PerformanceCalculator framerateCalculator(100, 25);

    // Initialize cumulated transform hash map
    std::unordered_map<uint32_t, VgMath::Transform> cumulatedTransforms;
    cumulatedTransforms.reserve( sceneGraph.m_GameObjects.size());

    std::unordered_map<uint32_t, glm::mat4> modelMatrices;
    modelMatrices.reserve(sceneGraph.m_GameObjects.size());

    // Now the 2 maps have all the keys as the SceneGraph
    for (auto& it : sceneGraph.m_GameObjects) {
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

    PhysicsEngine physicsEngine(glfwGetTime(), 5U);

    // Rendering loop: this code is executed at each frame
    while (!glfwWindowShouldClose(renderer.m_Window))
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

        SubroutineKeyCallback(renderer.m_Window);
        camera.OnResize(renderer.m_Width, renderer.m_Height);
        camera.OnUpdate(deltaTime);

        // render your GUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controls");

        ImGui::Text("DROP");
        ImGui::NewLine;
        ImGui::Text("FPS: %d", framerate);
        ImGui::NewLine;
        
        ImGui::Text("ON/OFF cursor:\n");
        ImGui::Text("   Right mouse btn");
        
        ImGui::Separator();
        ImGui::Text("Camera pos: \n\t%.3f, \n\t%.3f, \n\t%.3f", camera.m_Position.x, camera.m_Position.y, camera.m_Position.z);

        ImGui::End();

        if (spinning)
            orientationY += (deltaTime * spin_speed);

        //sceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Translate +=
        //    VgMath::Vector3(1.0, 0.0, 0.0) * deltaTime;
        
        sceneGraph.m_GameObjects[sphereSGHandle].m_LocalTransform.m_Rotate =
            VgMath::Quaternion::angleAxis(VgMath::Degrees(orientationY), VgMath::Vector3(0.0, 1.0, 0.0).normalized());

        sceneGraph.CalculateWorldTransforms(
            cumulatedTransforms,
            modelMatrices
        );

        ////////////////////////////////////////
        
        renderer.RenderScene(
            &rendereableObjects,
            camera.GetViewMatrix(),
            camera.GetProjectionMatrix(),
            lightDir0,
            &shadow_shader,
            &illumination_shader,
            renderer.m_DepthMapFBO,
            renderer.m_DepthMap,
            wireframe,
            current_subroutine,
            &shaders,
            renderer.m_Width,
            renderer.m_Height
        );

        uint32_t physIter = 0;
        while (!physicsEngine.m_IsPaused && currentTime > physicsEngine.getVirtualTIme()) {

            //physicsEngine.AddForceToAll(glm::vec3(0.0f, gravity, 0.0f));

            physicsEngine.PhysicsStep();

            physIter++;

            if (physIter > physicsEngine.maxIter) {
                std::cout << "Physics Simulation lagging " << std::endl;
                physicsEngine.SynchVirtualTime(currentTime);
                break;
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swapping back and front buffers
        glfwSwapBuffers(renderer.m_Window);
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
// callback for keyboard events
void SubroutineKeyCallback(GLFWwindow* window)
{

    // if ESC is pressed, we close the application
    if(Input::IsKeyDown(Key::Escape))
        glfwSetWindowShouldClose(window, GL_TRUE);

    // if L is pressed, we activate/deactivate wireframe rendering of models
    if (Input::IsKeyDown(Key::L))
        wireframe = !wireframe;

    GLuint new_subroutine;
    for (int i = 0; i < 9; i++)
    {
        int key = (int)Key::D0 + i;
        if (Input::IsKeyDown(KeyCode(key)))
        {
            // "1" to "9" -> ASCII codes from 49 to 59
            // we subtract 48 (= ASCII CODE of "0") to have integers from 1 to 9
            // we subtract 1 to have indices from 0 to 8
            new_subroutine = (key - '0' - 1);
            if (new_subroutine < shaders.size())
            {
                current_subroutine = new_subroutine;
                PrintCurrentShader(current_subroutine);
            }
        }

    }
}

void imGuiSetup(GLFWwindow* window)
{
    // ImGui SETUP
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}