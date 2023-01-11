#include "undo-redo/ur-commands/mesh-component/change-mesh-component-state-command.hpp"

namespace TWE {
    ChangeMeshComponentStateCommand::ChangeMeshComponentStateCommand(const Entity& entity, const MeshComponent& newState)
    : _entity(entity), _newState(newState) {
        _oldState = _entity.getComponent<MeshComponent>();
    }

    void ChangeMeshComponentStateCommand::execute() {
        auto& meshComponent = _entity.getComponent<MeshComponent>();
        meshComponent.vao = _newState.vao;
        meshComponent.vbo = _newState.vbo;
        meshComponent.ebo = _newState.ebo;
        meshComponent.registryId = _newState.registryId;
        meshComponent.modelPath = _newState.modelPath;
        meshComponent.texture->unbind();
        meshComponent.texture = _newState.texture;
    }

    void ChangeMeshComponentStateCommand::unExecute() {
        auto& meshComponent = _entity.getComponent<MeshComponent>();
        meshComponent.vao = _oldState.vao;
        meshComponent.vbo = _oldState.vbo;
        meshComponent.ebo = _oldState.ebo;
        meshComponent.registryId = _oldState.registryId;
        meshComponent.modelPath = _oldState.modelPath;
        meshComponent.texture->unbind();
        meshComponent.texture = _oldState.texture;
    }
}