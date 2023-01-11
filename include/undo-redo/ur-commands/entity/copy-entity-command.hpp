#ifndef COPY_ENTITY_COMMAND_HPP
#define COPY_ENTITY_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class CopyEntityCommand: public IURCommand {
    public:
        CopyEntityCommand(const Entity& entity, const Entity& parentEntity, std::function<void()> unselectFunc);
        void execute() override;
        void unExecute() override;
    private:
        void copyComponents(Entity& entity, Entity& to);
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
        PhysicsComponent* physicsComponent = nullptr;
        ScriptComponent* scriptComponent = nullptr;

        Entity _entity;
        entt::entity _newEntitySource;
        Entity _newEntity;
        Entity _parentEntity;

        std::function<void()> _unselectFunc;
        std::vector<CopyEntityCommand*> _childsCommand;
        Scene* _sourceScene;
        btDynamicsWorld* _dynamicsWorld = nullptr;
        btDynamicsWorld* _tempWorld;
    };
}

#endif