#include "scene/scene.hpp"

namespace TWE {
    Scene::Scene(uint32_t windowWidth, uint32_t windowHeight) {
        _sceneRegistry.current = &_sceneRegistry.edit;
        _sceneState = SceneState::Edit;

        FBOAttachmentSpecification attachments = { FBOTextureFormat::RGBA8, FBOTextureFormat::R32I, FBOTextureFormat::DEPTH24STENCIL8 };
        _frameBuffer = std::make_unique<FBO>(windowWidth, windowHeight, attachments);
        _debugCamera = nullptr;
        _scriptDLLRegistry = nullptr;
        _isFocusedOnDebugCamera = true;
        _drawColliders = true;
        _name = "Unnamed";
    }

    void Scene::setTransMat(const glm::mat4& transform, TransformMatrixOptions option) {
        _sceneRegistry.current->entityRegistry.view<MeshRendererComponent>().each([&](entt::entity entity, MeshRendererComponent& meshRendererComponent){
            meshRendererComponent.shader->setUniform(TRANS_MAT_OPTIONS[option], transform);
        });
    }

    void Scene::setDebugCamera(DebugCamera* debugCamera) {
        _debugCamera = debugCamera;
    }

    void Scene::setName(const std::string& name) {
        _name = name;
    }

    void Scene::setScriptDLLRegistry(Registry<DLLLoadData>* scriptDLLRegistry) {
        _scriptDLLRegistry = scriptDLLRegistry;
    }

    void Scene::setState(SceneState state) {
        _sceneState = state;
        switch (_sceneState) {
        case SceneState::Edit:
            _isFocusedOnDebugCamera = true;
            _sceneRegistry.current = &_sceneRegistry.edit;
            break;
        case SceneState::Run:
            _isFocusedOnDebugCamera = false;
            _sceneRegistry.run.lastId = 0;
            _sceneRegistry.run.urControl.reset();
            resetPhysics(&_sceneRegistry.run.entityRegistry);
            resetScripts(&_sceneRegistry.run.entityRegistry);
            resetEntityRegistry(&_sceneRegistry.run.entityRegistry);
            copySceneState(_sceneRegistry.edit, _sceneRegistry.run);
            _sceneRegistry.current = &_sceneRegistry.run;
            break;
        }
    }

    std::vector<Entity> Scene::unbindScript(entt::registry* registry, DLLLoadData* dllData) {
        std::vector<Entity> res;
        auto& view = registry->view<ScriptComponent>();
        for(auto entity : view) {
            auto& scriptComponent = view.get<ScriptComponent>(entity);
            if(scriptComponent.getBehaviorClassName() != dllData->scriptName)
                continue;
            scriptComponent.unbind();
            Entity instance { entity, this };
            res.push_back(instance);
        }
        return res;
    }

    void Scene::bindScript(DLLLoadData* dllData, Entity& entity) {
        if(!dllData || !dllData->isValid) {
            entity.getComponent<ScriptComponent>().bind<Behavior>();
            return;
        }
        auto behaviorFactory = DLLCreator::loadDLLFunc(*dllData);
        if(!behaviorFactory) {
            entity.getComponent<ScriptComponent>().bind<Behavior>();
            return;
        }
        Behavior* behavior = (Behavior*)behaviorFactory();
        entity.getComponent<ScriptComponent>().bind(behavior, dllData->scriptName);
    }

    void Scene::bindScript(DLLLoadData* dllData, std::vector<Entity>& entities) {
        for(auto& entity : entities)
            bindScript(dllData, entity);
    }

    void Scene::validateScript(const std::string& scriptName) {
        auto scriptDLLData = _scriptDLLRegistry->get(scriptName);
        if(scriptDLLData && scriptDLLData->isValid) {
            auto& editEntities = unbindScript(&_sceneRegistry.edit.entityRegistry, scriptDLLData);
            auto& runEntities = unbindScript(&_sceneRegistry.run.entityRegistry, scriptDLLData);
            DLLCreator::freeDLLFunc(*scriptDLLData);
            _scriptDLLRegistry->erase(scriptName);
            auto dllData = new DLLLoadData(DLLCreator::compileScript(scriptName, scriptDLLData->scriptDirectoryPath));
            _scriptDLLRegistry->add(scriptName, dllData);
            if(!dllData || !dllData->isValid)
                std::cout << "Script " +  scriptName + " compile error!\n";
            bindScript(dllData, editEntities);
        } else 
            std::cout << "Script " +  scriptName + " was not found in the script dll registry!\n";
    }

