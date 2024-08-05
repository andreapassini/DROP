#pragma once

#include "glm/glm.hpp"
#include "vector"

class Line
{
public:
    Line(glm::vec3 start, glm::vec3 end);

    inline unsigned int GetVAO()
    {
        return m_VAO;
    }

    ~Line();

public:
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    glm::vec3 m_LineColor = glm::vec3(0, 1, 0);

private:
    unsigned int m_VBO, m_VAO;

    std::vector<float> m_Vertices;

    glm::vec3 m_StartPoint;
    glm::vec3 m_EndPoint;

};