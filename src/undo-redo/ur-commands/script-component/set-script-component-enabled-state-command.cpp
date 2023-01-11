#include "undo-redo/ur-commands/script-component/set-script-component-enabled-state-command.hpp"

namespace TWE {
    SetScriptComponentEnabledStateCommand::SetScriptComponentEnabledStateCommand(const Entity& entity, bool newState)
    : _entity(entity), _newState(newState) {
        _oldState = _entity.getComponent<ScriptComponent>().isEnabled;
    }

    void SetScriptComponentEnabledStateCommand::execute() {
        auto& scriptComponent = _entity.getComponent<ScriptComponent>();
        scriptComponent.isEnabled = _newState;
    }

    void SetScriptComponentEnabledStateCommand::unExecute() {
        auto& scriptComponent = _entity.getComponent<ScriptComponent>();
        scriptComponent.isEnabled = _oldState;
    }
}