    void Scene::validateScripts() {
        auto& scriptDLLRegistryKeys = _scriptDLLRegistry->getKeys();
        for(auto& scriptName : scriptDLLRegistryKeys)
            validateScript(scriptName);
    }

    void Scene::reset() {
        resetPhysics(&_sceneRegistry.run.entityRegistry);
        resetScripts(&_sceneRegistry.run.entityRegistry);
        resetEntityRegistry(&_sceneRegistry.run.entityRegistry);
        resetPhysics(&_sceneRegistry.edit.entityRegistry);
        resetScripts(&_sceneRegistry.edit.entityRegistry);
        resetEntityRegistry(&_sceneRegistry.edit.entityRegistry);
        setState(SceneState::Edit);
        _sceneRegistry.run.lastId = 0;
        _sceneRegistry.edit.lastId = 0;
        _sceneRegistry.run.urControl.reset();
        _sceneRegistry.edit.urControl.reset();
        Shape::reset();
    }

    void Scene::resetEntityRegistry(entt::registry* registry) {
        registry->clear();
        *registry = {};
    }

    void Scene::resetScripts(entt::registry* registry) {
        registry->view<ScriptComponent>().each([&](entt::entity entity, ScriptComponent& scriptComponent){
            scriptComponent.unbind();
        });
    }

    void Scene::resetPhysics(entt::registry* registry) {
        registry->view<PhysicsComponent>().each([&](entt::entity entity, PhysicsComponent& physicsComponent){
            if(!physicsComponent.getRigidBody())
                return;
            delete physicsComponent.getRigidBody()->getMotionState();
            delete physicsComponent.getRigidBody()->getCollisionShape();
            physicsComponent.getDynamicsWorld()->removeRigidBody(physicsComponent.getRigidBody());
            delete physicsComponent.getRigidBody();
        });
    }

    void Scene::setLight(const LightComponent& light, TransformComponent& transform, const  uint32_t index) {
        std::string lightIndex = "lights[]";
        lightIndex.insert(lightIndex.length() - 1, std::to_string(index));
        _sceneRegistry.current->entityRegistry.view<MeshRendererComponent>().each([&](entt::entity entity, MeshRendererComponent& meshRendererComponent){
            Renderer::setLight(meshRendererComponent, light, transform, lightIndex);
        });
    }

    void Scene::setViewPos(const glm::vec3& pos) {
        _sceneRegistry.current->entityRegistry.view<MeshRendererComponent>().each([&](entt::entity entity, MeshRendererComponent& meshRendererComponent){
            Renderer::setViewPosition(meshRendererComponent, pos);
        });
    }

    void Scene::updateView(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& pos) {
        setTransMat(view, TransformMatrixOptions::VIEW);
        setTransMat(projection, TransformMatrixOptions::PROJECTION);
        _sceneRegistry.current->physics.getDebugDrawer()->setMats(view, projection);
        setViewPos(pos);
    }

    void Scene::updateShadows(uint32_t windowWidth, uint32_t windowHeight) {
        glm::mat4& lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 500.f);
        int lightIndex = -1;
        _drawColliders = false;
        _sceneRegistry.current->entityRegistry.view<LightComponent, TransformComponent>().each([&](entt::entity entity, LightComponent& lightComponent, 
        TransformComponent& transformComponent){
            ++lightIndex;
            if(!lightComponent.castShadows)
                return;
            glm::mat4& lightView = glm::lookAt(transformComponent.transform.position, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
            Renderer::generateDepthMap(lightComponent, transformComponent, lightProjection, lightView, this);
            setShadows(lightComponent, lightProjection * lightView, lightIndex);
        });
        Renderer::setViewport(0, 0, windowWidth, windowHeight);
    }

    void Scene::setShadows(const LightComponent& lightComponent, const glm::mat4& lightSpaceMat, int index) {
        std::string indexStr = std::to_string(index);
        std::string lightIndex = "lights[]";
        lightIndex.insert(lightIndex.length() - 1, indexStr);
        _sceneRegistry.current->entityRegistry.view<MeshRendererComponent>().each([&](entt::entity entity, MeshRendererComponent& meshRendererComponent){
            if(_sceneRegistry.current->entityRegistry.any_of<LightComponent>(entity))
                return;
            Renderer::setShadows(meshRendererComponent, lightSpaceMat, lightIndex);
        });
    }

