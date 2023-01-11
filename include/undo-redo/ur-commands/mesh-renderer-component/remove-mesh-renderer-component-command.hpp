#ifndef REMOVE_MESH_RENDERER_COMPONENT_COMMAND_HPP
#define REMOVE_MESH_RENDERER_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class RemoveMeshRendererComponentCommand: public IURCommand {
    public:
        RemoveMeshRendererComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        MeshRendererComponent _meshRendererComponent;
    };
}

#endif