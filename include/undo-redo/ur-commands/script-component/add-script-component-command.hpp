#ifndef ADD_SCRIPT_COMPONENT_COMMAND_HPP
#define ADD_SCRIPT_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class AddScriptComponentCommand: public IURCommand {
    public:
        AddScriptComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
    };
}

#endif