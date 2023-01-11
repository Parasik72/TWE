#ifndef CHANGE_LIGHT_COMPONENT_STATE_COMMAND_HPP
#define CHANGE_LIGHT_COMPONENT_STATE_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class ChangeLightComponentStateCommand: public IURCommand  {
    public:
        ChangeLightComponentStateCommand(const Entity& entity, const LightComponent& oldState, const LightComponent& newState);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        LightComponent _oldState;
        LightComponent _newState;
    };
}

#endif