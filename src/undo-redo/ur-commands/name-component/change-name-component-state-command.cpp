#include "undo-redo/ur-commands/name-component/change-name-component-state-command.hpp"

namespace TWE {
    ChangeNameComponentStateCommand::ChangeNameComponentStateCommand(const Entity& entity, const NameComponent& newState)
    : _entity(entity), _newState(newState) {
        _oldState = _entity.getComponent<NameComponent>();
    }

    void ChangeNameComponentStateCommand::execute() {
        auto& nameComponent = _entity.getComponent<NameComponent>();
        nameComponent.setName(_newState.getName());
    }

    void ChangeNameComponentStateCommand::unExecute() {
        auto& nameComponent = _entity.getComponent<NameComponent>();
        nameComponent.setName(_oldState.getName());
    }
}