    bool Scene::updateView() {
        _sceneCameraSpecification.camera = nullptr;
        _sceneCameraSpecification.position = glm::vec3(0.f);
        _sceneCameraSpecification.forward = glm::vec3(0.f);
        _sceneCameraSpecification.up = glm::vec3(0.f);
        if(_isFocusedOnDebugCamera && _debugCamera) {
            _sceneCameraSpecification.camera = _debugCamera;
            _sceneCameraSpecification.position = _debugCamera->getPosition();
            _sceneCameraSpecification.forward = _debugCamera->getForward();
            _sceneCameraSpecification.up = _debugCamera->getUp();
        }
        else {
            auto& camsView = _sceneRegistry.current->entityRegistry.view<TransformComponent>();
            for(auto camEntity : camsView) {
                Entity entity = { camEntity, this };
                if(!entity.hasComponent<CameraComponent>())
                    continue;
                auto& transformComponent = entity.getComponent<TransformComponent>();
                auto& cameraComponent = entity.getComponent<CameraComponent>();
                if(cameraComponent.isFocusedOn()){
                    _sceneCameraSpecification.camera = cameraComponent.getSource();
                    _sceneCameraSpecification.position = transformComponent.transform.position;
                    _sceneCameraSpecification.forward = -transformComponent.getForward();
                    _sceneCameraSpecification.up = transformComponent.getUp();
                    break;
                }
            }
        }
        if(!_sceneCameraSpecification.camera)
            return false;
        updateView(_sceneCameraSpecification.camera->getView(_sceneCameraSpecification.position, _sceneCameraSpecification.forward, _sceneCameraSpecification.up),
                   _sceneCameraSpecification.camera->getProjection(), _sceneCameraSpecification.position);
        return true;
    }

    void Scene::updateEditState() {
        updateTransforms();
        auto& fboSize = _frameBuffer->getSize();
        updateShadows(fboSize.first, fboSize.second);
        if(!updateView()) {
            _frameBuffer->bind();
            Renderer::cleanScreen({});
            _frameBuffer->unbind();
            return;
        }
        updateLight();
        _drawColliders = _isFocusedOnDebugCamera;
        #ifndef TWE_BUILD
        _frameBuffer->bind();
        Renderer::cleanScreen({});
        draw();
        if(_drawColliders)
            _sceneRegistry.current->physics.debugDrawWorld();
        _frameBuffer->unbind();
        #else
        draw();
        #endif
    }

    void Scene::updateRunState() {
        updatePhysics();
        updateScripts();
        updateTransforms();
        _sceneRegistry.current->physics.cleanCollisionDetection();
        auto& fboSize = _frameBuffer->getSize();
        updateShadows(fboSize.first, fboSize.second);
        if(!updateView()) {
            _frameBuffer->bind();
            Renderer::cleanScreen({});
            _frameBuffer->unbind();
            return;
        }
        updateLight();
        _drawColliders = _isFocusedOnDebugCamera;
        #ifndef TWE_BUILD
        _frameBuffer->bind();
        Renderer::cleanScreen({});
        draw();
        if(_drawColliders)
            _sceneRegistry.current->physics.debugDrawWorld();
        _frameBuffer->unbind();
        #else
        draw();
        #endif
    }

    void Scene::updateScripts() {
        _sceneRegistry.current->entityRegistry.view<ScriptComponent>().each([&](entt::entity entity, ScriptComponent& scriptComponent){
            if(!scriptComponent.isEnabled)
                return;
            try {
                if(!scriptComponent._isInitialized) {
                    scriptComponent.initialize(entity, this);
                    scriptComponent._instance->start();
                }
                if(scriptComponent._instance->gameObject.hasComponent<PhysicsComponent>()) {
                    auto& physicsComponent = scriptComponent._instance->gameObject.getComponent<PhysicsComponent>();
                    auto collisions = _sceneRegistry.current->physics.getCollisionDetection(physicsComponent.getRigidBody());
                    for(auto collisionObj : collisions){
                        auto userPointer = (PhysicsUserPointer*)collisionObj->getUserPointer();
                        scriptComponent._instance->collisionDetection(Entity{userPointer->entity, this}, collisionObj);
                    }
                }
                scriptComponent._instance->update(Time::getDeltaTime());
            } catch(const std::runtime_error& e) {
                std::cerr << e.what() << "\n";
                scriptComponent.isEnabled = false;
            }
        });
    } 

