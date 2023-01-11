#ifndef ADD_PHYSICS_COMPONENT_COMMAND_HPP
#define ADD_PHYSICS_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class AddPhysicsComponentCommand: public IURCommand {
    public:
        AddPhysicsComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
    };
}

#endif