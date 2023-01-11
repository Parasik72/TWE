#include "undo-redo/ur-commands/camera-component/change-camera-component-state-command.hpp"

namespace TWE {
    ChangeCameraComponentStateCommand::ChangeCameraComponentStateCommand(const Entity& entity, const CameraComponent& oldsState, const CameraComponent& newState)
    : _entity(entity), _oldState(oldsState), _newState(newState) {}
    
    void ChangeCameraComponentStateCommand::execute()  {
        auto& cameraComponent = _entity.getComponent<CameraComponent>();
        cameraComponent.setSource(*_newState.getSource());
        cameraComponent.setFocuse(_newState.isFocusedOn());
    }

    void ChangeCameraComponentStateCommand::unExecute() {
        auto& cameraComponent = _entity.getComponent<CameraComponent>();
        cameraComponent.setSource(*_oldState.getSource());
        cameraComponent.setFocuse(_oldState.isFocusedOn());
    }
}