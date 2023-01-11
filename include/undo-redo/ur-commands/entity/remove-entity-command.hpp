#ifndef REMOVE_ENTITY_COMMAND_HPP
#define REMOVE_ENTITY_COMMAND_HPP

#include "scene/scene.hpp"
#include "undo-redo/iur-command.hpp"

namespace TWE {
    class RemoveEntityCommand: public IURCommand {
    public:
        RemoveEntityCommand(const Entity& entity, std::function<void()> unselectFunc);
        ~RemoveEntityCommand();
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
        PhysicsComponent* physicsComponent = nullptr;
        ScriptComponent* scriptComponent = nullptr;

        std::vector<RemoveEntityCommand*> _childsCommand;
        std::function<void()> _unselectFunc;
        Entity _entity;
        entt::entity _sourceEntity;
        Scene* _sourceScene;
        btDynamicsWorld* _dynamicsWorld = nullptr;
        btDynamicsWorld* _tempWorld;
    };
}

#endif