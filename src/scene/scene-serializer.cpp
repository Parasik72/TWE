#include "scene/scene-serializer.hpp"

namespace TWE {
    bool SceneSerializer::serialize(Scene* scene, const std::string& path, ProjectData* projectData) {
        if(scene->_sceneState != SceneState::Edit)
            return false;
        nlohmann::json jsonMain;
        std::string sceneName = std::filesystem::path(path).stem().string();
        scene->setName(sceneName);
        jsonMain["Scene"] = sceneName;
        jsonMain["RootPath"] = projectData->rootPath;
        nlohmann::json jsonEntities = nlohmann::json::array();
        auto& view = scene->_sceneRegistry.edit.entityRegistry.view<NameComponent>();
        int size = view.size();
        for(int i = size - 1; i >= 0; --i) {
            Entity instance = { view[i], scene };
            if(instance.hasComponent<NameComponent>())
                serializeEntity(instance, jsonEntities, projectData, scene);
        }
        jsonMain["Entities"] = jsonEntities;
        File::save(path.c_str(), jsonMain.dump());
        return true;
    }

    bool SceneSerializer::deserialize(Scene* scene, const std::string& path) {
        std::string jsonBodyStr = File::getBody(path.c_str());
        nlohmann::json jsonMain = nlohmann::json::parse(jsonBodyStr);
        if(!jsonMain.contains("Scene"))
            return false;
        scene->reset();
        std::filesystem::path rootPath;
        #ifndef TWE_BUILD
        rootPath = (std::string)jsonMain["RootPath"];
        #else
        rootPath = "./";
        #endif
        scene->setName(jsonMain["Scene"]);
        auto& entities = jsonMain["Entities"].items();
        for(auto& [index, components] : entities) {        
            Entity instance = deserializeCreationTypeComponent(scene, components, rootPath);
            if(instance.getSource() != entt::null)
                deserializeEntity(scene, instance, components, rootPath);
        }
        revalidateParentChildsComponent(scene);
        return true;
    }

    void SceneSerializer::revalidateParentChildsComponent(Scene* scene) {
        int lastId = -1;
        std::map<entt::entity, ParentChildsComponent> newParentChilds;
        scene->_sceneRegistry.edit.entityRegistry.view<ParentChildsComponent>().each([&](entt::entity entityOut, ParentChildsComponent& parentChildsComponent){
            Entity outInstance = { entityOut, scene };
            std::vector<entt::entity> childs;
            entt::entity parent = entt::null;
            scene->_sceneRegistry.edit.entityRegistry.view<IDComponent>().each([&](entt::entity entityIn, IDComponent& idComponent) {
                if(idComponent.id > lastId)
                    lastId = idComponent.id;
                if(entityOut == entityIn)
                    return;
                if(parentChildsComponent.parent == (entt::entity)idComponent.id) {
                    parent = entityIn;
                    return;
                }
                for(auto childId : parentChildsComponent.childs)
                    if(childId == (entt::entity)idComponent.id) {
                        childs.push_back(entityIn);
                        return;
                    }
            });
            newParentChilds.insert({entityOut, ParentChildsComponent{parent, childs}});
        });
        scene->_sceneRegistry.edit.lastId = ++lastId;
        for(auto [entity, parentChilds] : newParentChilds) {
            Entity instance = { entity, scene };
            auto& parentChildsComponent = instance.getComponent<ParentChildsComponent>();
            parentChildsComponent.parent = parentChilds.parent;
            parentChildsComponent.childs = parentChilds.childs;
        }
    }

    void SceneSerializer::serializeEntity(Entity& entity, nlohmann::json& jsonEntities, ProjectData* projectData, Scene* scene) {
        nlohmann::json jsonEntity;
        jsonEntity["Entity ID"] = entity.getComponent<IDComponent>().id;
        serializeCreationTypeComponent(entity, jsonEntity);
        serializeNameComponent(entity, jsonEntity);
        serializeTransformComponent(entity, jsonEntity);
        serializeMeshComponent(entity, jsonEntity, projectData);
        serializeMeshRendererComponent(entity, jsonEntity);
        serializeCameraComponent(entity, jsonEntity);
        serializeLightComponent(entity, jsonEntity);
        serializePhysicsComponent(entity, jsonEntity);
        serializeScriptComponent(entity, jsonEntity);
        serializeParentChildsComponent(entity, jsonEntity, scene);
        serializeAudioComponent(entity, jsonEntity, projectData);
        jsonEntities.push_back(jsonEntity);
    }

