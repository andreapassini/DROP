#pragma once

// we use GLM to create the view matrix and to manage camera transformations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// possible camera movements
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera settings
// Initial camera orientation on Y and X (Z not considered)
const GLfloat YAW        = -90.0f; //Y
const GLfloat PITCH      =  0.0f; //X

// speed of camera movement
//const GLfloat SPEED      =  10.0f;
//const GLfloat UP_DOWN_MULTIPLIER      =  0.5f;
// speed compensation in case of diagonal camera movement (= 1/sqrt(2))
const GLfloat DIAGONAL_COMPENSATION = 0.70710678f;
// parameter to weight mouse movement
//const GLfloat SENSITIVITY =  0.25f;

class Camera
{
public:
    Camera(glm::vec3 position, 
        GLboolean onGround, 
        GLfloat MovementSpeed_val, 
        GLfloat MovementSpeedVerticalMultiplier_val,
        GLfloat MouseSensitivity_val)
        : m_Position(position), 
        m_OnGround(onGround), 
        m_MovementSpeed(MovementSpeed_val), m_MovementSpeedVerticalMultiplier(MovementSpeedVerticalMultiplier_val), 
        m_MouseSensitivity(MouseSensitivity_val)
    {
        // initialization of the camera reference system
        this->UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->m_Position, this->m_Position + this->m_Forward, this->m_Up);
    }
    //////////////////////////////////////////
    // if a single WASD key is pressed, then we will apply the full value of velocity v in the corresponding direction.
    // However, if two keys are pressed together in order to move diagonally (W+D, W+A, S+D, S+A), 
    // then we will apply a compensated velocity in the single directions, in order to have the full v applied in the diagonal direction
    // The compensation is = 1/sqrt(2)
    void SetMovementCompensation(GLboolean diagonal_movement)
    {
        m_MovementCompensation = (diagonal_movement ? DIAGONAL_COMPENSATION : 1.0f);
    }

    // it updates camera position when a WASD key is pressed
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {        
        // the velocity is weighted by a compensation factor
        // = 1 if a single WASD is pressed
        // = 1/sqrt(2) if two keys are pressed for a diagonal movement
        GLfloat velocity = this->m_MovementSpeed * deltaTime * m_MovementCompensation;

        if (direction == FORWARD)
            this->m_Position += (this->m_OnGround ? this->m_WorldForward : this->m_Forward) * velocity;
        if (direction == BACKWARD)
            this->m_Position -= (this->m_OnGround ? this->m_WorldForward : this->m_Forward) * velocity;
        if (direction == LEFT)
            this->m_Position -= this->m_Right * velocity;
        if (direction == RIGHT)
            this->m_Position += this->m_Right * velocity;
        if (direction == UP)
            this->m_Position += this->m_Up * (velocity * m_MovementSpeedVerticalMultiplier);
        if (direction == DOWN)
            this->m_Position -= this->m_Up * (velocity * m_MovementSpeedVerticalMultiplier);

    }

    // it updates camera orientation when mouse is moved
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
    {
        // the sensitivity is applied to weight the movement
        xoffset *= this->m_MouseSensitivity;
        yoffset *= this->m_MouseSensitivity;

        // rotation angles on Y and X are updated
        this->m_Yaw   += xoffset;
        this->m_Pitch += yoffset;

        // we apply a constraint to the rotation on X, to avoid to have the camera flipped upside down
        // N.B.) this constraint helps to avoid gimbal lock, if all the 3 rotations are considered
        if (constraintPitch)
        {
            if (this->m_Pitch > 89.0f)
                this->m_Pitch = 89.0f;
            if (this->m_Pitch < -89.0f)
                this->m_Pitch = -89.0f;
        }

        // the camera reference system is updated using the new camera rotations
        this->UpdateCameraVectors();
    }
public:
    // Camera Attributes
    glm::vec3 m_Position;
    glm::vec3 m_Forward;
    glm::vec3 m_WorldForward;
    glm::vec3 m_Up; // camera local UP vector
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f); //  camera world UP vector -> needed for the initial computation of Right vector
    GLboolean m_OnGround; // it defines if the camera is "anchored" to the ground, or if it strictly follows the current Front direction (even if this means that the camera "flies" in the scene)
    // N.B.) this version works only for flat terrains
    // Eular Angles
    GLfloat m_Yaw = YAW;
    GLfloat m_Pitch = PITCH;
    // Camera speed parameters
    GLfloat m_MovementSpeed;
    GLfloat m_MovementSpeedVerticalMultiplier;
    GLfloat m_MovementCompensation = 1.0f;
    // Camera rotation parameter
    GLfloat m_MouseSensitivity;

private:
    // Update the camera reference system
    void UpdateCameraVectors()
    {
        // it computes the new Front vector using trigonometric calculations using Yaw and Pitch angles
        // https://learnopengl.com/#!Getting-started/Camera
        glm::vec3 m_Forward;
        m_Forward.x = cos(glm::radians(this->m_Yaw)) * cos(glm::radians(this->m_Pitch));
        m_Forward.y = sin(glm::radians(this->m_Pitch));
        m_Forward.z = sin(glm::radians(this->m_Yaw)) * cos(glm::radians(this->m_Pitch));
        this->m_Forward = glm::normalize(m_Forward);
        // if the camera is "anchored" to the ground, the world Front vector is equal to the local Front vector, but with the y component = 0
        m_Forward.y = 0.0f;
        this->m_WorldForward = glm::normalize(m_Forward);
        // Once calculated the new view direction, we re-calculate the Right vector as cross product between Front and world Up vector
        this->m_Right = glm::normalize(glm::cross(this->m_Forward, this->m_WorldUp));
        // we calculate the camera local Up vector as cross product between Front and Right vectors
        this->m_Up    = glm::normalize(glm::cross(this->m_Right, this->m_Forward));     
    }
private:
    float m_VerticalFOV = 45.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 100.0f;

    glm::mat4 m_Projection{ 1.0f };
    glm::mat4 m_View{ 1.0f };
    glm::mat4 m_InverseProjection{ 1.0f };
    glm::mat4 m_InverseView{ 1.0f };

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};
