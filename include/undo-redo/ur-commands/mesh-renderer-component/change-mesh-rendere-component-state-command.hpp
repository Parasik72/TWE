#ifndef CHANGE_MESH_RENDERER_COMPONENT_STATE_HPP
#define CHANGE_MESH_RENDERER_COMPONENT_STATE_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class ChangeMeshRendererComponentState: public IURCommand {
    public:
        ChangeMeshRendererComponentState(const Entity& entity, const MeshRendererComponent& oldState, const MeshRendererComponent& newState);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        MeshRendererComponent _oldState;
        MeshRendererComponent _newState;
    };
}

#endif