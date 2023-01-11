#include "scene/components/camera-component.hpp"

namespace TWE {
    CameraComponent::CameraComponent() {
        _isFocusedOn = true;
    }

    CameraComponent::CameraComponent(const CameraComponent& cameraComponent) {
        this->_camera = cameraComponent._camera;
        this->_isFocusedOn = cameraComponent._isFocusedOn;
    }

    CameraComponent::~CameraComponent() {
        _isFocusedOn = false;
    }

    void CameraComponent::setSource(const Camera& source) {
        _camera = source;
    }

    void CameraComponent::setFocuse(bool isFocusedOn) {
        _isFocusedOn = isFocusedOn;
    }

    bool CameraComponent::isFocusedOn() const noexcept { return _isFocusedOn; }
    Camera* CameraComponent::getSource() { return &_camera; }
}