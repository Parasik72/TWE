#include "undo-redo/ur-commands/physics-component/change-physics-component-properties-command.hpp"

namespace TWE {
    ChangePhysicsComponentPropertiesCommand::ChangePhysicsComponentPropertiesCommand(const Entity& entity, 
    const PhysicsComponentProperties& oldProperties, const PhysicsComponentProperties& newProperties)
    : _entity(entity), _oldProperties(oldProperties), _newProperties(newProperties) {}

    void ChangePhysicsComponentPropertiesCommand::execute() {
        auto& physicsComponent = _entity.getComponent<PhysicsComponent>();
        physicsComponent.setMass(_newProperties.mass);
        physicsComponent.setIsRotated(_newProperties.isRotated);
        physicsComponent.setShapeDimensions(_newProperties.shapeDimensions);
    }

    void ChangePhysicsComponentPropertiesCommand::unExecute() {
        auto& physicsComponent = _entity.getComponent<PhysicsComponent>();
        physicsComponent.setMass(_oldProperties.mass);
        physicsComponent.setIsRotated(_oldProperties.isRotated);
        physicsComponent.setShapeDimensions(_oldProperties.shapeDimensions);
    }
}