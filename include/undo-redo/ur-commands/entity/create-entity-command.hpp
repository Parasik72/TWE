#ifndef CREATE_ENTITY_COMMAND_HPP
#define CREATE_ENTITY_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class CreateEntityCommand: public IURCommand {
    public:
        CreateEntityCommand(const Entity& entity, std::function<void()> unselectFunc);
        ~CreateEntityCommand();
        void execute() override;
        void unExecute() override;
    private:
        void copyComponents(Entity& entity);
        void setComponents(Entity& entity);
        TransformComponent* transformComponent = nullptr;
        NameComponent* nameComponent = nullptr;
        IDComponent* idComponent = nullptr;
        CreationTypeComponent* creationTypeComponent = nullptr;
        ParentChildsComponent* parentChildsComponent = nullptr;
        LightComponent* lightComponent = nullptr;
        CameraComponent* cameraComponent = nullptr;
        MeshComponent* meshComponent = nullptr;
        MeshRendererComponent* meshRendererComponent = nullptr;
        bool notExecuteOnce;
        std::function<void()> _unselectFunc;
        Entity _entity;
        entt::entity _sourceEntity;
        Scene* _sourceScene;
    };
}

#endif