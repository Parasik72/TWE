#ifndef REMOVE_SCRIPT_COMPONENT_COMMAND_HPP
#define REMOVE_SCRIPT_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class RemoveScriptComponentCommand: public IURCommand  {
    public:
        RemoveScriptComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        ScriptComponent _scriptComponent;
    };
}

#endif