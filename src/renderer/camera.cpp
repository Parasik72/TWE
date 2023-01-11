#include "renderer/camera.hpp"

namespace TWE {
    Camera::Camera(){
        setPerspective(60.f, 16, 9);
        orthographicSpecification = { -5.f, 5.f, -5.f, 5.f, -1.f, 1000.f };
    }

    Camera::Camera(const Camera& camera) {
        this->projection = camera.projection;
        this->type = camera.type;
        this->perspectiveSpecification = camera.perspectiveSpecification;
        this->orthographicSpecification = camera.orthographicSpecification;
    }

    void Camera::setPerspective(float fov, int wndWidth, int wndHeight, float nearDepth, float farDepth) {
        projection = glm::perspective(glm::radians<float>(fov), static_cast<float>(wndWidth) / static_cast<float>(wndHeight), nearDepth, farDepth);
        perspectiveSpecification = { fov, wndWidth, wndHeight, nearDepth, farDepth };
        type = CameraProjectionType::Perspective;
    }

    void Camera::setOrthographic(float left, float right, float bottom, float top, float nearDepth, float farDepth) {
        projection = glm::ortho(left, right, bottom, top, nearDepth, farDepth);
        orthographicSpecification = { left, right, bottom, top, nearDepth, farDepth };
        type = CameraProjectionType::Orthographic;
    }

    glm::mat4 Camera::getView(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up) {
        return glm::lookAt(pos, pos + forward, up);
    }

    bool Camera::operator==(const Camera& camera) {
        return this->projection == camera.projection
            && this->type == camera.type
            && this->perspectiveSpecification == camera.perspectiveSpecification
            && this->orthographicSpecification == camera.orthographicSpecification;
    }

    bool Camera::operator!=(const Camera& camera) {
        return !(*this == camera);
    }

    glm::mat4 Camera::getProjection() noexcept { return projection; }
    const glm::mat4& Camera::getProjection() const noexcept { return projection; }
    CameraProjectionType Camera::getType() const noexcept { return type; };
    PerspectiveSpecification Camera::getPerspectiveSpecification() const noexcept { return perspectiveSpecification; }
    OrthographicSpecification Camera::getOrthographicSpecification() const noexcept { return orthographicSpecification; }
    
    std::vector<std::string> cameraProjectionTypes = {
        "Perspective",
        "Orthographic"
    };
}
