#include "Camera.hpp"
#include <algorithm>

namespace Minecraft {

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_Position(position), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch)
{
    m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio, float fovDeg, float nearPlane, float farPlane) const {
    return glm::perspective(glm::radians(fovDeg), aspectRatio, nearPlane, farPlane);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = m_MovementSpeed * deltaTime;
    if (direction == FORWARD)
        m_Position += m_Front * velocity;
    if (direction == BACKWARD)
        m_Position -= m_Front * velocity;
    if (direction == LEFT)
        m_Position -= m_Right * velocity;
    if (direction == RIGHT)
        m_Position += m_Right * velocity;
    if (direction == UP)
        m_Position += m_WorldUp * velocity;
    if (direction == DOWN)
        m_Position -= m_WorldUp * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw   += xoffset;
    m_Pitch += yoffset;

    if (constrainPitch) {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}

}
