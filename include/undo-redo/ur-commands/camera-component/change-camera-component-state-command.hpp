#ifndef CHANGE_CAMERA_COMPONENT_STATE_COMMAND_HPP
#define CHANGE_CAMERA_COMPONENT_STATE_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class ChangeCameraComponentStateCommand: public IURCommand {
    public:
        ChangeCameraComponentStateCommand(const Entity& entity, const CameraComponent& oldsState, const CameraComponent& newState);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        CameraComponent _oldState;
        CameraComponent _newState;
    };
}

#endif