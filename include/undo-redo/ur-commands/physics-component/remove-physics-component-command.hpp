#ifndef REMOVE_PHYSICS_COMPONENT_COMMAND_HPP
#define REMOVE_PHYSICS_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class RemovePhysicsComponentCommand: public IURCommand {
    public:
        RemovePhysicsComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        PhysicsComponent _physicsComponent;
    };
}

#endif