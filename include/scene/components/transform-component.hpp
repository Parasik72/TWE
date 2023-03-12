#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glad.h>

#include "twe-math/twe-math.hpp"

namespace TWE {
    struct ModelSpecification {
        ModelSpecification() = default;
        ModelSpecification(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size)
            : position(position), rotation(rotation), size(size) {} 
        bool operator==(const ModelSpecification& modelSpecification) const {
            return this->position == modelSpecification.position 
                && this->rotation == modelSpecification.rotation
                && this->size == modelSpecification.size;
        }
        bool operator!=(const ModelSpecification& modelSpecification) const {
            return !(*this == modelSpecification);
        }
        glm::vec3 position = glm::vec3(0.f);
        glm::vec3 rotation = glm::vec3(0.f);
        glm::vec3 size = glm::vec3(1.f);
    };

    class TransformComponent {
    public:
        TransformComponent();
        TransformComponent(const TransformComponent& transform);
        void rotate(float angle, const glm::vec3& axis, bool acceptToChilds = true);
        void rotateAroundOrigin(const glm::vec3& angles, const glm::vec3& centerPosition, bool acceptToChilds = true);
        void move(const glm::vec3& pos, bool acceptToChilds = true);
        void scale(const glm::vec3& size, bool acceptToChilds = true);
        void setPosition(const glm::vec3& pos, bool acceptToChilds = true);
        void setRotation(float angle, const glm::vec3& axis, bool acceptToChilds = true);
        void setRotation(const glm::vec3& angles, bool acceptToChilds = true);
        void setSize(const glm::vec3& size, bool acceptToChilds = true);
        [[nodiscard]] glm::mat4 getModel();
        [[nodiscard]] const ModelSpecification& getTransform() const noexcept;
        [[nodiscard]] const glm::vec3& getPosition() const noexcept;
        [[nodiscard]] const glm::vec3& getRotation() const noexcept;
        [[nodiscard]] const glm::vec3& getSize() const noexcept;
        [[nodiscard]] glm::vec3 getForward();
        [[nodiscard]] glm::vec3 getRight();
        [[nodiscard]] glm::vec3 getUp();
    private:
        void setPreTransform(const ModelSpecification& preTransform);
        [[nodiscard]] glm::vec3 getZeroRotationAroundPos(const glm::vec3& centerPosition);
        [[nodiscard]] const ModelSpecification& getPreTransform() const noexcept;
        bool _needRecache;
        glm::mat4 _model;
        ModelSpecification _transform;
        ModelSpecification _preTransform;
        friend class Scene;
        friend class SceneSerializer;
    };
}

#endif