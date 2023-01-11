#ifndef DRAG_AND_DROP_ENTITY_COMMAND_HPP
#define DRAG_AND_DROP_ENTITY_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class DragAndDropEntityCommand: public IURCommand {
    public:
        DragAndDropEntityCommand(const Entity& entity, const Entity& to);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        Entity _to;
        Entity _preParent;
    };
}

#endif