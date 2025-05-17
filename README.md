DROP

a simple game engine


# Setup

This set-up works for **Windows x64**

### 1. Download static libraries

Download the directory "dependencies"

https://drive.google.com/drive/folders/1BnptvQUHHxcnXt53LJGVPdqLg2EXvkPA?usp=sharing

### 2. Generate the vs solutions and projects

- double click on "GenerateProjects.bat"

# TODO List
- [x] ECS porting (branch ECS_Setup)
    - [x] Renderer refactoring
    - [x] Static mesh component
    - [x] Transform component
    - [x] Particle system porting
    - [ ] Add multithreading to transform updates
    - [ ] Add multithreading to particle update
    - [ ] rename Scene and Renderer context into state
    - [ ] add a Game State to singleton components
    - [ ] Add time state
    - [ ] Move the active camera to a singleton component (Scene context)
    - [ ] Move the Inputs into a Singleton Components (TBD)
    - [ ] Consider refactoring singleton components to make them more efficient (without using a map) 
- [x] Code cleanup:
    - [x] Fix relative path for assets, using path from exe 
- [ ] Add debug main view, render normals in color channel
- [ ] Add debug main view, render shadow map with inverted colors, projected on the plane
- [ ] Consider reworking the material to use a map for textures 
- [ ] Add debug texture viewer in the renderer by adding a specific pass where needed
- [ ] Fix the scene graph move and remove node, the error is not considering move from another branch
- [ ] Add cube maps


# Arachitecture

![DROPArchitecture](https://github.com/user-attachments/assets/8edc2ab9-edb3-4983-beb6-bf3c770f03a7)

# Shadows

In order to create shadows we need to put the camera on the light position with the same orientation as the direction of the light.
We do a first render pass, witout calculating the color, in order to find the surfaces that are hidden by the objects.
We do a second render pass, this time we calculate the color of the fragments keeping in consideration also the area obscured by the shadows.

# Scene Graph

Implementing the scene graph using a tree.
We will store the transform in local coordinates and the cumulated transform in world coordinates.
Local transform for the transformations related to the single object.
The cumulated one for the scale, orientation and postion of the object in world space, to be used by the other components.

# Particle System

![image](https://github.com/user-attachments/assets/ad58199a-1f34-45df-a140-754480f9fa46)

![image](https://github.com/user-attachments/assets/4793492b-4a5c-4299-8a05-2b2bc904a3ff)

![image](https://github.com/user-attachments/assets/cc46a3b7-93f5-4fb0-a87d-c1d8ca80e737)

![image](https://github.com/user-attachments/assets/4ba4b20a-ca1e-46cf-b3b4-296daccfa756)



