#pragma once

#include "glm/glm.hpp"
#include "vector"

class Line
{
public:
    Line(glm::vec3 start, glm::vec3 end);
    Line(glm::vec3 start, glm::vec3 end, glm::vec3 color);

    inline unsigned int GetVAO()
    {
        return m_VAO;
    }

    ~Line();

public:
    glm::vec3 m_LineColor;

private:
    unsigned int m_VBO, m_VAO;

    std::vector<float> m_Vertices;

    glm::vec3 m_StartPoint;
    glm::vec3 m_EndPoint;

};