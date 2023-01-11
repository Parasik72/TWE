#include "undo-redo/ur-commands/transform-component/change-transform-component-state.hpp"

namespace TWE {
    ChangeTransformComponentState::ChangeTransformComponentState(const Entity& entity, const TransformComponent& oldState, 
    const TransformComponent& newState): _entity(entity), _oldState(oldState), _newState(newState) {}

    void ChangeTransformComponentState::execute() {
        auto& transformComponent = _entity.getComponent<TransformComponent>();
        transformComponent.setPosition(_newState.transform.position);
        transformComponent.setRotation(_newState.transform.rotation);
        transformComponent.setSize(_newState.transform.size);
    }

    void ChangeTransformComponentState::unExecute() {
        auto& transformComponent = _entity.getComponent<TransformComponent>();
        transformComponent.setPosition(_oldState.transform.position);
        transformComponent.setRotation(_oldState.transform.rotation);
        transformComponent.setSize(_oldState.transform.size);
    }
}