    void Scene::updateLight() {
        uint32_t lightIndex = 0;
        _sceneRegistry.current->entityRegistry.view<LightComponent, TransformComponent>()
            .each([&](entt::entity entity, LightComponent& lightComponent, TransformComponent& transformComponent){
                setLight(lightComponent, transformComponent, lightIndex++);
            });
    } 

    void Scene::updateTransforms() {
        _sceneRegistry.current->entityRegistry.view<TransformComponent>().each([&](entt::entity entity, TransformComponent& transformComponent){
            if(transformComponent.preTransform == transformComponent.transform)
                return;
            Entity instance = { entity, this };
            auto& parentChildsComponent = instance.getComponent<ParentChildsComponent>();
            ModelSpecification ratioTransform(
                transformComponent.transform.position - transformComponent.preTransform.position,
                transformComponent.transform.rotation - transformComponent.preTransform.rotation,
                transformComponent.transform.size - transformComponent.preTransform.size
            );
            for(auto& child : parentChildsComponent.childs) {
                Entity childInstance = { child, this };
                updateChildsTransform(childInstance, ratioTransform, transformComponent.transform.position);
            }
            if(instance.hasComponent<PhysicsComponent>()) {
                auto& physicsComponent = instance.getComponent<PhysicsComponent>();
                if(!physicsComponent.getNeedUpdate())
                    physicsComponent.setNeedUpdate(true);
                else {
                    if(ratioTransform.position != glm::vec3(0.f))
                        physicsComponent.setPosition(physicsComponent.getPosition() + ratioTransform.position);
                    if(ratioTransform.rotation != glm::vec3(0.f))
                        physicsComponent.setRotation(glm::quat(transformComponent.transform.rotation));
                    if(ratioTransform.size != glm::vec3(0.f))
                        physicsComponent.setSize(physicsComponent.getLocalScale() + ratioTransform.size);
                }
            }
            transformComponent.preTransform = transformComponent.transform;
        });
    } 

    void Scene::updateChildsTransform(Entity& entity, ModelSpecification& ratioTransform, const glm::vec3& centerPositon) {
        auto& transformComponent = entity.getComponent<TransformComponent>();
        bool hasPhysics = entity.hasComponent<PhysicsComponent>();
        PhysicsComponent* physicsComponent;
        if(hasPhysics)
            physicsComponent = &entity.getComponent<PhysicsComponent>();
        if(ratioTransform.position != glm::vec3(0.f)) {
            transformComponent.setPosition(transformComponent.transform.position + ratioTransform.position);
            if(hasPhysics)
                physicsComponent->setPosition(physicsComponent->getPosition() + ratioTransform.position);
        }
        if(ratioTransform.rotation != glm::vec3(0.f)) {
            transformComponent.rotateAroundOrigin(ratioTransform.rotation, centerPositon);
            if(hasPhysics) {
                physicsComponent->setRotation(glm::quat(transformComponent.transform.rotation));
                physicsComponent->setPosition(transformComponent.transform.position);
            }
        }
        if(ratioTransform.size != glm::vec3(0.f)) {
            transformComponent.setSize(transformComponent.transform.size + ratioTransform.size);
            if(hasPhysics)
                physicsComponent->setSize(physicsComponent->getLocalScale() + ratioTransform.size);
        }
        auto& parentChildsComponent = entity.getComponent<ParentChildsComponent>();
        for(auto& child : parentChildsComponent.childs) {
            Entity childInstance = { child, this };
            updateChildsTransform(childInstance, ratioTransform, centerPositon);
        }
        transformComponent.preTransform = transformComponent.transform;
    }

    void Scene::update() {
        #ifndef TWE_BUILD
        switch (_sceneState) {
        case SceneState::Edit:
        case SceneState::Pause:
            updateEditState();
            break;
        case SceneState::Run:
            updateRunState();
            break;
        }
        #else
        updateRunState();
        #endif
    }

    void Scene::draw() {
        auto& lightEnteties = _sceneRegistry.current->entityRegistry.view<LightComponent>();
        _sceneRegistry.current->entityRegistry.view<MeshComponent, MeshRendererComponent, TransformComponent>()
            .each([&](entt::entity entity, MeshComponent& meshComponent, MeshRendererComponent& meshRendererComponent, TransformComponent& transformComponent){
                Renderer::execute(&meshComponent, &meshRendererComponent, &transformComponent, lightEnteties.size());
            });
    }

