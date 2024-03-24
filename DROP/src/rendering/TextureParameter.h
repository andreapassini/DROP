#pragma once

#include "glm/glm.hpp"
#include "glad/glad.h"

class TextureParameter
{
private:
public:
    bool useTexture;
    GLuint textureId;
    std::vector<int>* textures;
    // UV repetitions
    GLfloat repeat;

    TextureParameter(
        bool use_texture_val = false,
        GLuint textId_val = 0,
        std::vector<int>* textures_val = nullptr,
        GLfloat repeatParameter_val = 1.0f)
        :
        useTexture(use_texture_val),
        textureId(textId_val),
        textures(textures_val),
        repeat(repeatParameter_val) {};

    ~TextureParameter() {};
};