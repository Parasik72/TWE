#include "undo-redo/ur-commands/entity/copy-entity-command.hpp"

namespace TWE {
    CopyEntityCommand::CopyEntityCommand(const Entity& entity, const Entity& parentEntity, std::function<void()> unselectFunc)
    : _entity(entity), _parentEntity(parentEntity), _unselectFunc(unselectFunc) {
        _sourceScene = _entity.getScene();
        _newEntitySource = _sourceScene->getRegistry()->create();
        _newEntity = { _newEntitySource, _sourceScene };
        _sourceScene->getSceneStateSpecification()->entityRegistry.emplace<EmptyComponent>(_newEntitySource);

        auto collisionConfig = new btDefaultCollisionConfiguration();
        auto dispatcher = new btCollisionDispatcher(collisionConfig);
        auto broadPhase = new btDbvtBroadphase();
        auto solver = new btSequentialImpulseConstraintSolver();
        _tempWorld = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfig);

        if(_entity.hasComponent<PhysicsComponent>())
            _dynamicsWorld = _entity.getComponent<PhysicsComponent>().getDynamicsWorld();
        copyComponents(_entity, _newEntity);
        auto& childs = _entity.getComponent<ParentChildsComponent>().childs;
        for(auto child : childs) {
            Entity childEntity = { child, _sourceScene };
            _childsCommand.push_back(new CopyEntityCommand(childEntity, _newEntity, unselectFunc));
        }
    }

    void CopyEntityCommand::execute() {
        _newEntity = { _newEntitySource, _sourceScene };
        setComponents(_newEntity);
        for(auto command : _childsCommand)
            command->execute();
    }

    void CopyEntityCommand::unExecute() {
        _unselectFunc();
        for(auto command : _childsCommand)
            command->unExecute();
        _newEntity.destroy();
    }

    void CopyEntityCommand::copyComponents(Entity& entity, Entity& to) {
        auto dynamicsWorld = _tempWorld;
        transformComponent = new TransformComponent(entity.getComponent<TransformComponent>());
        nameComponent = new NameComponent(entity.getComponent<NameComponent>());
        idComponent = new IDComponent(_sourceScene->getSceneStateSpecification()->lastId++);
        creationTypeComponent = new CreationTypeComponent(entity.getComponent<CreationTypeComponent>());
        parentChildsComponent = new ParentChildsComponent();
        parentChildsComponent->parent = _parentEntity.getSource();
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
                meshRendererComponentCopy.shader->getFragPath().c_str(), (int)to.getSource(), meshRendererComponentCopy.registryId);
            meshRendererComponent->setMaterial(meshRendererComponentCopy.material);
        }
        if(entity.hasComponent<PhysicsComponent>()) {
            auto& physicsComponentCopy = entity.getComponent<PhysicsComponent>();
            if(physicsComponentCopy.getColliderType() != ColliderType::TriangleMesh) {
                auto shapeDimensions = physicsComponentCopy.getShapeDimensions() / physicsComponentCopy.getLocalScale();
                physicsComponent = new PhysicsComponent(dynamicsWorld, physicsComponentCopy.getColliderType(), 
                    shapeDimensions, physicsComponentCopy.getLocalScale(), physicsComponentCopy.getPosition(),
                    physicsComponentCopy.getRotation(), physicsComponentCopy.getMass(), to.getSource());
                physicsComponent->setIsRotated(physicsComponentCopy.getIsRotated());
            } else {
                physicsComponent = new PhysicsComponent(dynamicsWorld, physicsComponentCopy.getColliderType(), 
                    physicsComponentCopy.getTriangleMesh(), physicsComponentCopy.getLocalScale(), physicsComponentCopy.getPosition(),
                    physicsComponentCopy.getRotation(), to.getSource());
            }
        }
        if(entity.hasComponent<ScriptComponent>()) {
            scriptComponent = new ScriptComponent();
            auto scriptDLLData = _sourceScene->getScriptDLLRegistry()->get(entity.getComponent<ScriptComponent>().getBehaviorClassName());
            bool isEnabled = false;
            if(!scriptDLLData || !scriptDLLData->isValid)
                scriptComponent->bind<Behavior>();
            else {
                auto behaviorFactory = DLLCreator::loadDLLFunc(*scriptDLLData);
                if(!behaviorFactory)
                    scriptComponent->bind<Behavior>();
                else {
                    Behavior* behavior = (Behavior*)behaviorFactory();
                    scriptComponent->bind(behavior, scriptDLLData->scriptName);
                }
                isEnabled = entity.getComponent<ScriptComponent>().isEnabled;
            }
            scriptComponent->unbind();
            scriptComponent->isEnabled = isEnabled;
        }
    }

    void CopyEntityCommand::setComponents(Entity& entity) {
        entity.addComponent<TransformComponent>(*transformComponent);
        entity.addComponent<NameComponent>(*nameComponent);
        entity.addComponent<IDComponent>(*idComponent);
        entity.addComponent<CreationTypeComponent>(*creationTypeComponent);
        entity.addComponent<ParentChildsComponent>(*parentChildsComponent);
        if(parentChildsComponent->parent != entt::null) {
            Entity parentEntity = { parentChildsComponent->parent, _sourceScene };
            auto& childs = parentEntity.getComponent<ParentChildsComponent>().childs;
            auto& itChild = std::find_if(childs.begin(), childs.end(), [&](entt::entity entityChild) {
                return entityChild == entity.getSource();
            });
            if(itChild == childs.end())
                childs.push_back(entity.getSource());
        }
        if(lightComponent)
            entity.addComponent<LightComponent>(*lightComponent);
        if(cameraComponent)
            entity.addComponent<CameraComponent>(*cameraComponent);
        if(meshComponent)
            entity.addComponent<MeshComponent>(*meshComponent);
        if(meshRendererComponent)
            entity.addComponent<MeshRendererComponent>(*meshRendererComponent);
        if(physicsComponent) {
            if(physicsComponent->getColliderType() != ColliderType::TriangleMesh) {
                auto shapeDimensions = physicsComponent->getShapeDimensions() / physicsComponent->getLocalScale();
                auto physicsComponentNew = entity.addComponent<PhysicsComponent>(_dynamicsWorld, physicsComponent->getColliderType(), 
                    shapeDimensions, physicsComponent->getLocalScale(), physicsComponent->getPosition(),
                    physicsComponent->getRotation(), physicsComponent->getMass(), entity.getSource());
                physicsComponentNew.setIsRotated(physicsComponent->getIsRotated());
            } else {
                physicsComponent = new PhysicsComponent(_dynamicsWorld, physicsComponent->getColliderType(), 
                    physicsComponent->getTriangleMesh(), physicsComponent->getLocalScale(), physicsComponent->getPosition(),
                    physicsComponent->getRotation(), entity.getSource());
            }
        }
        if(scriptComponent) {
            auto scriptDLLData = _sourceScene->getScriptDLLRegistry()->get(scriptComponent->getBehaviorClassName());
            if(!scriptDLLData || !scriptDLLData->isValid)
                entity.addComponent<ScriptComponent>().bind<Behavior>();
            else {
                auto behaviorFactory = DLLCreator::loadDLLFunc(*scriptDLLData);
                if(!behaviorFactory)
                    entity.addComponent<ScriptComponent>().bind<Behavior>();
                else {
                    Behavior* behavior = (Behavior*)behaviorFactory();
                    entity.addComponent<ScriptComponent>().bind(behavior, scriptDLLData->scriptName);
                }
                scriptComponent->isEnabled = entity.getComponent<ScriptComponent>().isEnabled;
            }
        }
    }
}