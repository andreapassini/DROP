DROP

a simple game engine


# Setup

This set-up works for **Windows x64**

### 1. Download static libraries

Download the directory "dependencies"

https://drive.google.com/drive/folders/1BnptvQUHHxcnXt53LJGVPdqLg2EXvkPA?usp=sharing

### 2. Generate the vs solutions and projects

- double click on "GenerateProjects.bat"

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

![image](https://github.com/user-attachments/assets/70f6bfc4-3504-4031-bca7-cbb88a01a625)


