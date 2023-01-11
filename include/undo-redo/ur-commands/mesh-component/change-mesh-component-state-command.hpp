#ifndef CHANGE_MESH_COMPONENT_STATE_COMMAND_HPP
#define CHANGE_MESH_COMPONENT_STATE_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class ChangeMeshComponentStateCommand: public IURCommand {
    public:
        ChangeMeshComponentStateCommand(const Entity& entity, const MeshComponent& newState);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        MeshComponent _oldState;
        MeshComponent _newState;
    };
}

#endif