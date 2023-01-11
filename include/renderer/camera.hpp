#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <string>

namespace TWE {
    enum CameraProjectionType {
        Perspective,
        Orthographic
    };

    struct PerspectiveSpecification {
        bool operator==(const PerspectiveSpecification& spec) {
            return this->fov == spec.fov
                && this->wndWidth == spec.wndWidth
                && this->wndHeight == spec.wndHeight
                && this->nearDepth == spec.nearDepth
                && this->farDepth == spec.farDepth;
        }
        bool operator!=(const PerspectiveSpecification& spec) {
            return !(*this == spec);
        }
        float fov;
        int wndWidth;
        int wndHeight;
        float nearDepth;
        float farDepth;
    };

    struct OrthographicSpecification {
        bool operator==(const OrthographicSpecification& spec) {
            return this->left == spec.left
                && this->right == spec.right
                && this->bottom == spec.bottom
                && this->top == spec.top
                && this->nearDepth == spec.nearDepth
                && this->farDepth == spec.farDepth;
        }
        bool operator!=(const OrthographicSpecification& spec) {
            return !(*this == spec);
        }
        float left;
        float right;
        float bottom;
        float top;
        float nearDepth;
        float farDepth;
    };

    extern std::vector<std::string> cameraProjectionTypes;

    class Camera {
    public:
        Camera();
        Camera(const Camera& camera);
        void setPerspective(float fov, int wndWidth, int wndHeight, float near = 0.1f, float farDepth = 1000.f);
        void setOrthographic(float left, float right, float bottom, float top, float near = -1.f, float farDepth = 1000.f);
        [[nodiscard]] glm::mat4 getView(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up);
        [[nodiscard]] glm::mat4 getProjection() noexcept;
        [[nodiscard]] const glm::mat4& getProjection() const noexcept;
        [[nodiscard]] CameraProjectionType getType() const noexcept;
        [[nodiscard]] PerspectiveSpecification getPerspectiveSpecification() const noexcept;
        [[nodiscard]] OrthographicSpecification getOrthographicSpecification() const noexcept;
        [[nodiscard]] bool operator==(const Camera& camera);
        [[nodiscard]] bool operator!=(const Camera& camera);
    protected:
        glm::mat4 projection;
        PerspectiveSpecification perspectiveSpecification;
        OrthographicSpecification orthographicSpecification;
        CameraProjectionType type;
    };
}

#endif