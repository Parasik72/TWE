#ifndef SCRIPT_COMPONENT_HPP
#define SCRIPT_COMPONENT_HPP

#include <string>

#include "entity/behavior.hpp"

namespace TWE {
    class ScriptComponent {
    public:
        ScriptComponent();
        ScriptComponent(const ScriptComponent& scriptComponent);
        template<typename T>
        void bind();
        void unbind() {
            if(_instance) {
                delete _instance;
                _instance = nullptr;
            }
            _isInitialized = false;
            isEnabled = false;
        }
        void bind(Behavior* behavior, const std::string& behaviorClassName) {
            _behaviorClassName = behaviorClassName;
            _instance = behavior;
            isEnabled = true;
        }
        void initialize(entt::entity entity, Scene* scene) {
            _instance->gameObject = { entity, scene };
            _instance->setInput(Input::keyboardPressedKeys, Input::mousePressedButtons, Input::mouseOffset, 
                Input::keyboardPressedActions, Input::mousePressedActions);
            _isInitialized = true;
        }
        [[nodiscard]] std::string getBehaviorClassName() const noexcept {
            return _behaviorClassName;
        }
        bool isEnabled;
    private:
        bool _isInitialized;
        Behavior* _instance;
        std::string _behaviorClassName;
        friend class Scene;
    };

    template<typename T>
    void ScriptComponent::bind() {
        _behaviorClassName = std::string(typeid(T).name()).substr(6);
        _instance = static_cast<Behavior*>(new T);
        isEnabled = true;
    }
}

#endif