#include "line.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <glfw/glfw3.h>


// default color GREEN
Line::Line(glm::vec3 start, glm::vec3 end, glm::vec3 color)
{
    m_StartPoint = start;
    m_EndPoint = end;
    m_LineColor = color;

    //m_Vertices = {
    //        start.x, start.y, start.z,
    //        end.x, end.y, end.z,
    //};

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices) * 6, m_Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


Line& Line::operator=(Line&& line) noexcept
{
    FreeGPUresources();

    if (line.m_VAO)
    {
        //m_Vertices = std::move(line.m_Vertices);
        m_LineColor = std::move(line.m_LineColor);
        m_StartPoint = std::move(line.m_StartPoint);
        m_EndPoint = std::move(line.m_EndPoint);
        m_VAO = line.m_VAO;
        m_VBO = line.m_VBO;

        line.m_VAO = 0;
    }
    else
    {
        m_VAO = 0;
    }

    return *this;
}

Line::~Line() noexcept
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Line::FreeGPUresources()
{
    if (m_VAO)
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        m_VAO = 0;
        m_VBO = 0;
    }
}
