#pragma once

#include "colors.h"

#include "glm/glm.hpp"
#include "vector"


class Line
{
public:
    Line(glm::vec3 start, glm::vec3 end, glm::vec3 color = DEFAULT_LINE_COLOR);

    // We have to delete copy
    // since vec when resize will delete the Back(copy)
    // and our VAO will be delete from FreeGPUresources()
    Line(const Line& line) = delete;
    Line& operator= (const Line& line) = delete;

    Line(Line&& line) noexcept
        : /*m_Vertices(std::move(line.m_Vertices)),*/
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


    ~Line() noexcept;

    inline unsigned int GetVAO() const
    {
        return m_VAO;
    }


public:
    // Do we really need a ModelMatrix? 
    // cant we just transform the points and use a identity matrix when drawing?
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    glm::vec3 m_LineColor;

private:
    void FreeGPUresources();
private:
    unsigned int m_VBO, m_VAO;

    // Use a union for vertices and start/end points
    union
    {
        struct 
        {
            glm::vec3 m_StartPoint;
            glm::vec3 m_EndPoint;
        };

        float m_Vertices[6];
    };

    //std::vector<float> m_Vertices;

    //glm::vec3 m_StartPoint;
    //glm::vec3 m_EndPoint;

};