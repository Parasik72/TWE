#include "undo-redo/ur-commands/mesh-renderer-component/change-mesh-rendere-component-state-command.hpp"

namespace TWE {
    ChangeMeshRendererComponentState::ChangeMeshRendererComponentState(const Entity& entity, const MeshRendererComponent& oldState,  const MeshRendererComponent& newState)
    : _entity(entity), _oldState(oldState), _newState(newState) {}

    void ChangeMeshRendererComponentState::execute() {
        auto& meshRendererComponent = _entity.getComponent<MeshRendererComponent>();
        meshRendererComponent.setShader(_newState.shader->getVertPath().c_str(), 
            _newState.shader->getFragPath().c_str(), _newState.registryId);
        meshRendererComponent.setMaterial(_newState.material);
    }

    void ChangeMeshRendererComponentState::unExecute() {
        auto& meshRendererComponent = _entity.getComponent<MeshRendererComponent>();
        meshRendererComponent.setShader(_oldState.shader->getVertPath().c_str(), 
            _oldState.shader->getFragPath().c_str(), _oldState.registryId);
        meshRendererComponent.setMaterial(_oldState.material);
    }
}