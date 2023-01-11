#ifndef ADD_LIGHT_COMPONENT_COMMAND_HPP
#define ADD_LIGHT_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class AddLightComponentCommand: public IURCommand {
    public:
        AddLightComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
    };
}

#endif