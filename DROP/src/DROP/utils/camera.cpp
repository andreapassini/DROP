#include "camera.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "DROP/input/Input.h"
#include "DROP/math/versor3.h"

using namespace Drop;

bool Camera::OnUpdate(float deltaTime)
{
	glm::vec2 mousePos = Input::GetMousePosition();
	glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
	m_LastMousePosition = mousePos;

	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return false;
	}

	Input::SetCursorMode(CursorMode::Locked);

	bool moved = false;

	constexpr glm::vec3 worldUpDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDirection = glm::cross(m_Forward, worldUpDirection);

	// Movement
	if (Input::IsKeyPressed(KeyCode::W))
	{
		m_Position += m_Forward * m_MovementSpeed * deltaTime;
		moved = true;
	}
	else if (Input::IsKeyPressed(KeyCode::S))
	{
		m_Position -= m_Forward * m_MovementSpeed * deltaTime;
		moved = true;
	}
	if (Input::IsKeyPressed(KeyCode::A))
	{
		m_Position -= rightDirection * m_MovementSpeed * deltaTime;
		moved = true;
	}
	else if (Input::IsKeyPressed(KeyCode::D))
	{
		m_Position += rightDirection * m_MovementSpeed * deltaTime;
		moved = true;
	}
	if (Input::IsKeyPressed(KeyCode::Q))
	{
		m_Position -= worldUpDirection * m_MovementSpeed * deltaTime;
		moved = true;
	}
	else if (Input::IsKeyPressed(KeyCode::E))
	{
		m_Position += worldUpDirection * m_MovementSpeed * deltaTime;
		moved = true;
	}

	// Rotation
	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		float pitchDelta = delta.y * m_MouseSensitivity;
		float yawDelta = delta.x * m_MouseSensitivity;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
			glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
		m_Forward = glm::rotate(q, m_Forward);

		moved = true;
	}

	if (moved)
	{
		RecalculateView();
	}

	return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
	if (width == m_ViewportWidth && height == m_ViewportHeight)
		return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjection();
}

void Camera::RecalculateProjection()
{
	m_ProjectionMatrix = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	m_InverseProjectionMatrix = glm::inverse(m_ProjectionMatrix);
}

void Camera::RecalculateView()
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, glm::vec3(0.0f, 1.0f, 0.0f));
	m_InverseViewMatrix = glm::inverse(m_ViewMatrix);
}