    void SceneSerializer::deserializeEntity(Scene* scene, Entity& entity, nlohmann::json& jsonComponents, const std::filesystem::path& rootPath) {
        auto& components = jsonComponents.items();
        for(auto& [key, value] : components) {
            deserializeNameComponent(entity, key, value);
            deserializeTransformComponent(entity, key, value);
            deserializeMeshRendererComponent(entity, key, value);
            deserializeCameraComponent(entity, key, value);
            deserializeLightComponent(entity, key, value);
            deserializePhysicsComponent(scene, entity, key, value);
            deserializeScriptComponent(scene, entity, key, value);
            deserializeParentChildsComponent(entity, key, value);
            deserializeIDComponent(entity, key, value);
            deserializeAudioComponent(entity, key, value, rootPath);
        }
    }

    void SceneSerializer::serializeCreationTypeComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<CreationTypeComponent>())
            return;
        auto& creationTypeComponent = entity.getComponent<CreationTypeComponent>();
        nlohmann::json jsonCreationTypeComponent;

        jsonCreationTypeComponent["Type"] = creationTypeComponent.getType();

        jsonEntity["CreationTypeComponent"] = jsonCreationTypeComponent;
    }

    Entity SceneSerializer::deserializeCreationTypeComponent(Scene* scene, nlohmann::json& jsonComponent, const std::filesystem::path& rootPath) {
        auto creationTypeComponent = static_cast<EntityCreationType>(jsonComponent["CreationTypeComponent"]["Type"]);
        auto jsonMeshComponent = jsonComponent.find("MeshComponent");
        TextureAttachmentSpecification* textureAtttachments = new TextureAttachmentSpecification();
        bool hasMesh = false;
        if(jsonMeshComponent != jsonComponent.end()) {
            nlohmann::json jsonTextures = jsonComponent["MeshComponent"]["Textures"];
            for(auto& spec : jsonTextures) {
                std::string imgPath = rootPath.string() + '/' + (std::string)spec["ImgPath"];
                if(!std::filesystem::exists(imgPath))
                    continue;
                hasMesh = true;
                TextureSpecification specification;
                specification.imgPath = imgPath;
                specification.texNumber = spec["TexNumber"];
                specification.texType = spec["TexType"];
                specification.inOutTexFormat = spec["InOutTexFormat"];
                textureAtttachments->textureSpecifications.push_back(specification);
            }
        }
        TextureAttachmentSpecification* textureInRegistry = nullptr;
        if(hasMesh) {
            auto values = Shape::shapeSpec->textureRegistry->getValues();
            for(auto& value : values)
                if(*value == *textureAtttachments) {
                    textureInRegistry = value;
                    break;
                }
        }
        if(!textureInRegistry && hasMesh) {
            Texture* texture = new Texture(*textureAtttachments);
            textureAtttachments = &texture->getAttachments();
            textureInRegistry = Shape::shapeSpec->textureRegistry->add("Texture-" + std::to_string(Shape::shapeSpec->textureNumber++), textureAtttachments);
        }
        else
            delete textureAtttachments;
        Entity entity;
        switch (creationTypeComponent) {
        case EntityCreationType::None:
            entity = scene->createEntity();
            break;
        case EntityCreationType::Cube:
            entity = Shape::createCubeEntity(scene);
            break;
        case EntityCreationType::Plate:
            entity = Shape::createPlateEntity(scene);
            break;
        case EntityCreationType::Cubemap: {
            TextureAttachmentSpecification textureAttachmentSpecification;
            entity = Shape::createCubemapEntity(scene, textureAttachmentSpecification);
        } break;
        case EntityCreationType::SpotLight: {
            entity = Shape::createSpotLightEntity(scene);
        } break;
        case EntityCreationType::PointLight: {
            entity = Shape::createPointLightEntity(scene);
        } break;
        case EntityCreationType::DirLight: {
            entity = Shape::createDirLightEntity(scene);
        } break;
        case EntityCreationType::Camera: {
            entity = Shape::createCameraEntity(scene);
        } break;
        case EntityCreationType::Model: {
            std::filesystem::path modelPath = rootPath / (std::string)jsonComponent["MeshComponent"]["ModelPath"];
            if(jsonComponent["MeshComponent"].find("ModelIndex") == jsonComponent["MeshComponent"].end())
                return {};
            int modelIndex = jsonComponent["MeshComponent"]["ModelIndex"];
            entity = Shape::createModelEntity(scene, modelPath, modelIndex);
        } break;
        }
        if(entity.hasComponent<MeshComponent>() && textureInRegistry) {
            auto& meshComponent = entity.getComponent<MeshComponent>();
            Texture* texture = new Texture();
            texture->setAttachments(*textureInRegistry);
            meshComponent.texture = std::make_shared<Texture>(*texture);
            int a = 5;
        }
        return entity;
    }

    void SceneSerializer::serializeNameComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<NameComponent>())
            return;
        auto& nameComponent = entity.getComponent<NameComponent>();
        nlohmann::json jsonNameComponent;

        jsonNameComponent["Name"] = nameComponent.getName();
            
        jsonEntity["NameComponent"] = jsonNameComponent;
    }

    void SceneSerializer::deserializeNameComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "NameComponent")
            return;
        std::string name = deleteInvertedCommas(jsonComponent["Name"]);
        if(!entity.hasComponent<NameComponent>())
            entity.addComponent<NameComponent>();
        auto& nameComponent = entity.getComponent<NameComponent>();
        nameComponent.setName(name);
    }

    void SceneSerializer::serializeTransformComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<TransformComponent>())
            return;
        auto& transformComponent = entity.getComponent<TransformComponent>();
        nlohmann::json jsonTransformComponent;

        nlohmann::json jsonPosition = nlohmann::json::array();
        jsonPosition.push_back(transformComponent.transform.position.x);
        jsonPosition.push_back(transformComponent.transform.position.y);
        jsonPosition.push_back(transformComponent.transform.position.z);
        jsonTransformComponent["Position"] = jsonPosition;

        nlohmann::json jsonRotation = nlohmann::json::array();
        jsonRotation.push_back(transformComponent.transform.rotation.x);
        jsonRotation.push_back(transformComponent.transform.rotation.y);
        jsonRotation.push_back(transformComponent.transform.rotation.z);
        jsonTransformComponent["Rotation"] = jsonRotation;

        nlohmann::json jsonSize = nlohmann::json::array();
        jsonSize.push_back(transformComponent.transform.size.x);
        jsonSize.push_back(transformComponent.transform.size.y);
        jsonSize.push_back(transformComponent.transform.size.z);
        jsonTransformComponent["Size"] = jsonSize;

        jsonEntity["TransformComponent"] = jsonTransformComponent;
    }

    void SceneSerializer::deserializeTransformComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "TransformComponent")
            return;
        if(!entity.hasComponent<TransformComponent>())
            entity.addComponent<TransformComponent>();
        auto& transformComponent = entity.getComponent<TransformComponent>();

        nlohmann::json jsonPosition = jsonComponent["Position"];
        glm::vec3 position = {jsonPosition[0], jsonPosition[1], jsonPosition[2]};
        transformComponent.setPosition(position);
        transformComponent.preTransform.position = position;

        nlohmann::json jsonRotation = jsonComponent["Rotation"];
        glm::vec3 rotation = {jsonRotation[0], jsonRotation[1], jsonRotation[2]};
        transformComponent.setRotation(rotation);
        transformComponent.preTransform.rotation = rotation;

        nlohmann::json jsonSize = jsonComponent["Size"];
        glm::vec3 size = {jsonSize[0], jsonSize[1], jsonSize[2]};
        transformComponent.setSize(size);
        transformComponent.preTransform.size = size;
    }

    void SceneSerializer::serializeMeshComponent(Entity& entity, nlohmann::json& jsonEntity, ProjectData* projectData) {
        if(!entity.hasComponent<MeshComponent>())
            return;
        auto& meshComponent = entity.getComponent<MeshComponent>();
        nlohmann::json jsonMeshComponent;
        jsonMeshComponent["ModelPath"] = std::filesystem::relative(meshComponent.modelSpec.modelPath, projectData->rootPath).string();
        jsonMeshComponent["ModelIndex"] = meshComponent.modelSpec.modelIndex;
        jsonMeshComponent["IsModel"] = meshComponent.modelSpec.isModel;
            
        nlohmann::json jsonMeshTextures = nlohmann::json::array();
        for(auto& textureSpec : meshComponent.texture->getAttachments().textureSpecifications) {
            nlohmann::json jsonMeshTexture = nlohmann::json::object();
            jsonMeshTexture["ImgPath"] = std::filesystem::relative(textureSpec.imgPath, projectData->rootPath).string();
            jsonMeshTexture["TexNumber"] = textureSpec.texNumber;
            jsonMeshTexture["TexType"] = textureSpec.texType;
            jsonMeshTexture["InOutTexFormat"] = textureSpec.inOutTexFormat;
            jsonMeshTextures.push_back(jsonMeshTexture);
        }
        jsonMeshComponent["Textures"] = jsonMeshTextures;

        jsonEntity["MeshComponent"] = jsonMeshComponent;
    }

    void SceneSerializer::serializeMeshRendererComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<MeshRendererComponent>())
            return;
        auto& meshRendererComponent = entity.getComponent<MeshRendererComponent>();
        nlohmann::json jsonMeshRendererComponent;

        nlohmann::json jsonMaterial = nlohmann::json::object();
        jsonMaterial["Ambient"] = meshRendererComponent.material.ambient;
        jsonMaterial["Diffuse"] = meshRendererComponent.material.diffuse;
        jsonMaterial["Shininess"] = meshRendererComponent.material.shininess;
        jsonMaterial["Specular"] = meshRendererComponent.material.specular;
        nlohmann::json jsonMaterialColor = nlohmann::json::array();
        jsonMaterialColor.push_back(meshRendererComponent.material.objColor.x);
        jsonMaterialColor.push_back(meshRendererComponent.material.objColor.y);
        jsonMaterialColor.push_back(meshRendererComponent.material.objColor.z);
        jsonMaterial["ObjColor"] = jsonMaterialColor;
        jsonMeshRendererComponent["Material"] = jsonMaterial;

        nlohmann::json jsonShaders;
        jsonShaders["VertPath"] = std::filesystem::relative(meshRendererComponent.shader->getVertPath(), "../../").string();
        jsonShaders["FragPath"] = std::filesystem::relative(meshRendererComponent.shader->getFragPath(), "../../").string();
        jsonMeshRendererComponent["Shaders"] = jsonShaders;
            
        jsonEntity["MeshRendererComponent"] = jsonMeshRendererComponent;
    }

    void SceneSerializer::deserializeMeshRendererComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "MeshRendererComponent")
            return;
        if(!entity.hasComponent<MeshRendererComponent>())
            entity.addComponent<MeshRendererComponent>();
        auto& meshRendererComponent = entity.getComponent<MeshRendererComponent>();

        nlohmann::json jsonMaterial = jsonComponent["Material"];
        meshRendererComponent.material.ambient = jsonMaterial["Ambient"];
        meshRendererComponent.material.diffuse = jsonMaterial["Diffuse"];
        meshRendererComponent.material.shininess = jsonMaterial["Shininess"];
        meshRendererComponent.material.specular = jsonMaterial["Specular"];
        nlohmann::json jsonObjColor = jsonMaterial["ObjColor"];
        meshRendererComponent.material.objColor = { jsonObjColor[0], jsonObjColor[1], jsonObjColor[2] };

        std::string rootPath;
        #ifndef TWE_BUILD
        rootPath = "../../";
        #else
        rootPath = "./";
        #endif
        nlohmann::json jsonShaders = jsonComponent["Shaders"];
        std::string vertPath = rootPath + '/' + (std::string)jsonShaders["VertPath"];
        std::string fragPath = rootPath + '/' + (std::string)jsonShaders["FragPath"];
        std::string componentShaderVertName = std::filesystem::path(meshRendererComponent.shader->getVertPath()).filename().string();
        std::string componentShaderFragName = std::filesystem::path(meshRendererComponent.shader->getFragPath()).filename().string();
        std::string vertName = std::filesystem::path(vertPath).filename().string();
        std::string fragName = std::filesystem::path(fragPath).filename().string();
        if(componentShaderVertName != vertName || componentShaderFragName != fragName) {
            auto& source = Shape::shapeSpec->meshRendererRegistry->getSource();
            for(auto& [key, value] : source) {
                std::string vertNameSource = std::filesystem::path(value->vertexShaderPath).filename().string();
                if(vertNameSource == vertName) {
                    meshRendererComponent.setShader(value->vertexShaderPath.c_str(), value->fragmentShaderPath.c_str(), key);
                    break;
                }
            }
        }
    }

    void SceneSerializer::serializeCameraComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<CameraComponent>())
            return;
        auto& cameraComponent = entity.getComponent<CameraComponent>();
        Camera* camera = cameraComponent.getSource();
        nlohmann::json jsonCameraComponent;

        jsonCameraComponent["IsFocusedOn"] = cameraComponent.isFocusedOn();
        jsonCameraComponent["Type"] = camera->getType();

        nlohmann::json jsonPerspectiveSpecification;
        auto& perspectiveSpecification = camera->getPerspectiveSpecification();
        jsonPerspectiveSpecification["FOV"] = perspectiveSpecification.fov;
        jsonPerspectiveSpecification["WndWidth"] = perspectiveSpecification.wndWidth;
        jsonPerspectiveSpecification["WndHeight"] = perspectiveSpecification.wndHeight;
        jsonPerspectiveSpecification["Near"] = perspectiveSpecification.nearDepth;
        jsonPerspectiveSpecification["Far"] = perspectiveSpecification.farDepth;
        jsonCameraComponent["PerspectiveSpecification"] = jsonPerspectiveSpecification;

        nlohmann::json jsonOrthographicSpecification;
        auto& orthographicSpecification = camera->getOrthographicSpecification();
        jsonOrthographicSpecification["Left"] = orthographicSpecification.left;
        jsonOrthographicSpecification["Right"] = orthographicSpecification.right;
        jsonOrthographicSpecification["Bottom"] = orthographicSpecification.bottom;
        jsonOrthographicSpecification["Top"] = orthographicSpecification.top;
        jsonOrthographicSpecification["Near"] = orthographicSpecification.nearDepth;
        jsonOrthographicSpecification["Far"] = orthographicSpecification.farDepth;
        jsonCameraComponent["OrthographicSpecification"] = jsonOrthographicSpecification;

        jsonEntity["CameraComponent"] = jsonCameraComponent;
    }

    void SceneSerializer::deserializeCameraComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "CameraComponent")
            return;
        if(!entity.hasComponent<CameraComponent>())
            entity.addComponent<CameraComponent>();
        auto& cameraComponent = entity.getComponent<CameraComponent>();
        Camera* cameraSouce = cameraComponent.getSource();

        cameraComponent.setFocuse(jsonComponent["IsFocusedOn"]);
        
        nlohmann::json jsonPerspectiveSpecification = jsonComponent["PerspectiveSpecification"];
        nlohmann::json jsonOrthographicSpecification = jsonComponent["OrthographicSpecification"];
        auto type = static_cast<CameraProjectionType>(jsonComponent["Type"]);
        switch (type) {
        case CameraProjectionType::Perspective:
            cameraSouce->setOrthographic(
                jsonOrthographicSpecification["Left"],
                jsonOrthographicSpecification["Right"],
                jsonOrthographicSpecification["Bottom"],
                jsonOrthographicSpecification["Top"],
                jsonOrthographicSpecification["Near"],
                jsonOrthographicSpecification["Far"]
            );
            cameraSouce->setPerspective(
                jsonPerspectiveSpecification["FOV"], 
                jsonPerspectiveSpecification["WndWidth"],
                jsonPerspectiveSpecification["WndHeight"],
                jsonPerspectiveSpecification["Near"],
                jsonPerspectiveSpecification["Far"]
            );
            break;
        case CameraProjectionType::Orthographic:
            cameraSouce->setPerspective(
                jsonPerspectiveSpecification["FOV"], 
                jsonPerspectiveSpecification["WndWidth"],
                jsonPerspectiveSpecification["WndHeight"],
                jsonPerspectiveSpecification["Near"],
                jsonPerspectiveSpecification["Far"]
            );
            cameraSouce->setOrthographic(
                jsonOrthographicSpecification["Left"],
                jsonOrthographicSpecification["Right"],
                jsonOrthographicSpecification["Bottom"],
                jsonOrthographicSpecification["Top"],
                jsonOrthographicSpecification["Near"],
                jsonOrthographicSpecification["Far"]
            );
            break;
        }
    }

    void SceneSerializer::serializeLightComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<LightComponent>())
            return;
        auto& lightComponent = entity.getComponent<LightComponent>();
        nlohmann::json jsonLightComponent;

        jsonLightComponent["CastShadows"] = lightComponent.castShadows;
        jsonLightComponent["Type"] = lightComponent.type;
        jsonLightComponent["Constant"] = lightComponent.constant;
        jsonLightComponent["Linear"] = lightComponent.linear;
        jsonLightComponent["Quadratic"] = lightComponent.quadratic;
        jsonLightComponent["InnerRadius"] = lightComponent.innerRadius;
        jsonLightComponent["OuterRadius"] = lightComponent.outerRadius;
        jsonLightComponent["LightProjectionAspect"] = lightComponent.getLightProjectionAspect();
        auto fbo = lightComponent.getFBO();
        jsonLightComponent["ShadowMapSize"] = fbo ? fbo->getSize().first : 0;

        nlohmann::json jsonLightColor = nlohmann::json::array();
        jsonLightColor.push_back(lightComponent.color.x);
        jsonLightColor.push_back(lightComponent.color.y);
        jsonLightColor.push_back(lightComponent.color.z);
        jsonLightComponent["Color"] = jsonLightColor;
            
        jsonEntity["LightComponent"] = jsonLightComponent;
    }

    void SceneSerializer::deserializeLightComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "LightComponent")
            return;
        if(!entity.hasComponent<LightComponent>())
            entity.addComponent<LightComponent>();
        auto& lightComponent = entity.getComponent<LightComponent>();

        lightComponent.castShadows = jsonComponent["CastShadows"];
        lightComponent.constant = jsonComponent["Constant"];
        lightComponent.linear = jsonComponent["Linear"];
        lightComponent.quadratic = jsonComponent["Quadratic"];
        lightComponent.innerRadius = jsonComponent["InnerRadius"];
        lightComponent.outerRadius = jsonComponent["OuterRadius"];
        lightComponent.type = jsonComponent["Type"];
        lightComponent.setLightProjectionAspect(jsonComponent["LightProjectionAspect"]);
        lightComponent.setShadowMapSize(jsonComponent["ShadowMapSize"]);
        nlohmann::json jsonLightColor = jsonComponent["Color"];
        lightComponent.color = { jsonLightColor[0], jsonLightColor[1], jsonLightColor[2] };
    }

    void SceneSerializer::serializePhysicsComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<PhysicsComponent>())
            return;
        auto& physicsComponent = entity.getComponent<PhysicsComponent>();
        auto& transform = physicsComponent.getRigidBody()->getWorldTransform();
        nlohmann::json jsonPhysicsComponent;

        jsonPhysicsComponent["Type"] = physicsComponent.getColliderType();
        jsonPhysicsComponent["Mass"] = physicsComponent.getMass();

        auto& position = transform.getOrigin();
        nlohmann::json jsonPosition = nlohmann::json::array();
        jsonPosition.push_back(position.x());
        jsonPosition.push_back(position.y());
        jsonPosition.push_back(position.z());
        jsonPhysicsComponent["Position"] = jsonPosition;

        auto& rotation = physicsComponent.getRotation();
        nlohmann::json jsonRotation = nlohmann::json::array();
        jsonRotation.push_back(rotation.x);
        jsonRotation.push_back(rotation.y);
        jsonRotation.push_back(rotation.z);
        jsonPhysicsComponent["Rotation"] = jsonRotation;

        auto& size = physicsComponent.getShapeDimensions();
        nlohmann::json jsonSize = nlohmann::json::array();
        jsonSize.push_back(size.x);
        jsonSize.push_back(size.y);
        jsonSize.push_back(size.z);
        jsonPhysicsComponent["Size"] = jsonSize;

        auto& localScale = physicsComponent.getLocalScale();
        nlohmann::json jsonLocalScale = nlohmann::json::array();
        jsonLocalScale.push_back(localScale.x);
        jsonLocalScale.push_back(localScale.y);
        jsonLocalScale.push_back(localScale.z);
        jsonPhysicsComponent["LocalScale"] = jsonLocalScale;

        jsonPhysicsComponent["IsRotated"] = physicsComponent.getIsRotated();
        jsonPhysicsComponent["IsTrigger"] = physicsComponent.getIsTrigger();

        jsonEntity["PhysicsComponent"] = jsonPhysicsComponent;
    }

    void SceneSerializer::deserializePhysicsComponent(Scene* scene, Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "PhysicsComponent")
            return;
        if(entity.hasComponent<PhysicsComponent>())
            return;
        nlohmann::json jsonPosition = jsonComponent["Position"];
        nlohmann::json jsonRotation = jsonComponent["Rotation"];
        nlohmann::json jsonSize = jsonComponent["Size"];
        nlohmann::json jsonLocalScale = jsonComponent["LocalScale"];
        ColliderType type = static_cast<ColliderType>(jsonComponent["Type"]);
        glm::vec3 size = {jsonSize[0], jsonSize[1], jsonSize[2]};
        glm::vec3 localScale = {jsonLocalScale[0], jsonLocalScale[1], jsonLocalScale[2]};
        glm::vec3 position = {jsonPosition[0], jsonPosition[1], jsonPosition[2]};
        glm::vec3 rotation = {jsonRotation[0], jsonRotation[1], jsonRotation[2]};
        bool isRotated = jsonComponent["IsRotated"];
        bool isTrigger = jsonComponent["IsTrigger"];
        float mass = jsonComponent["Mass"];
        if(type != ColliderType::TriangleMesh) {
            auto& physicsComponent = entity.addComponent<PhysicsComponent>(scene->getDynamicWorld(), type, size, localScale, position, rotation, mass, entity.getSource());
            physicsComponent.setIsRotated(isRotated);
            physicsComponent.setIsTrigger(isTrigger);
        }
        else {
            auto& meshComponent = entity.getComponent<MeshComponent>();
            TriangleMeshSpecification triangleMesh = { meshComponent.vbo, meshComponent.ebo };
            auto& physicsComponent = entity.addComponent<PhysicsComponent>(scene->getDynamicWorld(), type, triangleMesh, localScale, position, rotation, entity.getSource());
            physicsComponent.setIsTrigger(isTrigger);
        }
    }

    void SceneSerializer::serializeScriptComponent(Entity& entity, nlohmann::json& jsonEntity) {
        if(!entity.hasComponent<ScriptComponent>())
            return;
        auto& scriptComponent = entity.getComponent<ScriptComponent>();
        nlohmann::json jsonScriptComponent;

        nlohmann::json jsonBehaviors = nlohmann::json::array();
        auto& scripts = scriptComponent.getScripts();
        for(auto& script : scripts) {
            nlohmann::json jsonBehavior;
            jsonBehavior["BehaviorClassName"] = script.behaviorClassName;
            jsonBehavior["IsEnabled"] = script.isEnabled;
            jsonBehaviors.push_back(jsonBehavior);
        }
        jsonScriptComponent["Behaviors"] = jsonBehaviors;

        jsonEntity["ScriptComponent"] = jsonScriptComponent;
    }

    void SceneSerializer::deserializeScriptComponent(Scene* scene, Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "ScriptComponent")
            return;
        if(!entity.hasComponent<ScriptComponent>())
            entity.addComponent<ScriptComponent>();
        auto& scriptComponent = entity.getComponent<ScriptComponent>();

        for(auto& [index, behavior] : jsonComponent["Behaviors"].items()) {
            std::string behaviorClassName = behavior["BehaviorClassName"];
            bool isEnabled = behavior["IsEnabled"];
            auto scriptDLLData = scene->_scriptDLLRegistry->get(behaviorClassName);
            if(scriptDLLData && scriptDLLData->isValid) {
                auto behaviorFactory = DLLCreator::loadDLLFunc(*scriptDLLData);
                if(behaviorFactory) {
                    Behavior* behavior = (Behavior*)behaviorFactory();
                    if(!behavior)
                        continue;
                    auto scriptSpec = scriptComponent.bind(behavior, scriptDLLData->scriptName);
                    if(scriptSpec)
                        scriptSpec->isEnabled = isEnabled;
                }
            }
        }
    }

    void SceneSerializer::serializeParentChildsComponent(Entity& entity, nlohmann::json& jsonEntity, Scene* scene) {
        auto& parentChildsComponent = entity.getComponent<ParentChildsComponent>();
        nlohmann::json jsonParentChildsComponent;

        entt::entity parentId = entt::null;
        if(parentChildsComponent.parent != entt::null) {
            Entity parentEntity = { parentChildsComponent.parent, scene };
            auto& idComponent = parentEntity.getComponent<IDComponent>();
            parentId = (entt::entity)idComponent.id;
        }
        jsonParentChildsComponent["Parent"] = parentId;

        nlohmann::json jsonChilds = nlohmann::json::array();
        for(auto child : parentChildsComponent.childs) {
            Entity childEntity = { child, scene };
            auto& idComponent = childEntity.getComponent<IDComponent>();
            jsonChilds.push_back(idComponent.id);
        }
        jsonParentChildsComponent["Childs"] = jsonChilds;

        jsonEntity["ParentChildsComponent"] = jsonParentChildsComponent;
    }

    void SceneSerializer::deserializeParentChildsComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "ParentChildsComponent")
            return;
        auto& parentChildsComponent = entity.getComponent<ParentChildsComponent>();

        parentChildsComponent.parent = jsonComponent["Parent"];
        
        for(entt::entity childEntity : jsonComponent["Childs"])
            parentChildsComponent.childs.push_back(childEntity);
    }

    void SceneSerializer::deserializeIDComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent) {
        if(key != "Entity ID")
            return;
        auto& idComponent = entity.getComponent<IDComponent>();
        idComponent.id = jsonComponent;
    }

    void SceneSerializer::serializeAudioComponent(Entity& entity, nlohmann::json& jsonEntity, ProjectData* projectData) {
        if(!entity.hasComponent<AudioComponent>())
            return;
        auto& audioComponent = entity.getComponent<AudioComponent>();
        nlohmann::json jsonAudioComponent;
        nlohmann::json jsonSoundSources = nlohmann::json::array();

        auto& soundSources = audioComponent.getSoundSources();
        for(auto soundSource : soundSources) {
            nlohmann::json jsonSoundSource;
            jsonSoundSource["Is3D"] = soundSource->getIs3D();
            jsonSoundSource["PlayLooped"] = soundSource->getPlayLooped();
            jsonSoundSource["StartPaused"] = soundSource->getStartPaused();
            jsonSoundSource["Volume"] = soundSource->getVolume();
            jsonSoundSource["MinDistance"] = soundSource->getMinDistance();
            jsonSoundSource["MaxDistance"] = soundSource->getMaxDistance();
            jsonSoundSource["PlaybackSpeed"] = soundSource->getPlaybackSpeed();
            jsonSoundSource["SoundSourcePath"] = std::filesystem::relative(soundSource->getSoundSourcePath(), projectData->rootPath).string();
            jsonSoundSources.push_back(jsonSoundSource);
        }
        jsonAudioComponent["SoundSources"] = jsonSoundSources;

        jsonEntity["AudioComponent"] = jsonAudioComponent;
    }

    void SceneSerializer::deserializeAudioComponent(Entity& entity, const std::string& key, nlohmann::json& jsonComponent, const std::filesystem::path& rootPath) {
        if(key != "AudioComponent")
            return;
        if(!entity.hasComponent<AudioComponent>())
            entity.addComponent<AudioComponent>(entity.getScene()->getSceneAudio()->getSoundEngine());
        auto& audioComponent = entity.getComponent<AudioComponent>();
        nlohmann::json jsonSoundSources = jsonComponent["SoundSources"];
        for(auto& jsonSoundSource : jsonSoundSources) {
            std::filesystem::path soundSourcePath = rootPath.string() + '/' + (std::string)jsonSoundSource["SoundSourcePath"];
            auto soundSource = audioComponent.addSoundSource(soundSourcePath, jsonSoundSource["Is3D"]);
            soundSource->setPlayLooped(jsonSoundSource["PlayLooped"]);
            soundSource->setStartPaused(jsonSoundSource["StartPaused"]);
            soundSource->setVolume(jsonSoundSource["Volume"]);
            soundSource->setMinDistance(jsonSoundSource["MinDistance"]);
            soundSource->setMaxDistance(jsonSoundSource["MaxDistance"]);
            soundSource->setPlaybackSpeed(jsonSoundSource["PlaybackSpeed"]);
        }
    }

    std::string SceneSerializer::deleteInvertedCommas(const std::string& str) {
        std::string result = str;
        result.erase(0, 0);
        result.erase(result.length(), 1);
        return result;
    }
}