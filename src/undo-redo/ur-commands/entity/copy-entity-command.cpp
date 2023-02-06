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
                meshComponentCopy.registryId, meshComponentCopy.modelSpec, meshComponentCopy.texture);
        }
        if(entity.hasComponent<MeshRendererComponent>()) {
            auto& meshRendererComponentCopy = entity.getComponent<MeshRendererComponent>();
            meshRendererComponent = new MeshRendererComponent(meshRendererComponentCopy.shader->getVertPath().c_str(),
                meshRendererComponentCopy.shader->getFragPath().c_str(), (int)to.getSource(), meshRendererComponentCopy.registryId);
            meshRendererComponent->setMaterial(meshRendererComponentCopy.material);
            meshRendererComponent->is3D = meshRendererComponentCopy.is3D;
        }
        if(entity.hasComponent<PhysicsComponent>()) {
            auto& physicsComponentCopy = entity.getComponent<PhysicsComponent>();
            if(physicsComponentCopy.getColliderType() != ColliderType::TriangleMesh) {
                physicsComponent = new PhysicsComponent(dynamicsWorld, physicsComponentCopy.getColliderType(), 
                    physicsComponentCopy.getShapeDimensions(), physicsComponentCopy.getLocalScale(), physicsComponentCopy.getPosition(),
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
            scriptComponent->getScripts() = entity.getComponent<ScriptComponent>().getScripts();
        }
        if(entity.hasComponent<AudioComponent>()) {
            auto& audioComponentCopy = entity.getComponent<AudioComponent>();
            audioComponent = new AudioComponent(_sourceScene->getSceneAudio()->getSoundEngine());
            auto& soundSources = audioComponentCopy.getSoundSources();
            for(auto soundSource : soundSources) {
                auto newSoundSource = audioComponent->addSoundSource(soundSource->getSoundSourcePath(), soundSource->getIs3D());
                newSoundSource->setPlayLooped(soundSource->getPlayLooped());
                newSoundSource->setStartPaused(soundSource->getStartPaused());
                newSoundSource->setVolume(soundSource->getVolume());
                newSoundSource->setMinDistance(soundSource->getMinDistance());
                newSoundSource->setMaxDistance(soundSource->getMaxDistance());
                newSoundSource->setPlaybackSpeed(soundSource->getPlaybackSpeed());
            }
        }
    }

    void CopyEntityCommand::setComponents(Entity& entity) {
        if(!entity.hasComponent<TransformComponent>())
            entity.addComponent<TransformComponent>(*transformComponent);
        if(!entity.hasComponent<NameComponent>())
            entity.addComponent<NameComponent>(*nameComponent);
        if(!entity.hasComponent<IDComponent>())
            entity.addComponent<IDComponent>(*idComponent);
        if(!entity.hasComponent<CreationTypeComponent>())
            entity.addComponent<CreationTypeComponent>(*creationTypeComponent);
        if(!entity.hasComponent<ParentChildsComponent>())
            entity.addComponent<ParentChildsComponent>(*parentChildsComponent);
        if(parentChildsComponent->parent != entt::null) {
            Entity parentEntity = { parentChildsComponent->parent, _sourceScene };
            if(parentEntity.hasComponent<ParentChildsComponent>()) {
                auto& childs = parentEntity.getComponent<ParentChildsComponent>().childs;
                auto& itChild = std::find_if(childs.begin(), childs.end(), [&](entt::entity entityChild) {
                    return entityChild == entity.getSource();
                });
                if(itChild == childs.end())
                    childs.push_back(entity.getSource());
            }
        }
        if(lightComponent && !entity.hasComponent<LightComponent>())
            entity.addComponent<LightComponent>(*lightComponent);
        if(cameraComponent && !entity.hasComponent<CameraComponent>())
            entity.addComponent<CameraComponent>(*cameraComponent);
        if(meshComponent && !entity.hasComponent<MeshComponent>())
            entity.addComponent<MeshComponent>(*meshComponent);
        if(meshRendererComponent && !entity.hasComponent<MeshRendererComponent>())
            entity.addComponent<MeshRendererComponent>(*meshRendererComponent);
        if(physicsComponent && !entity.hasComponent<PhysicsComponent>()) {
            if(physicsComponent->getColliderType() != ColliderType::TriangleMesh) {
                auto physicsComponentNew = entity.addComponent<PhysicsComponent>(_dynamicsWorld, physicsComponent->getColliderType(), 
                    physicsComponent->getShapeDimensions(), physicsComponent->getLocalScale(), physicsComponent->getPosition(),
                    physicsComponent->getRotation(), physicsComponent->getMass(), entity.getSource());
                physicsComponentNew.setIsRotated(physicsComponent->getIsRotated());
            } else {
                entity.addComponent<PhysicsComponent>(_dynamicsWorld, physicsComponent->getColliderType(), 
                    physicsComponent->getTriangleMesh(), physicsComponent->getLocalScale(), physicsComponent->getPosition(),
                    physicsComponent->getRotation(), entity.getSource());
            }
        }
        if(scriptComponent && !entity.hasComponent<ScriptComponent>()) {
            auto& scriptComponentNew = entity.addComponent<ScriptComponent>();
            auto& scripts = scriptComponent->getScripts();
            for(auto& script : scripts) {
                auto scriptDLLData = _sourceScene->getScriptDLLRegistry()->get(script.behaviorClassName);
                if(scriptDLLData && scriptDLLData->isValid) {
                    auto behaviorFactory = DLLCreator::loadDLLFunc(*scriptDLLData);
                    if(behaviorFactory) {
                        Behavior* behavior = (Behavior*)behaviorFactory();
                        auto scriptSpec = scriptComponentNew.bind(behavior, scriptDLLData->scriptName);
                        if(scriptSpec)
                            scriptSpec->isEnabled = script.isEnabled;
                    }
                }
            }
        }
        if(audioComponent && !entity.hasComponent<AudioComponent>()) {
            auto& soundSources = audioComponent->getSoundSources();
            auto& audioComponentNew = entity.addComponent<AudioComponent>(_sourceScene->getSceneAudio()->getSoundEngine());
            for(auto soundSource : soundSources) {
                auto newSoundSource = audioComponentNew.addSoundSource(soundSource->getSoundSourcePath(), soundSource->getIs3D());
                newSoundSource->setPlayLooped(soundSource->getPlayLooped());
                newSoundSource->setStartPaused(soundSource->getStartPaused());
                newSoundSource->setVolume(soundSource->getVolume());
                newSoundSource->setMinDistance(soundSource->getMinDistance());
                newSoundSource->setMaxDistance(soundSource->getMaxDistance());
                newSoundSource->setPlaybackSpeed(soundSource->getPlaybackSpeed());
            }
        }
    }
}