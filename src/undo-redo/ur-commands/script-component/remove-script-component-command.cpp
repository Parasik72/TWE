#include "undo-redo/ur-commands/script-component/remove-script-component-command.hpp"

namespace TWE {
    RemoveScriptComponentCommand::RemoveScriptComponentCommand(const Entity& entity)
    : _entity(entity) {
        _scriptComponent = _entity.getComponent<ScriptComponent>();
    }

    void RemoveScriptComponentCommand::execute() {
        _entity.getComponent<ScriptComponent>().unbind();
        _entity.removeComponent<ScriptComponent>();
    }

    void RemoveScriptComponentCommand::unExecute() {
        auto& scriptComponent = _entity.addComponent<ScriptComponent>();
        auto scriptDLLData = _entity.getScene()->getScriptDLLRegistry()->get(_scriptComponent.getBehaviorClassName());
        if(!scriptDLLData || !scriptDLLData->isValid)
            scriptComponent.bind<Behavior>();
        else {
            auto behaviorFactory = DLLCreator::loadDLLFunc(*scriptDLLData);
            if(!behaviorFactory)
                scriptComponent.bind<Behavior>();
            else {
                Behavior* behavior = (Behavior*)behaviorFactory();
                scriptComponent.bind(behavior, scriptDLLData->scriptName);
            }
            scriptComponent.isEnabled = _scriptComponent.isEnabled;
        }
    }
}