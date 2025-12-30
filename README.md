DROP

a simple game engine


# Setup

This set-up works for **Windows x64**

### 1. Download static libraries

Download the directory "dependencies", paste assimp.lib into the project dependencies folder.

https://drive.google.com/drive/folders/1BnptvQUHHxcnXt53LJGVPdqLg2EXvkPA?usp=sharing

### 2. Generate the vs solutions and projects

- double click on "scripts\\Setup.bat"

# TODO List
- Project structure
    - [ ] Link libs with GE
    - [ ] Expose all the libs call via a GE layer to the game

- Math
    - [x] Vector4

- ECS porting
    - [x] Static mesh component
    - [x] Transform component
    - [x] Particle system porting
    - [ ] Add multithreading to transform updates
    - [ ] Add multithreading to particle update
    - [ ] Fix the scene graph move and remove node, the error is not considering move from another branch
    - [ ] rename Scene and Renderer context into state
    - [ ] add a Game State to singleton components
    - [ ] Add time state
    - [ ] Move the active camera to a singleton component (Scene context)
    - [ ] Move the Inputs into a Singleton Components (TBD)
    - [ ] Consider refactoring singleton components to make them more efficient (without using a map) 

- Particles
  - [ ] GPU Particles

- Code cleanup:
    - [x] Fix relative path for assets, using path from exe 

- Rendering
    - [x] Renderer refactoring
    - [x] Render translucent objects
    - [x] back-face culling, fix the CCW(for mesh) and CW(for billboards)
    - Debug
        - [ ] Add debug texture viewer in the renderer by adding a specific pass where needed
        - [ ] Add debug main view, render normals in color channel
        - [ ] Add debug main view, render shadow map with inverted colors, projected on the plane
    - [ ] Consider reworking the material to use a map for textures 
    - [ ] Add Normale, Specular, Bump and Displacement maps
    - [ ] Add cube maps

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



# Live Coding

By having the game as a DLL we can:
- Edit the code at runtime
- Rebuild the game DLL (*)
- Detach the old DLL and Attach the new one (*)

## Caveat

- We cannot allocate memory on the DLL and hope to be available again   

### Satic on DLL
Statics on DLL will be whiped  
**Ex:**  
Using a std::vector as static value

At the first run
```c++
static std::vector<int32_t> Vector;
```
```c++
DLL_PROCESS_ATTACH
Message printed from executable
<<< StartGame
        Number: 1
        Number: 1
UpdateGame with deltaTime: 0.0166
Vector:
         size: 6
         ptr to raw data: 000002B1E9E7B740
         Content:
                 - at 0, value: 1
                 - at 1, value: 1
                 - at 2, value: 1
                 - at 3, value: 1
                 - at 4, value: 1
                 - at 5, value: 1
```

After rebuilding DLL
```c++
DLL_PROCESS_DETACH
DLL_PROCESS_ATTACH
Message printed from executable
        Number: 1
UpdateGame with deltaTime: 0.0166
Vector:
         size: 0
```
