#ifndef SET_SCRIPT_COMPONENT_BEHAVIOR_COMMAND_HPP
#define SET_SCRIPT_COMPONENT_BEHAVIOR_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class SetScriptComponentBehaviorCommand: public IURCommand {
    public:
        SetScriptComponentBehaviorCommand(const Entity& entity, const std::string& newBehaviorName);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        std::string _newBehaviorName;
        std::string _oldBehaviorName;
    };
}

#endif