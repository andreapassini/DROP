DROP

a simple game engine


# Setup

This set-up works for **Windows x64**

### 1. Download static libraries

Download the directory "dependencies"

https://drive.google.com/drive/folders/1BnptvQUHHxcnXt53LJGVPdqLg2EXvkPA?usp=sharing

### 2. Generate the vs solutions and projects

- double click on "GenerateProjects.bat"

# Shadows

In order to create shadows we need to put the camera on the light position with the same orientation as the direction of the light.
We do a first render pass, witout calculating the color, in order to find the surfaces that are hidden by the objects.
We do a second render pass, this time we calculate the color of the fragments keeping in consideration also the area obscured by the shadows.