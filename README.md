DROP

a simple game engine


# Setup

This set-up works for **Windows x64**

### 1. Download static libraries

Download the directory "dependencies", paste assimp.lib into the project dependencies folder.

https://drive.google.com/drive/folders/1BnptvQUHHxcnXt53LJGVPdqLg2EXvkPA?usp=sharing

### 2. Generate the vs solutions and projects

- double click on "scripts\\Setup.bat"

## Next Big Features Roadmap

- Vertex Animations for Grass
- Outline and stencil buffer
- Collision Detection & Response System
- Physics simulation for rigid bodies made of particles (rework and how to convert particle pos into transform?)
- Skeletal animations
- Cloth simulation (just a rework)
- Fluid simulation (Look at Doom:DarkAges video https://youtu.be/Ed4vNNQwCDU?si=3qRpfoFei1mSx-Qn)
- RayMarching for clouds and fog (https://youtu.be/Ed4vNNQwCDU?si=3qRpfoFei1mSx-Qn)
- Vulkan and renderer rework (winter is coming)

# Architecture
![DROPArchitecture](https://github.com/user-attachments/assets/8edc2ab9-edb3-4983-beb6-bf3c770f03a7)

# Shadows

In order to create shadows we need to put the camera on the light position with the same orientation as the direction of the light.
We do a first render pass, without calculating the color, in order to find the surfaces that are hidden by the objects.
We do a second render pass, this time we calculate the color of the fragments keeping in consideration also the area obscured by the shadows.


# Vulkan Renderer

https://howtovulkan.com/#libraries

## Setup

### Shading Language

Vulkan consumes shaders in an intermediate format called **SPIR-V**.
This decouples the API from the actual shading language.
https://www.khronos.org/spirv/

![SPIR-V Language Ecosystem](https://github.com/user-attachments/assets/c1688b2a-56ed-4bce-b4ee-ace3ae153fb5)

### Vulkan SDK

- **Volk** (https://github.com/zeux/volk) - Meta-loader that simplifies loading of Vulkan functions
- **VMA** (https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) - Simplifies dealing with memory allocations. Removes some of the verbosity around memory management

https://vulkan.lunarg.com/sdk/home

While it's not required for developing Vulkan applications, the LunarG Vulkan SDK provides a convenient way to install commonly used libraries and tools, some of which are used in this tutorial. It's therefore recommended to install this. When installing, be sure to select the optional GLM, Volk, SDL3 and Vulkan Memory Allocator components. Alternatively, you can download these from their respective repositories and adjust the include paths in the CMakeLists.txt file.

### Validation layers

Vulkan was designed to minimize driver overhead. While that can result in better performance, it also removes many of the safeguards that APIs like OpenGL had and puts that responsibility into your hands. If you misuse Vulkan the driver is free to crash. So even if your app works on one GPU, it doesn't guarantee that it works on others. On the other hand, the Vulkan specification defines valid usages for all functionality. And with the validation layers, an easy-to-use tool to check for that exists. (https://github.com/KhronosGroup/Vulkan-ValidationLayers)

Validation layers can be enabled in code, but the easier option is to enable the layers via the Vulkan Configurator GUI provided by the Vulkan SDK. Once they're enabled, any improper use of the API will be logged to the command line window of our application.

# Scene Graph

Implementing the scene graph using a tree.
We will store the transform in local coordinates and the cumulated transform in world coordinates.
Local transform for the transformations related to the single object.
The cumulated one for the scale, orientation and position of the object in world space, to be used by the other components.

# Particle System

![image](https://github.com/user-attachments/assets/ad58199a-1f34-45df-a140-754480f9fa46)

![image](https://github.com/user-attachments/assets/4793492b-4a5c-4299-8a05-2b2bc904a3ff)

![image](https://github.com/user-attachments/assets/cc46a3b7-93f5-4fb0-a87d-c1d8ca80e737)

![image](https://github.com/user-attachments/assets/4ba4b20a-ca1e-46cf-b3b4-296daccfa756)


# Terrain Streaming System

My Bare Bone Terrain Streaming System.



## Pre-requisites

Pre requisite: having a terrain to stream.

Since this is just a toy game engine for the moment, 
I just generated a pseudo random terrain.

## Terrain Setup

- **Divide the scene into grid of cells**.  
    For this example the grid is a square made of 11x11 cells 
    ![TerrainSetup](https://github.com/user-attachments/assets/91956d2c-233b-4895-acc9-6f77f5a6514b)

- **Displace the terrain component** or starting point to the **bottom left** of the scene.       
    This will simplify the grid management since we will not need to handle row and col with negative numbers
    ![TerrainComponentPosition](https://github.com/user-attachments/assets/b00e63d8-8886-47d8-9c3a-6c2f73bbc322)

### Terrain Cell or Slice

Each cell is a slice of our terrain with width and length of 10x10.
It's made of:

- **`plane mesh`**
- **`height map`** or vertical displacement texture 
    
    This is the part that was pseudo randomly generated
    It is made of 81 floats (9x9), based on the number of vertices of the mesh we are using.
    Each pixel represent the vertical displacement applied to a specific mesh vertex.


![SliceOfTerrain](https://github.com/user-attachments/assets/0d0314a0-7414-4f1f-a9b4-fff57c7c5b68)

## Streaming

![Streaming](https://github.com/user-attachments/assets/79c0ff37-97d2-41df-beec-f7c3eb1f859f)

The goal of the Terrain Streaming System is to dynamically load only the necessary displacement maps around the Target/Player.

### Detection

Once the Target/Player changes cell, new displacement maps will be required.

![TargetPosition](https://github.com/user-attachments/assets/e1d5625d-194c-4ca6-a645-613128718b63)

![TargetMovingPosition](https://github.com/user-attachments/assets/dd3f0f5c-1b1c-46c5-8d69-faddf228cff4)

![NewMapsRequested](https://github.com/user-attachments/assets/fc938642-d47c-4c6c-91c2-40f5e88830db)

### Response

The map requested are forwarded to the Asset Thread that will read the specific files and fill its response buffer. Once every N loaded maps, the game thread will copy the results into its buffer to be rendered.

![TerrainGridDisplacement](https://github.com/user-attachments/assets/f320c1d3-25e6-4a63-8aa1-07c8f2d6b83b)

![GridCalculation](https://github.com/user-attachments/assets/b0d2b53a-fe8f-4d2a-aaa3-417033fce857)
