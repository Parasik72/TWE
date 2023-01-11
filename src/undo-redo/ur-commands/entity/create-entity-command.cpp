#include "undo-redo/ur-commands/entity/create-entity-command.hpp"

namespace TWE {
    CreateEntityCommand::CreateEntityCommand(const Entity& entity, std::function<void()> unselectFunc)
    : _entity(entity), _unselectFunc(unselectFunc) {
        _sourceEntity = _entity.getSource();
        _sourceScene = _entity.getScene();
        copyComponents(_entity);
        notExecuteOnce = true;
    }

    CreateEntityCommand::~CreateEntityCommand() {
        delete transformComponent;
        delete nameComponent;
        delete idComponent;
        delete creationTypeComponent;
        delete parentChildsComponent;
        if(lightComponent)
            delete lightComponent;
        if(cameraComponent)
            delete cameraComponent;
        if(meshComponent)
            delete meshComponent;
        if(meshRendererComponent)
            delete meshRendererComponent;
    }

    void CreateEntityCommand::execute() {
        if(notExecuteOnce) {
            notExecuteOnce = false;
            return;
        }
        _entity = { _sourceEntity, _sourceScene };
        setComponents(_entity);
    }

    void CreateEntityCommand::unExecute() {
        _unselectFunc();
        _entity.destroy();
    }

    void CreateEntityCommand::copyComponents(Entity& entity) {
        transformComponent = new TransformComponent(entity.getComponent<TransformComponent>());
        nameComponent = new NameComponent(entity.getComponent<NameComponent>());
        idComponent = new IDComponent(entity.getComponent<IDComponent>());
        creationTypeComponent = new CreationTypeComponent(entity.getComponent<CreationTypeComponent>());
        parentChildsComponent = new ParentChildsComponent(entity.getComponent<ParentChildsComponent>());
        if(entity.hasComponent<LightComponent>())
            lightComponent = new LightComponent(entity.getComponent<LightComponent>());
        if(entity.hasComponent<CameraComponent>())
            cameraComponent = new CameraComponent(entity.getComponent<CameraComponent>());
        if(entity.hasComponent<MeshComponent>()) {
            auto& meshComponentCopy = entity.getComponent<MeshComponent>();
            meshComponent = new MeshComponent(meshComponentCopy.vao, meshComponentCopy.vbo, meshComponentCopy.ebo, 
                meshComponentCopy.registryId, meshComponentCopy.texture->getAttachments());
        }
        if(entity.hasComponent<MeshRendererComponent>()) {
            auto& meshRendererComponentCopy = entity.getComponent<MeshRendererComponent>();
            meshRendererComponent = new MeshRendererComponent(meshRendererComponentCopy.shader->getVertPath().c_str(),
                meshRendererComponentCopy.shader->getFragPath().c_str(), (int)entity.getSource(), meshRendererComponentCopy.registryId);
            meshRendererComponent->setMaterial(meshRendererComponentCopy.material);
        }
    }

    void CreateEntityCommand::setComponents(Entity& entity) {
        entity.addComponent<TransformComponent>(*transformComponent);
        entity.addComponent<NameComponent>(*nameComponent);
        entity.addComponent<IDComponent>(*idComponent);
        entity.addComponent<CreationTypeComponent>(*creationTypeComponent);
        entity.addComponent<ParentChildsComponent>(*parentChildsComponent);
        if(parentChildsComponent->parent != entt::null) {
            Entity parentEntity = { parentChildsComponent->parent, _sourceScene };
            parentEntity.getComponent<ParentChildsComponent>().childs.push_back(entity.getSource());
        }
        if(lightComponent)
            entity.addComponent<LightComponent>(*lightComponent);
        if(cameraComponent)
            entity.addComponent<CameraComponent>(*cameraComponent);
        if(meshComponent)
            entity.addComponent<MeshComponent>(*meshComponent);
        if(meshRendererComponent)
            entity.addComponent<MeshRendererComponent>(*meshRendererComponent);
    }
}