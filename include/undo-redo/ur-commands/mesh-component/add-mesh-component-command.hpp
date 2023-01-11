#ifndef ADD_MESH_COMPONENT_COMMAND_HPP
#define ADD_MESH_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class AddMeshCompoentnCommand: public IURCommand {
    public:
        AddMeshCompoentnCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        bool meshComponentAdded = false;
        bool meshRendererComponentAdded = false;
        Entity _entity;
    };
}

#endif