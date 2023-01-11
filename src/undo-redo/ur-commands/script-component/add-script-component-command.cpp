#include "undo-redo/ur-commands/script-component/add-script-component-command.hpp"

namespace TWE {
    AddScriptComponentCommand::AddScriptComponentCommand(const Entity& entity): _entity(entity) {}

    void AddScriptComponentCommand::execute() {
        _entity.addComponent<ScriptComponent>().bind<Behavior>();
    }

    void AddScriptComponentCommand::unExecute() {
        _entity.getComponent<ScriptComponent>().unbind();
        _entity.removeComponent<ScriptComponent>();
    }
}