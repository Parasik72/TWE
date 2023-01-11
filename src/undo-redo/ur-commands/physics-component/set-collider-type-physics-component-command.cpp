#include "undo-redo/ur-commands/physics-component/set-collider-type-physics-component-command.hpp"

namespace TWE {
    SetColliderTypePhysicsComponentCommand::SetColliderTypePhysicsComponentCommand(const Entity& entity, ColliderType newColliderType,
    ColliderType oldColliderType, const glm::vec3& localScale): _entity(entity), _newColliderType(newColliderType)
    , _oldColliderType(oldColliderType), _localScale(localScale) {}

    void SetColliderTypePhysicsComponentCommand::execute() {
        auto& physicsComponent = _entity.getComponent<PhysicsComponent>();
        if(_newColliderType == ColliderType::TriangleMesh) {
            if(_entity.hasComponent<MeshComponent>()) {
                auto meshSpecification = Shape::meshRegistry->get(_entity.getComponent<MeshComponent>().registryId);
                physicsComponent.setColliderType(_newColliderType, TriangleMeshSpecification{meshSpecification->vbo, meshSpecification->ebo});
                physicsComponent.setSize(_localScale);
            }
        } else
            physicsComponent.setColliderType(_newColliderType);
    }

    void SetColliderTypePhysicsComponentCommand::unExecute() {
        auto& physicsComponent = _entity.getComponent<PhysicsComponent>();
        if(_oldColliderType == ColliderType::TriangleMesh) {
            if(_entity.hasComponent<MeshComponent>()) {
                auto meshSpecification = Shape::meshRegistry->get(_entity.getComponent<MeshComponent>().registryId);
                physicsComponent.setColliderType(_oldColliderType, TriangleMeshSpecification{meshSpecification->vbo, meshSpecification->ebo});
                physicsComponent.setSize(_localScale);
            }
        } else
            physicsComponent.setColliderType(_oldColliderType);
    }
}