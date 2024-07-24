#pragma once

#include "glm/glm.hpp"
#include "glad/glad.h"

struct TextureParameter
{
    bool UseTexture = false;
    uint32_t TextureId = 0;
    std::vector<int>* Textures = nullptr;
    // UV repetitions
    float Repeat = 1.0f;
};