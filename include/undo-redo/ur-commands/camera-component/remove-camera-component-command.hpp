#ifndef REMOVE_CAMERA_COMPONENT_COMMAND_HPP
#define REMOVE_CAMERA_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class RemoveCameraComponentCommand: public IURCommand {
    public:
        RemoveCameraComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        CameraComponent cameraComponent;
        Entity _entity;
    };
}

#endif