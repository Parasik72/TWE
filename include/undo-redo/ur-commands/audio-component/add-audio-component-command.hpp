#ifndef ADD_AUDIO_COMPONENT_COMMAND_HPP
#define ADD_AUDIO_COMPONENT_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class AddAudioComponentCommand: public IURCommand {
    public:
        AddAudioComponentCommand(const Entity& entity);
        void execute() override;
        void unExecute() override;
    private:
        Entity _entity;
    };
}

#endif