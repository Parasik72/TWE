#include "undo-redo/ur-commands/physics-component/remove-physics-component-command.hpp"

namespace TWE {
    RemovePhysicsComponentCommand::RemovePhysicsComponentCommand(const Entity& entity)
    : _entity(entity) {
        _physicsComponent = _entity.getComponent<PhysicsComponent>();
    }

    void RemovePhysicsComponentCommand::execute() {
        auto& physicsComponent = _entity.getComponent<PhysicsComponent>();
        physicsComponent.getDynamicsWorld()->removeRigidBody(physicsComponent.getRigidBody());
        _entity.removeComponent<PhysicsComponent>();
    }

    void RemovePhysicsComponentCommand::unExecute() {
        PhysicsComponent* physicsComponent;
        if(_physicsComponent.getColliderType() != ColliderType::TriangleMesh) {
            auto shapeDimensions = _physicsComponent.getShapeDimensions() / _physicsComponent.getLocalScale();
            physicsComponent = &_entity.addComponent<PhysicsComponent>(_physicsComponent.getDynamicsWorld(), _physicsComponent.getColliderType(),
                shapeDimensions, _physicsComponent.getLocalScale(), _physicsComponent.getPosition(),
                _physicsComponent.getRotation(), _physicsComponent.getMass(), _entity.getSource());
            physicsComponent->setIsRotated(_physicsComponent.getIsRotated());
        } else 
            physicsComponent = &_entity.addComponent<PhysicsComponent>(_physicsComponent.getDynamicsWorld(), _physicsComponent.getColliderType(),
                _physicsComponent.getTriangleMesh(), _physicsComponent.getLocalScale(), _physicsComponent.getPosition(),
                _physicsComponent.getRotation(), _entity.getSource());
    }
}