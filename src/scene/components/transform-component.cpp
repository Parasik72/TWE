#include "scene/components/transform-component.hpp"

namespace TWE {
    TransformComponent::TransformComponent(): needRecache(true) {}

    TransformComponent::TransformComponent(const TransformComponent& transform) {
        this->transform = transform.transform;
        this->preTransform = transform.preTransform;
        this->model = transform.model;
        this->needRecache = transform.needRecache;
    }

    void TransformComponent::move(const glm::vec3& pos, bool acceptToChilds) {
        auto model = glm::translate(getModel(), pos / transform.size);
        transform.position = model[3];
        if(!acceptToChilds)
            preTransform.position = transform.position;
        needRecache = true;
    }

    void TransformComponent::rotate(float angle, const glm::vec3& axis, bool acceptToChilds) {
        setRotation({ transform.rotation.x + (axis.x * (angle * PI / 180.f)), 
                      transform.rotation.y + (axis.y * (angle * PI / 180.f)),
                      transform.rotation.z + (axis.z * (angle * PI / 180.f)) }, acceptToChilds);
    }

    void TransformComponent::scale(const glm::vec3& size, bool acceptToChilds) {
        transform.size *= size;
        if(!acceptToChilds)
            preTransform.size = transform.size;
        needRecache = true;
    }

    void TransformComponent::setPosition(const glm::vec3& pos, bool acceptToChilds) {
        transform.position = pos;
        if(!acceptToChilds)
            preTransform.position = transform.position;
        needRecache = true;
    }

    void TransformComponent::setRotation(float angle, const glm::vec3& axis, bool acceptToChilds) {
        transform.rotation = glm::eulerAngles(glm::angleAxis(angle, axis)) * PI / 180.f;
        if(!acceptToChilds)
            preTransform.rotation = transform.rotation;
        needRecache = true;
    }

    void TransformComponent::setRotation(const glm::vec3& angles, bool acceptToChilds) {
        transform.rotation = angles;
        if(!acceptToChilds)
            preTransform.rotation = transform.rotation;
        needRecache = true;
    }

    void TransformComponent::rotateAroundOrigin(const glm::vec3& angles, const glm::vec3& centerPosition, bool acceptToChilds) {
        glm::vec3 zeroPos = getZeroRotationAroundPos(centerPosition);
        this->transform.rotation += angles;
        auto translate1 = glm::translate(glm::mat4(1.f), -centerPosition);
        auto rotation = glm::toMat4(glm::quat(this->transform.rotation));
        auto translate2 = glm::translate(glm::mat4(1.f), centerPosition);
        auto transform = translate2 * rotation * translate1;
        this->transform.position = transform * glm::vec4(zeroPos, 1.f);
        if(!acceptToChilds) {
            preTransform.rotation = this->transform.rotation;
            preTransform.position = this->transform.position;
        }
        needRecache = true;
    }

    glm::vec3 TransformComponent::getZeroRotationAroundPos(const glm::vec3& centerPosition) {
        auto translate1 = glm::translate(glm::mat4(1.f), -centerPosition);
        auto rotation = glm::inverse(glm::toMat4(glm::quat(this->transform.rotation)));
        auto translate2 = glm::translate(glm::mat4(1.f), centerPosition);
        auto transform = translate2 * rotation * translate1;
        return transform * glm::vec4(this->transform.position, 1.f);
    }

    void TransformComponent::setSize(const glm::vec3& size, bool acceptToChilds) {
        transform.size = size;
        if(!acceptToChilds)
            preTransform.size = transform.size;
        needRecache = true;
    }

    glm::mat4 TransformComponent::getModel() {
        if(needRecache) {
            model = glm::translate(glm::mat4(1.f), transform.position)
                  * glm::toMat4(glm::quat(transform.rotation))
                  * glm::scale(glm::mat4(1.f), transform.size);
            needRecache = false;
        }
        return model;
    }

    glm::vec3 TransformComponent::getForward() { return glm::normalize(glm::vec3(getModel()[2])); }
    glm::vec3 TransformComponent::getRight() { return glm::normalize(glm::vec3(getModel()[0])); }
    glm::vec3 TransformComponent::getUp() { return glm::normalize(glm::vec3(getModel()[1])); }
}