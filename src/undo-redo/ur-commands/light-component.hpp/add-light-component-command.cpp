#include "undo-redo/ur-commands/light-component.hpp/add-light-component-command.hpp"

namespace TWE {
    AddLightComponentCommand::AddLightComponentCommand(const Entity& entity): _entity(entity) {}
    
    void AddLightComponentCommand::execute() {
        _entity.addComponent<LightComponent>();
    }

    void AddLightComponentCommand::unExecute() {
        _entity.removeComponent<LightComponent>();
    }
}