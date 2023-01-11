#ifndef SET_COLLIDER_TYPE_PHYSICS_COMPONENT_COMMAND_HPP
#define SET_COLLIDER_TYPE_PHYSICS_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class SetColliderTypePhysicsComponentCommand: public IURCommand {
    public:
        SetColliderTypePhysicsComponentCommand(const Entity& entity, ColliderType newColliderType,
            ColliderType oldColliderType, const glm::vec3& localScale);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        ColliderType _oldColliderType;
        ColliderType _newColliderType;
        glm::vec3 _localScale;
    };
}

#endif