#include "scene/components/script-component.hpp"

namespace TWE {
    ScriptComponent::ScriptComponent(): _instance(nullptr), _isInitialized(false), isEnabled(true) {}

    ScriptComponent::ScriptComponent(const ScriptComponent& scriptComponent) {
        this->isEnabled = scriptComponent.isEnabled;
        this->_behaviorClassName = scriptComponent._behaviorClassName;
        this->_instance = scriptComponent._instance;
        this->_isInitialized = scriptComponent._isInitialized;
    }
}