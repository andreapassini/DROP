#pragma once

#include "glm/glm.hpp"
#include "glad/glad.h"

struct TextureParameter
{
    bool UseTexture = false;
    GLuint TextureId = 0;
    std::vector<int>* Textures = nullptr;
    // UV repetitions
    GLfloat Repeat = 1.0f;
};