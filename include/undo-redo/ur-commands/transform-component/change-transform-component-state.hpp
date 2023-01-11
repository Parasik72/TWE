#ifndef CHANGE_TRANSFORM_COMPONENT_STATE_HPP
#define CHANGE_TRANSFORM_COMPONENT_STATE_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class ChangeTransformComponentState: public IURCommand {
    public:
        ChangeTransformComponentState(const Entity& entity, const TransformComponent& oldState, const TransformComponent& newState);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
        TransformComponent _oldState;
        TransformComponent _newState;
    };
}

#endif