#include "renderer/debug-camera.hpp"

namespace TWE {
    DebugCamera::DebugCamera(const glm::vec3& pos, float sensitivity, float speed)
    : sensitivity(sensitivity), speed(speed), pos(glm::vec3(0.f)) {
        forward = glm::vec3(0.f, 0.f, 1.f);
        worldUp = glm::vec3(0.f, 1.f, 0.f);
        pitch = 0.f;
        yaw = -90.f;
        boost = 1.f;
        updateState();
    }

    void DebugCamera::keyInput(CamMovement dir, float deltaTime) {
        GLfloat camSpeed = speed * boost * deltaTime;
        if(dir == CamMovement::FORWARD)
            pos += forward * camSpeed;
        if(dir == CamMovement::BACKWARD)
            pos -= forward * camSpeed;
        if(dir == CamMovement::RIGHT)
            pos += right * camSpeed;
        if(dir == CamMovement::LEFT)
            pos -= right * camSpeed;
        boost = dir == CamMovement::L_SHIFT ? 2.f : 1.f;
    }

    void DebugCamera::updateState() {
        glm::vec3 forwardTemp;
        forwardTemp.x = cos(glm::radians<float>(pitch)) * cos(glm::radians<float>(yaw));
        forwardTemp.y = sin(glm::radians<float>(pitch));
        forwardTemp.z = cos(glm::radians<float>(pitch)) * sin(glm::radians<float>(yaw));
        forward = glm::normalize(forwardTemp);
        right = glm::normalize(glm::cross(forward, worldUp));
        up = glm::normalize(glm::cross(right, forward));
    }

    void DebugCamera::mouseInput(float xOffset, float yOffset) {
        xOffset *= sensitivity;
        yOffset *= sensitivity;
        yaw += xOffset;
        pitch += yOffset;
        if(pitch > 89.9f)
            pitch = 89.9f;
        else if(pitch < -89.9f)
            pitch = -89.9f;
        updateState();
    }

    void DebugCamera::setPosition(const glm::vec3& pos) {
        this->pos = pos;
    }

    glm::vec3 DebugCamera::getForward() const noexcept { return forward; }
    glm::vec3 DebugCamera::getRight() const noexcept { return right; }
    glm::vec3 DebugCamera::getUp() const noexcept { return up; }

    glm::vec3 DebugCamera::getPosition() const noexcept { return pos; }
}