    void Scene::updatePhysics() {
        _sceneRegistry.current->entityRegistry.view<PhysicsComponent, TransformComponent>().each([](entt::entity entity, PhysicsComponent& physicsComponent, 
        TransformComponent& transformComponent){
            btRigidBody* rigidBody = physicsComponent.getRigidBody();
            rigidBody->activate();
            if(physicsComponent.getMass() == 0.f)
                return;
            btTransform worldTransform;
            rigidBody->getMotionState()->getWorldTransform(worldTransform);
            btVector3 pos = worldTransform.getOrigin();
            btQuaternion quat = worldTransform.getRotation();
            transformComponent.setPosition(glm::vec3(pos.getX(), pos.getY(), pos.getZ()));
            glm::vec3 rot;
            quat.getEulerZYX(rot.z, rot.y, rot.x);
            transformComponent.setRotation(rot);
            physicsComponent.setNeedUpdate(false);
        });
        _sceneRegistry.current->physics.getDynamicWorld()->stepSimulation(Time::getDeltaTime());
        _sceneRegistry.current->physics.checkCollisionsDetection();
    }

    void Scene::cleanEntity(Entity& entity) {
        if(entity.hasComponent<CameraComponent>())
            entity.removeComponent<CameraComponent>();
        if(entity.hasComponent<CreationTypeComponent>())
            entity.removeComponent<CreationTypeComponent>();
        if(entity.hasComponent<LightComponent>())
            entity.removeComponent<LightComponent>();
        if(entity.hasComponent<MeshComponent>())
            entity.removeComponent<MeshComponent>();
        if(entity.hasComponent<MeshRendererComponent>())
            entity.removeComponent<MeshRendererComponent>();
        if(entity.hasComponent<PhysicsComponent>()){
            auto& physicsComponent = entity.getComponent<PhysicsComponent>();
            physicsComponent.getDynamicsWorld()->removeRigidBody(physicsComponent.getRigidBody());
            entity.removeComponent<PhysicsComponent>();
        }
        if(entity.hasComponent<ScriptComponent>()) {
            entity.getComponent<ScriptComponent>().unbind();
            entity.removeComponent<ScriptComponent>();
        }
        entity.removeComponent<TransformComponent>();
        entity.removeComponent<NameComponent>();
        entity.removeComponent<IDComponent>();

        auto& parentChildsComponent = entity.getComponent<ParentChildsComponent>();
        if(parentChildsComponent.parent != entt::null) {
            Entity parentEntity = Entity{ parentChildsComponent.parent, this };
            auto& parentParentChildsComponent = parentEntity.getComponent<ParentChildsComponent>();
            auto itChild = std::find_if(parentParentChildsComponent.childs.begin(), parentParentChildsComponent.childs.end(), [&](entt::entity item){
                return item == entity.getSource();
            });
            if(itChild != parentParentChildsComponent.childs.end())
                parentParentChildsComponent.childs.erase(itChild);
        }
        for(auto childEntity : parentChildsComponent.childs)
            cleanEntity(Entity{ childEntity, this });
        entity.removeComponent<ParentChildsComponent>();
    }

    Entity Scene::createEntity(const std::string& name) {
        auto entity = _sceneRegistry.current->entityRegistry.create();
        _sceneRegistry.current->entityRegistry.emplace<TransformComponent>(entity);
        _sceneRegistry.current->entityRegistry.emplace<NameComponent>(entity, name);
        _sceneRegistry.current->entityRegistry.emplace<CreationTypeComponent>(entity);
        _sceneRegistry.current->entityRegistry.emplace<ParentChildsComponent>(entity);
        _sceneRegistry.current->entityRegistry.emplace<IDComponent>(entity, _sceneRegistry.current->lastId++);
        _sceneRegistry.current->entityRegistry.emplace<EmptyComponent>(entity);
        return { entity, this };
    }

    void Scene::copySceneState(SceneStateSpecification& from, SceneStateSpecification& to) {
        auto& view = from.entityRegistry.view<EmptyComponent>();
        int size = view.size();
        for(int i = size - 1; i >= 0; --i)
            copyEntityState(Entity{ view[i], this }, to);
    }

