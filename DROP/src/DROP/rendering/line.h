#pragma once

#include "glm/glm.hpp"
#include "vector"

class Line
{
public:
    Line(glm::vec3 start, glm::vec3 end);

    // We have to delete copy
    // since vec when resize will delete the Back(copy)
    // and our VAO will be delete from FreeGPUresources()
    
    Line(const Line& line) = delete;
    Line& operator= (const Line& line) = delete;

    Line(Line&& line) noexcept
        : m_Vertices(std::move(line.m_Vertices)),
        m_ModelMatrix(std::move(line.m_ModelMatrix)),
        m_LineColor(std::move(line.m_LineColor)),
        m_StartPoint(std::move(line.m_StartPoint)),
        m_EndPoint(std::move(line.m_EndPoint)),
        m_VAO(line.m_VAO),
        m_VBO(line.m_VBO)
    {
        line.m_VAO = 0;
    }
    Line& operator= (Line&& line) noexcept;

    inline unsigned int GetVAO()
    {
        return m_VAO;
    }

    ~Line() noexcept;
public:
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    glm::vec3 m_LineColor = glm::vec3(0, 1, 0);

private:
    void FreeGPUresources();
private:
    unsigned int m_VBO, m_VAO;

    std::vector<float> m_Vertices;

    glm::vec3 m_StartPoint;
    glm::vec3 m_EndPoint;

};