#pragma once

#include "glm/glm.hpp"
#include "vector"

class DrawableBox
{
public:
    DrawableBox(glm::vec3 min, glm::vec3 max);

    inline uint32_t GetVAO()
    {
        return m_VAO;
    }

    ~DrawableBox();

public:
    glm::vec3 m_LineColor = glm::vec3(0, 1, 0);
    std::vector<uint32_t> m_Indices;
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

    uint32_t m_VBO, m_VAO, m_EBO;

private:
    std::vector<float> m_Vertices;

    glm::vec3 m_Min;
    glm::vec3 m_Max;

};