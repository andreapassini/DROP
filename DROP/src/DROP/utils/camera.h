#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera();
    Camera(glm::vec3 position,  
        float verticalFOV,
        float nearClip,
        float farClip,
        float MovementSpeed,
        float MouseSensitivity)
        : m_Position(position), 
        m_VerticalFOV(verticalFOV), 
        m_NearClip(nearClip), 
        m_FarClip(farClip),
        m_MovementSpeed(MovementSpeed),
        m_MouseSensitivity(MouseSensitivity),
        m_Forward(glm::vec3(0, 0, -1))
    { 
        RecalculateView();
    }

    bool OnUpdate(float deltaTime);
    void OnResize(uint32_t width, uint32_t height);

    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::mat4& GetInverseProjectionMatrix() const { return m_InverseProjectionMatrix; }
    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4& GetInverseViewMatrix() const { return m_InverseViewMatrix; }
public:
    glm::vec3 m_Position{ 0.0f };
    glm::vec3 m_Forward = glm::vec3(0, 0, -1);

    float m_MovementSpeed = 10.0f;

    // Camera rotation parameter
    float m_MouseSensitivity = 0.25f;
private:
    void RecalculateProjection();
    void RecalculateView();
private:
    float m_VerticalFOV = 45.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 100.0f;

    glm::mat4 m_ProjectionMatrix{ 1.0f };
    glm::mat4 m_ViewMatrix{ 1.0f };
    glm::mat4 m_InverseProjectionMatrix{ 1.0f };
    glm::mat4 m_InverseViewMatrix{ 1.0f };

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    glm::vec2 m_LastMousePosition = glm::vec2(0.0f, 0.0f);
};
