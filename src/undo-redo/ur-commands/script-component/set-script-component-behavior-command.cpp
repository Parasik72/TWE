#include "undo-redo/ur-commands/script-component/set-script-component-behavior-command.hpp"

namespace TWE {
    SetScriptComponentBehaviorCommand::SetScriptComponentBehaviorCommand(const Entity& entity, const std::string& newBehaviorName)
    : _entity(entity), _newBehaviorName(newBehaviorName) {
        _oldBehaviorName = _entity.getComponent<ScriptComponent>().getBehaviorClassName();
    }
    
    void SetScriptComponentBehaviorCommand::execute() {
        auto& scriptComponent = _entity.getComponent<ScriptComponent>();
        scriptComponent.unbind();
        auto scriptDLLData = _entity.getScene()->getScriptDLLRegistry()->get(_newBehaviorName);
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
        }
    }

    void SetScriptComponentBehaviorCommand::unExecute() {
        auto& scriptComponent = _entity.getComponent<ScriptComponent>();
        scriptComponent.unbind();
        auto scriptDLLData = _entity.getScene()->getScriptDLLRegistry()->get(_oldBehaviorName);
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
        }
    }
}