    Entity Scene::copyEntityState(Entity& entity, SceneStateSpecification& to) {
        entt::entity instance = to.entityRegistry.create();

        if(entity.hasComponent<TransformComponent>())
            to.entityRegistry.emplace<TransformComponent>(instance, entity.getComponent<TransformComponent>());
        if(entity.hasComponent<NameComponent>())
            to.entityRegistry.emplace<NameComponent>(instance, entity.getComponent<NameComponent>());
        if(entity.hasComponent<CreationTypeComponent>())
            to.entityRegistry.emplace<CreationTypeComponent>(instance, entity.getComponent<CreationTypeComponent>());
        if(entity.hasComponent<ParentChildsComponent>())
            to.entityRegistry.emplace<ParentChildsComponent>(instance, entity.getComponent<ParentChildsComponent>());
        if(entity.hasComponent<IDComponent>())
            to.entityRegistry.emplace<IDComponent>(instance, entity.getComponent<IDComponent>());
        if(entity.hasComponent<CameraComponent>())
            to.entityRegistry.emplace<CameraComponent>(instance, entity.getComponent<CameraComponent>());
        if(entity.hasComponent<LightComponent>())
            to.entityRegistry.emplace<LightComponent>(instance, entity.getComponent<LightComponent>());

        if(entity.hasComponent<ScriptComponent>()) {
            auto& scriptComponent = to.entityRegistry.emplace<ScriptComponent>(instance);
            auto scriptDLLData = _scriptDLLRegistry->get(entity.getComponent<ScriptComponent>().getBehaviorClassName());
            if(!scriptDLLData || !scriptDLLData->isValid)
                scriptComponent.bind<Behavior>();
            else {
                auto behaviorFactory = DLLCreator::loadDLLFunc(*scriptDLLData);
                if(!behaviorFactory)
                    scriptComponent.bind<Behavior>();
                else {
                    Behavior* behavior = (Behavior*)behaviorFactory();
                    scriptComponent.bind(behavior, scriptDLLData->scriptName);
                }
                scriptComponent.isEnabled = entity.getComponent<ScriptComponent>().isEnabled;
            }
        }

        if(entity.hasComponent<MeshComponent>()) {
            auto& meshComponent = entity.getComponent<MeshComponent>();
            to.entityRegistry.emplace<MeshComponent>(instance, meshComponent.vao, meshComponent.vbo, meshComponent.ebo, 
                meshComponent.registryId, meshComponent.texture->getAttachments());
        }

        if(entity.hasComponent<MeshRendererComponent>()) {
            auto& meshRendererComponent = entity.getComponent<MeshRendererComponent>();
            auto& meshRendererComponentCopy = to.entityRegistry.emplace<MeshRendererComponent>(instance, meshRendererComponent.shader->getVertPath().c_str(), 
                meshRendererComponent.shader->getFragPath().c_str(), (int)instance, meshRendererComponent.registryId);
            meshRendererComponentCopy.setMaterial(meshRendererComponent.material);
        }

        if(entity.hasComponent<PhysicsComponent>()) {
            auto& physicsComponent = entity.getComponent<PhysicsComponent>();
            if(physicsComponent.getColliderType() != ColliderType::TriangleMesh) {
                auto shapeDimensions = physicsComponent.getShapeDimensions() / physicsComponent.getLocalScale();
                auto& physxComp = to.entityRegistry.emplace<PhysicsComponent>(instance, to.physics.getDynamicWorld(), physicsComponent.getColliderType(), 
                    shapeDimensions, physicsComponent.getLocalScale(), physicsComponent.getPosition(),
                    physicsComponent.getRotation(), physicsComponent.getMass(), instance);
                physxComp.setIsRotated(physicsComponent.getIsRotated());
            } else {
                to.entityRegistry.emplace<PhysicsComponent>(instance, to.physics.getDynamicWorld(), physicsComponent.getColliderType(), 
                    physicsComponent.getTriangleMesh(), physicsComponent.getLocalScale(), physicsComponent.getPosition(),
                    physicsComponent.getRotation(), instance);
            }
        }

        return { instance, this };
    }

    bool& Scene::getIsFocusedOnDebugCamera() { return _isFocusedOnDebugCamera; }
    entt::registry* Scene::getRegistry() const noexcept { return &_sceneRegistry.current->entityRegistry; }
    btDynamicsWorld* Scene::getDynamicWorld() const noexcept { return _sceneRegistry.current->physics.getDynamicWorld(); }
    std::string Scene::getName() const noexcept { return _name; }
    FBO* Scene::getFrameBuffer() const noexcept { return _frameBuffer.get(); }
    Registry<DLLLoadData>* Scene::getScriptDLLRegistry() const noexcept { return _scriptDLLRegistry; }
    SceneStateSpecification* Scene::getSceneStateSpecification() { return _sceneRegistry.current; }
}