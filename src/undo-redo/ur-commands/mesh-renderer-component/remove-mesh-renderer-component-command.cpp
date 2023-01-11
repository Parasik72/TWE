#include "undo-redo/ur-commands/mesh-renderer-component/remove-mesh-renderer-component-command.hpp"

namespace TWE {
    RemoveMeshRendererComponentCommand::RemoveMeshRendererComponentCommand(const Entity& entity)
    : _entity(entity) {
        _meshRendererComponent = _entity.getComponent<MeshRendererComponent>();
    }

    void RemoveMeshRendererComponentCommand::execute() {
        _entity.removeComponent<MeshRendererComponent>();
    }

    void RemoveMeshRendererComponentCommand::unExecute() {
        auto& meshRendererComponent = _entity.addComponent<MeshRendererComponent>(_meshRendererComponent.shader->getVertPath().c_str(), 
            _meshRendererComponent.shader->getFragPath().c_str(), _meshRendererComponent.entityId, _meshRendererComponent.registryId);
        meshRendererComponent.setMaterial(_meshRendererComponent.material);
    }
}