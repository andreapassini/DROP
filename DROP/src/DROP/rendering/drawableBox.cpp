#include "drawableBox.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

DrawableBox::DrawableBox(glm::vec3 min, glm::vec3 max)
    : 
    m_Min(min),
    m_Max(max)
{
    m_Vertices = {
        m_Min.x, m_Min.y, m_Min.z,
        m_Max.x, m_Min.y, m_Min.z,
        m_Min.x, m_Max.y, m_Min.z,
        m_Min.x, m_Min.y, m_Max.z,

        m_Max.x, m_Max.y, m_Max.z,
        m_Min.x, m_Max.y, m_Max.z,
        m_Max.x, m_Min.y, m_Max.z,
        m_Max.x, m_Max.y, m_Min.z
    };

    m_Indices = {
        0, 1, 7, // Front
        0, 7, 2,
        //0, 2, 7, 
        
        2, 7, 4, // Top
        2, 4, 5,
        
        1, 6, 4, // Right
        1, 4, 7,

        0, 3, 5, // Left
        0, 5, 2,

        0, 1, 6, // Bottom
        0, 6, 3,

        3, 6, 4, // Back
        3, 4, 5
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_Vertices.size(), &this->m_Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_Indices.size() * sizeof(uint32_t), &this->m_Indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

DrawableBox::~DrawableBox()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}