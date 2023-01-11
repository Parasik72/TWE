#ifndef CHANGE_NAME_COMPONENT_STATE_COMMAND_HPP
#define CHANGE_NAME_COMPONENT_STATE_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class ChangeNameComponentStateCommand: public IURCommand {
    public:
        ChangeNameComponentStateCommand(const Entity& entity, const NameComponent& newState);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        NameComponent _oldState;
        NameComponent _newState;
    };
}

#endif