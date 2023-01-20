#ifndef ADD_CAMERA_COMPONENT_COMMAND_HPP
#define ADD_CAMERA_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class AddCameraComponentCommand: public IURCommand {
    public:
        AddCameraComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
    };
};

#endif