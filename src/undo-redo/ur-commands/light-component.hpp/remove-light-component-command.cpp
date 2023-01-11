#include "undo-redo/ur-commands/light-component.hpp/remove-light-component-command.hpp"

namespace TWE {
    RemoveLightComponentCommand::RemoveLightComponentCommand(const Entity& entity): _entity(entity) {}

    void RemoveLightComponentCommand::execute() {
        _entity.removeComponent<LightComponent>();
    }

    void RemoveLightComponentCommand::unExecute() {
        _entity.addComponent<LightComponent>();
    }
}