#pragma once

#include "glm/glm.hpp"
#include "vector"

constexpr glm::vec3 DEFAULT_LINE_COLOR = glm::vec3(0.0f, 1.0f, 0.0f);

class Line
{
public:
    Line(glm::vec3 start, glm::vec3 end, glm::vec3 color = DEFAULT_LINE_COLOR);

    inline unsigned int GetVAO() const
    {
        return m_VAO;
    }

    ~Line();

public:
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    glm::vec3 m_LineColor;

private:
    unsigned int m_VBO, m_VAO;

    std::vector<float> m_Vertices;

    glm::vec3 m_StartPoint;
    glm::vec3 m_EndPoint;

};