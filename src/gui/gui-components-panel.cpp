#include "gui/gui-components-panel.hpp"

namespace TWE {
    GUIComponentsPanel::GUIComponentsPanel() {
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(1.0f, 1.0f, 0.0f, 1.f));
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpeg", ImVec4(1.0f, 1.0f, 0.0f, 1.f));
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpg", ImVec4(1.0f, 1.0f, 0.0f, 1.f));
    }

    void GUIComponentsPanel::showPanel(Entity& entity) {
        ImGui::Begin("Components");
        if(ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) 
            ImGui::SetWindowFocus();
        if(entity.getSource() != entt::null) {
            showNameComponent(entity);
            showTransformComponent(entity);
            showMeshComponent(entity);
            showMeshRendererComponent(entity);
            showCameraComponent(entity);
            showLightComponent(entity);
            showPhysicsComponent(entity);
            showScriptComponent(entity);
            showAddComponentMenu(entity);
            showFileDialog(entity);
        }
        ImGui::End();
    }

    void GUIComponentsPanel::showFileDialog(Entity& entity) {
        if(ImGuiFileDialog::Instance()->Display("BaseTexture")) {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                if(entity.hasComponent<MeshComponent>()){
                    auto& meshComponent = entity.getComponent<MeshComponent>();
                    auto newState = meshComponent;
                    TextureSpecification textureSpecification(filePathName, 0, TextureType::Texture2D, TextureInOutFormat::RGBA);
                    newState.texture = std::make_shared<Texture>();
                    newState.texture->setTexture(textureSpecification);
                    _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshComponentStateCommand(entity, newState));
                }
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("CubemapTexture")) {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                auto& selection = ImGuiFileDialog::Instance()->GetSelection();
                if(selection.size() == 6) {
                    static std::vector<std::string> positions = {
                        "posx", "negx",
                        "posy", "negy",
                        "posz", "negz",
                    };
                    TextureAttachmentSpecification attachemnts;
                    for(auto& item : positions)
                        for(auto& [key, value] : selection) {
                            auto position = std::filesystem::path(key).stem().string();
                            if(item == position) {
                                TextureSpecification specification(value, 0, TextureType::CubemapTexture, TextureInOutFormat::RGBA);
                                attachemnts.textureSpecifications.push_back(specification);
                                break;
                            }
                        }
                    if(attachemnts.textureSpecifications.size() == 6) {
                        if(entity.hasComponent<MeshComponent>()){
                            auto& meshComponent = entity.getComponent<MeshComponent>();
                            auto newState = meshComponent;
                            newState.texture = std::make_shared<Texture>(attachemnts);
                            _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshComponentStateCommand(entity, newState));
                        }
                    }
                }
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
    }

    void GUIComponentsPanel::setScene(Scene* scene) {
        _scene = scene;
    }

    void GUIComponentsPanel::showAddComponentMenu(Entity& entity) {
        ImGui::Dummy({0.f, 5.f});
        std::string popUpId = guiPopups[GUIPopupIds::AddComponentPopup];
        if(ImGui::Button("Add component", { ImGui::GetContentRegionAvail().x, 20.f }))
            ImGui::OpenPopup(popUpId.c_str());
        float popUpWidth = 150.f;
        ImGui::SetNextWindowSize({popUpWidth, 0.f});
        if(ImGui::BeginPopup(popUpId.c_str())) {
            auto& availSize = ImGui::GetContentRegionAvail();
            if(!entity.hasComponent<MeshComponent>() || !entity.hasComponent<MeshRendererComponent>()) {
                if(ImGui::Button("Mesh component", {availSize.x, 0.f})) {
                    _scene->_sceneRegistry.current->urControl.execute(new AddMeshCompoentnCommand(entity));
                    ImGui::CloseCurrentPopup();
                }
            }
            if(!entity.hasComponent<CameraComponent>()) {
                if(ImGui::Button("Camera component", {availSize.x, 0.f})) {
                    _scene->_sceneRegistry.current->urControl.execute(new AddCameraComponentCommand(entity));
                    ImGui::CloseCurrentPopup();
                }
            }
            if(!entity.hasComponent<LightComponent>()) {
                if(ImGui::Button("Light component", {availSize.x, 0.f})) {
                    _scene->_sceneRegistry.current->urControl.execute(new AddLightComponentCommand(entity));
                    ImGui::CloseCurrentPopup();
                }
            }
            if(!entity.hasComponent<PhysicsComponent>()) {
                if(ImGui::Button("Physics component", {availSize.x, 0.f})) {
                    _scene->_sceneRegistry.current->urControl.execute(new AddPhysicsComponentCommand(entity));
                    ImGui::CloseCurrentPopup();
                }
            }
            if(!entity.hasComponent<ScriptComponent>()) {
                if(ImGui::Button("Script component", {availSize.x, 0.f})) {
                    _scene->_sceneRegistry.current->urControl.execute(new AddScriptComponentCommand(entity));
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }

    void GUIComponentsPanel::showNameComponent(Entity& entity) {
        if(entity.hasComponent<NameComponent>()) {
            auto& nameComponent = entity.getComponent<NameComponent>();
            auto& name = nameComponent.getName();
            ImGui::Text("Name");
            ImGui::SameLine();
            if(ImGui::InputText("##Name", &name)) {
                auto newState = nameComponent;
                newState.setName(name);
                _scene->_sceneRegistry.current->urControl.execute(new ChangeNameComponentStateCommand(entity, newState));
            }
            showSeparator();
        }
    }

    void GUIComponentsPanel::showTransformComponent(Entity& entity) {
        if(entity.hasComponent<TransformComponent>()) {
            auto id = (void*)(typeid(TransformComponent).hash_code());
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if(ImGui::TreeNodeEx(id, flags, "Transform")) {
                auto& transformComponent = entity.getComponent<TransformComponent>();
                auto position = transformComponent.transform.position;
                static bool addToURControl = false;
                static TransformComponent oldState;
                static TransformComponent newState;
                if(!addToURControl)
                    oldState = transformComponent;
                if(GUIComponents::dragFloat3("Position", position, 0.05f, 0.f)) {
                    addToURControl = true;
                    transformComponent.setPosition(position);
                    newState = transformComponent;
                }
                auto rotation = transformComponent.transform.rotation * 180.f / PI;
                if(GUIComponents::dragFloat3("Rotation", rotation, 0.5f, 0.f)) {
                    addToURControl = true;
                    rotation *= PI / 180.f;
                    transformComponent.setRotation(rotation);
                    newState = transformComponent;
                }
                auto size = transformComponent.transform.size;
                if(GUIComponents::dragFloat3("Scale", size, 0.01f, 1.f, 0.01f, 999999.f)) {
                    addToURControl = true;
                    transformComponent.setSize(size);
                    newState = transformComponent;
                }
                if(Input::mouseButtonAction(Mouse::MOUSE_BUTTON_LEFT) == Action::RELEASE && addToURControl) {
                    addToURControl = false;
                    if(oldState.transform != newState.transform)
                        _scene->_sceneRegistry.current->urControl.execute(new ChangeTransformComponentState(entity, oldState, newState));
                }
                ImGui::TreePop();
            }
            showSeparator();
        }
    }

    void GUIComponentsPanel::showSeparator() {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.f);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.2f);
        ImGui::Separator();
        ImGui::PopStyleVar(1);
    }

    void GUIComponentsPanel::showMeshComponent(Entity& entity) {
        if(entity.hasComponent<MeshComponent>()) {
            auto id = (void*)(typeid(MeshComponent).hash_code());
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if(ImGui::TreeNodeEx(id, flags, "Mesh")) {
                std::string popUpId = guiPopups[GUIPopupIds::MeshPopup];
                if(ImGui::IsItemClicked(1))
                    ImGui::OpenPopup(popUpId.c_str());
                auto& meshComponent = entity.getComponent<MeshComponent>();
                std::vector<std::string> meshRegistryKeys = Shape::meshRegistry->getKeys();
                int meshIndex = GUIComponents::combo("Mesh", meshComponent.registryId, meshRegistryKeys);
                if(meshIndex != -1) {
                    auto newState = meshComponent;
                    std::string meshId = meshRegistryKeys[meshIndex];
                    auto meshSpecification = Shape::meshRegistry->get(meshId);
                    newState.setMesh(meshSpecification->vao, meshSpecification->vbo, meshSpecification->ebo, meshId);
                    newState.setModelPath(meshSpecification->modelPath);
                    entity.getComponent<CreationTypeComponent>().setType(meshSpecification->creationType);
                    _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshComponentStateCommand(entity, newState));
                }
                auto textureId = (void*)(typeid(Texture).hash_code());
                bool isCubemap = false;
                if(entity.hasComponent<MeshRendererComponent>()) {
                    auto& meshRendererComponent = entity.getComponent<MeshRendererComponent>();
                    if(meshRendererComponent.registryId == "Cubemap renderer")
                        isCubemap = true;
                }
                if(ImGui::TreeNodeEx(id, flags, "Texture")) {
                    auto baseTexture = meshComponent.texture->getTextureSpecByTexNumber(0);
                    std::function<void()> baseDragAndDropFunc;
                    if(isCubemap)
                        baseDragAndDropFunc = [](){};
                    else
                        baseDragAndDropFunc = [&]() {
                            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(guiDragAndDropTypes[GUIDragAndDropType::DirectoryItem].c_str())) {
                                auto newState = meshComponent;
                                const wchar_t* item = static_cast<const wchar_t*>(payload->Data);
                                std::filesystem::path path(item);
                                TextureSpecification textureSpecification(path.string(), 0, TextureType::Texture2D, TextureInOutFormat::RGBA);
                                newState.texture = std::make_shared<Texture>();
                                newState.texture->setTexture(textureSpecification);
                                _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshComponentStateCommand(entity, newState));
                            }
                        };
                    int baseBtnPressed = GUIComponents::imageButton("Base", baseTexture ? (void*)(uint64_t)baseTexture->id : (void*)(uint64_t)0, { 20.f, 20.f }, baseDragAndDropFunc);
                    if(baseBtnPressed == 0)
                        if(isCubemap) {
                            ImGuiFileDialog::Instance()->OpenDialog("CubemapTexture", "Choose File", ".png,.jpeg,.jpg", ".", 6, nullptr);
                        } else
                            ImGuiFileDialog::Instance()->OpenDialog("BaseTexture", "Choose File", ".png,.jpeg,.jpg", ".", 1, nullptr);
                    else if(baseBtnPressed == 1) {
                        auto newState = meshComponent;
                        if(isCubemap) {
                            int size = meshComponent.texture->getAttachments().textureSpecifications.size();
                            newState.texture = std::make_shared<Texture>(meshComponent.texture->getAttachments());
                            for(int i = 0; i < size; ++i)
                                newState.texture->removeTexture(0);
                            _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshComponentStateCommand(entity, newState));
                        } else {
                            newState.texture = std::make_shared<Texture>(meshComponent.texture->getAttachments());
                            newState.texture->removeTexture(0);
                            _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshComponentStateCommand(entity, newState));
                        }
                    }
                    ImGui::TreePop();
                }
                float popUpWidth = 150.f;
                ImGui::SetNextWindowSize({popUpWidth, 0.f});
                if(ImGui::BeginPopup(popUpId.c_str())) {
                    auto& availSize = ImGui::GetContentRegionAvail();
                    if(ImGui::Button("Remove component", { availSize.x, 0.f })) {
                        _scene->_sceneRegistry.current->urControl.execute(new RemoveMeshComponentCommand(entity));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::TreePop();
            }
            showSeparator();
        }
    }

    void GUIComponentsPanel::showMeshRendererComponent(Entity& entity) {
        if(entity.hasComponent<MeshRendererComponent>()) {
            auto id = (void*)(typeid(MeshRendererComponent).hash_code());
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if(ImGui::TreeNodeEx(id, flags, "Mesh Renderer")) {
                std::string popUpId = guiPopups[GUIPopupIds::MeshRendererPopup];
                if(ImGui::IsItemClicked(1))
                    ImGui::OpenPopup(popUpId.c_str());
                auto& meshRendererComponent = entity.getComponent<MeshRendererComponent>();
                std::vector<std::string> meshRendererRegistryKeys = Shape::meshRendererRegistry->getKeys();
                int meshRendererIndex = GUIComponents::combo("Shader", meshRendererComponent.registryId, meshRendererRegistryKeys);
                if(meshRendererIndex != -1) {
                    auto newState = meshRendererComponent;
                    std::string meshRendererId = meshRendererRegistryKeys[meshRendererIndex];
                    auto meshRendererSpecification = Shape::meshRendererRegistry->get(meshRendererId);
                    newState.setShader(meshRendererSpecification->vertexShaderPath.c_str(), 
                        meshRendererSpecification->fragmentShaderPath.c_str(), meshRendererId);
                    _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshRendererComponentState(entity, meshRendererComponent, newState));
                }
                static bool addToURControl = false;
                static MeshRendererComponent oldState;
                static MeshRendererComponent newState;
                if(!addToURControl)
                    oldState = meshRendererComponent;
                if(ImGui::TreeNodeEx(id, flags, "Material")) {
                    glm::vec3 color = meshRendererComponent.material.objColor;
                    if(GUIComponents::colorEdit3("Color", color)) {
                        addToURControl = true;
                        meshRendererComponent.material.objColor = color;
                        newState = meshRendererComponent;
                    }
                    float ambient = meshRendererComponent.material.ambient;
                    if(GUIComponents::dragFloat("Ambient", ambient, 0.01f)) {
                        addToURControl = true;
                        meshRendererComponent.material.ambient = ambient;
                        newState = meshRendererComponent;
                    }
                    float diffuse = meshRendererComponent.material.diffuse;
                    if(GUIComponents::dragFloat("Diffuse", diffuse, 0.1f, 0.1f, 999999.f)) {
                        addToURControl = true;
                        meshRendererComponent.material.diffuse = diffuse;
                        newState = meshRendererComponent;
                    }
                    float specular = meshRendererComponent.material.specular;
                    if(GUIComponents::dragFloat("Specular", specular, 0.1f, 0.f, 999999.f)) {
                        addToURControl = true;
                        meshRendererComponent.material.specular = specular;
                        newState = meshRendererComponent;
                    }
                    float shininess = meshRendererComponent.material.shininess;
                    if(GUIComponents::dragFloat("Shininess", shininess, 1.f, 12.f, 999999.f)) {
                        addToURControl = true;
                        meshRendererComponent.material.shininess = shininess;
                        newState = meshRendererComponent;
                    }
                    if(Input::mouseButtonAction(Mouse::MOUSE_BUTTON_LEFT) == Action::RELEASE && addToURControl) {
                        addToURControl = false;
                        if(oldState.material != newState.material)
                            _scene->_sceneRegistry.current->urControl.execute(new ChangeMeshRendererComponentState(entity, oldState, newState));
                    }
                    ImGui::TreePop();
                }
                float popUpWidth = 150.f;
                ImGui::SetNextWindowSize({popUpWidth, 0.f});
                if(ImGui::BeginPopup(popUpId.c_str())) {
                    auto& availSize = ImGui::GetContentRegionAvail();
                    if(ImGui::Button("Remove component", { availSize.x, 0.f })) {
                        _scene->_sceneRegistry.current->urControl.execute(new RemoveMeshRendererComponentCommand(entity));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::TreePop();
            }
            showSeparator();
        }
    }

    void GUIComponentsPanel::showCameraComponent(Entity& entity) {
        if(entity.hasComponent<CameraComponent>()) {
            auto id = (void*)(typeid(CameraComponent).hash_code());
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if(ImGui::TreeNodeEx(id, flags, "Camera")) {
                std::string popUpId = guiPopups[GUIPopupIds::CameraPopup];
                if(ImGui::IsItemClicked(1))
                    ImGui::OpenPopup(popUpId.c_str());
                auto& cameraComponent = entity.getComponent<CameraComponent>();
                auto* cameraSource = cameraComponent.getSource();
                static bool addToURControl = false;
                static CameraComponent oldState;
                static CameraComponent newState;
                if(!addToURControl)
                    oldState = cameraComponent;
                int typeIndex = GUIComponents::combo("Projection", cameraProjectionTypes[cameraSource->getType()], cameraProjectionTypes, 105.f);
                if(typeIndex == CameraProjectionType::Perspective) {
                    addToURControl = true;
                    auto& perspectiveSpecification = cameraComponent.getSource()->getPerspectiveSpecification();
                    cameraComponent.getSource()->setPerspective(perspectiveSpecification.fov, perspectiveSpecification.wndWidth, 
                        perspectiveSpecification.wndHeight, perspectiveSpecification.nearDepth, perspectiveSpecification.farDepth);
                    newState = cameraComponent;
                } else if(typeIndex == CameraProjectionType::Orthographic) {
                    addToURControl = true;
                    auto& orthographicSpecification = cameraComponent.getSource()->getOrthographicSpecification();
                    cameraComponent.getSource()->setOrthographic(orthographicSpecification.left, orthographicSpecification.right, 
                        orthographicSpecification.bottom, orthographicSpecification.top, orthographicSpecification.nearDepth, 
                        orthographicSpecification.farDepth);
                    newState = cameraComponent;
                }
                auto isFocusedOn = cameraComponent.isFocusedOn();
                if(GUIComponents::checkBox("Is focused on", isFocusedOn, 105.f)) {
                    cameraComponent.setFocuse(isFocusedOn);
                    newState = cameraComponent;
                    _scene->_sceneRegistry.current->urControl.execute(new ChangeCameraComponentStateCommand(entity, oldState, newState));
                }
                if(cameraSource->getType() == CameraProjectionType::Perspective) {
                    auto& perspectiveSpecification = cameraSource->getPerspectiveSpecification();
                    float fov = perspectiveSpecification.fov;
                    if(GUIComponents::dragFloat("FOV", fov, 1.f, 1.f, 140.f, 105.f)) {
                        addToURControl = true;
                        cameraComponent.getSource()->setPerspective(fov, perspectiveSpecification.wndWidth, 
                            perspectiveSpecification.wndHeight, perspectiveSpecification.nearDepth, perspectiveSpecification.farDepth);
                        newState = cameraComponent;
                    }
                } else if(cameraSource->getType() == CameraProjectionType::Orthographic) {
                    auto& orthographicSpecification = cameraSource->getOrthographicSpecification();
                    auto aspect = orthographicSpecification.right;
                    if(GUIComponents::dragFloat("Aspect", aspect, 0.2f, 0.2f, 100.f, 105.f)) {
                        addToURControl = true;
                        cameraComponent.getSource()->setOrthographic(-aspect, aspect, -aspect, aspect, 
                            orthographicSpecification.nearDepth, orthographicSpecification.farDepth);
                        newState = cameraComponent;
                    }
                }
                if(Input::mouseButtonAction(Mouse::MOUSE_BUTTON_LEFT) == Action::RELEASE && addToURControl) {
                    addToURControl = false;
                    if(*oldState.getSource() != *newState.getSource())
                        _scene->_sceneRegistry.current->urControl.execute(new ChangeCameraComponentStateCommand(entity, oldState, newState));
                }
                float popUpWidth = 150.f;
                ImGui::SetNextWindowSize({popUpWidth, 0.f});
                if(ImGui::BeginPopup(popUpId.c_str())) {
                    auto& availSize = ImGui::GetContentRegionAvail();
                    if(ImGui::Button("Remove component", { availSize.x, 0.f })) {
                        _scene->_sceneRegistry.current->urControl.execute(new RemoveCameraComponentCommand(entity));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::TreePop();
            }
            showSeparator();
        }
    }

    void GUIComponentsPanel::showLightComponent(Entity& entity) {
        if(entity.hasComponent<LightComponent>()) {
            auto id = (void*)(typeid(LightComponent).hash_code());
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if(ImGui::TreeNodeEx(id, flags, "Light")) {
                std::string popUpId = guiPopups[GUIPopupIds::LightPopup];
                if(ImGui::IsItemClicked(1))
                    ImGui::OpenPopup(popUpId.c_str());
                auto& lightComponent = entity.getComponent<LightComponent>();
                std::string selectedType = lightTypes[lightComponent.type];
                static bool addToURControl = false;
                static LightComponent oldState;
                static LightComponent newState;
                if(!addToURControl)
                    oldState = lightComponent;
                int typeIndex = GUIComponents::combo("Type", lightTypes[lightComponent.type], lightTypes, 100.f);
                if(typeIndex > -1) {
                    addToURControl = true;
                    lightComponent.setType(static_cast<LightType>(typeIndex));
                    newState = lightComponent;
                }
                glm::vec3 color = lightComponent.color;
                if(GUIComponents::colorEdit3("Color", color, 100.f)) {
                    addToURControl = true;
                    lightComponent.color = color;
                    newState = lightComponent;
                }
                if(lightComponent.type == LightType::Spot) {
                    float innderRadius = lightComponent.innerRadius;
                    if(GUIComponents::dragFloat("Inner radius", innderRadius, 0.2f, 0.f, 999999.f, 100.f)) {
                        addToURControl = true;
                        lightComponent.innerRadius = innderRadius;
                        newState = lightComponent;
                    }
                    float outerRadius = lightComponent.outerRadius;
                    if(GUIComponents::dragFloat("Outer radius", outerRadius, 0.2f, 0.f, 999999.f, 100.f)) {
                        addToURControl = true;
                        lightComponent.outerRadius = outerRadius;
                        newState = lightComponent;
                    }
                }
                if(lightComponent.type == LightType::Spot || lightComponent.type == LightType::Point) {
                    float constant = lightComponent.constant;
                    if(GUIComponents::dragFloat("Constant", constant, 0.05f, 0.01f, 999999.f, 100.f)) {
                        addToURControl = true;
                        lightComponent.constant = constant;
                        newState = lightComponent;
                    }
                    float linear = lightComponent.linear;
                    if(GUIComponents::dragFloat("Linear", linear, 0.01f, 0.01f, 999999.f, 100.f)) {
                        addToURControl = true;
                        lightComponent.linear = linear;
                        newState = lightComponent;
                    }
                    float quadratic = lightComponent.quadratic;
                    if(GUIComponents::dragFloat("Quadratic", quadratic, 0.001f, 0.001f, 999999.f, 100.f)) {
                        addToURControl = true;
                        lightComponent.quadratic = quadratic;
                        newState = lightComponent;
                    }
                }
                if(lightComponent.type == LightType::Dir) {
                    bool castShadows = lightComponent.castShadows;
                    if(GUIComponents::checkBox("Cast shadows", castShadows, 100.f)) {
                        lightComponent.setCastShadows(lightComponent.castShadows);
                        newState = lightComponent;
                        _scene->_sceneRegistry.current->urControl.execute(new ChangeLightComponentStateCommand(entity, oldState, newState));
                    }
                }
                if(Input::mouseButtonAction(Mouse::MOUSE_BUTTON_LEFT) == Action::RELEASE && addToURControl) {
                    addToURControl = false;
                    if(oldState != newState)
                        _scene->_sceneRegistry.current->urControl.execute(new ChangeLightComponentStateCommand(entity, oldState, newState));
                }
                float popUpWidth = 150.f;
                ImGui::SetNextWindowSize({popUpWidth, 0.f});
                if(ImGui::BeginPopup(popUpId.c_str())) {
                    auto& availSize = ImGui::GetContentRegionAvail();
                    if(ImGui::Button("Remove component", { availSize.x, 0.f })) {
                        _scene->_sceneRegistry.current->urControl.execute(new RemoveLightComponentCommand(entity));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::TreePop();
            }
            showSeparator();
        }
    }

    void GUIComponentsPanel::showPhysicsComponent(Entity& entity) {
        if(entity.hasComponent<PhysicsComponent>()) {
            auto id = (void*)(typeid(PhysicsComponent).hash_code());
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if(ImGui::TreeNodeEx(id, flags, "Physics")) {
                std::string popUpId = guiPopups[GUIPopupIds::PhysicsPopup];
                if(ImGui::IsItemClicked(1))
                    ImGui::OpenPopup(popUpId.c_str());
                auto& physicsComponent = entity.getComponent<PhysicsComponent>();
                physicsComponent.setShowCollider(true);
                ColliderType colliderType = physicsComponent.getColliderType();

                int colliderTypeIndex = GUIComponents::combo("Collider type", colliderTypes[static_cast<int>(colliderType)], colliderTypes, 90.f);
                if(colliderTypeIndex != -1) {
                    ColliderType newColliderType = static_cast<ColliderType>(colliderTypeIndex);
                    _scene->_sceneRegistry.current->urControl.execute(new SetColliderTypePhysicsComponentCommand(entity, newColliderType, 
                    colliderType, entity.getComponent<TransformComponent>().transform.size));
                }

                static bool addToURControl = false;
                static PhysicsComponentProperties oldProperties;
                static PhysicsComponentProperties newProperties;
                if(!addToURControl) {
                    oldProperties = PhysicsComponentProperties{ physicsComponent.getMass(), physicsComponent.getIsRotated(), physicsComponent.getShapeDimensions() };
                    newProperties = oldProperties;
                }
                if(colliderType != ColliderType::TriangleMesh) {
                    auto mass = physicsComponent.getMass();
                    if(GUIComponents::dragFloat("Mass", mass, 0.01f, 0.f, 999999.f, 90.f)) {
                        addToURControl = true;
                        physicsComponent.setMass(mass);
                        newProperties.mass = mass;
                    }
                    bool isRotated = physicsComponent.getIsRotated();
                    if(GUIComponents::checkBox("Is rotated", isRotated, 90.f)) {
                        PhysicsComponentProperties newProperties = { physicsComponent.getMass(), isRotated, physicsComponent.getShapeDimensions() };
                        _scene->_sceneRegistry.current->urControl.execute(new ChangePhysicsComponentPropertiesCommand(entity, oldProperties, newProperties));
                    }
                }

                auto shapeDimensions = physicsComponent.getShapeDimensions();
                switch(colliderType) {
                case ColliderType::Box:
                    if(GUIComponents::dragFloat3("Scale", shapeDimensions, 0.01f, 1.f, 0.01f, 999999.f, 90.f)) {
                        addToURControl = true;
                        physicsComponent.setShapeDimensions(shapeDimensions);
                        newProperties.shapeDimensions = shapeDimensions;
                    }
                    break;
                case ColliderType::Sphere:
                    if(GUIComponents::dragFloat("Radius", shapeDimensions.x, 0.01f, 0.01f, 999999.f, 90.f)) {
                        addToURControl = true;
                        physicsComponent.setShapeDimensions(shapeDimensions);
                        newProperties.shapeDimensions = shapeDimensions;
                    }
                    break;
                case ColliderType::Cylinder:
                case ColliderType::Cone:
                case ColliderType::Capsule:
                    if(GUIComponents::dragFloat("Radius", shapeDimensions.x, 0.01f, 0.01f, 999999.f, 90.f)) {
                        addToURControl = true;
                        physicsComponent.setShapeDimensions(shapeDimensions);
                        newProperties.shapeDimensions = shapeDimensions;
                    }
                    if(GUIComponents::dragFloat("Height", shapeDimensions.y, 0.01f, 0.01f, 999999.f, 90.f)) {
                        addToURControl = true;
                        physicsComponent.setShapeDimensions(shapeDimensions);
                        newProperties.shapeDimensions = shapeDimensions;
                    }
                    break;
                }
                if(Input::mouseButtonAction(Mouse::MOUSE_BUTTON_LEFT) == Action::RELEASE && addToURControl) {
                    addToURControl = false;
                    if(oldProperties != newProperties)
                        _scene->_sceneRegistry.current->urControl.execute(new ChangePhysicsComponentPropertiesCommand(entity, oldProperties, newProperties));
                }
                float popUpWidth = 150.f;
                ImGui::SetNextWindowSize({popUpWidth, 0.f});
                if(ImGui::BeginPopup(popUpId.c_str())) {
                    auto& availSize = ImGui::GetContentRegionAvail();
                    if(ImGui::Button("Remove component", { availSize.x, 0.f })) {
                        _scene->_sceneRegistry.current->urControl.execute(new RemovePhysicsComponentCommand(entity));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::TreePop();
            }
            showSeparator();
        }
    }

    void GUIComponentsPanel::showScriptComponent(Entity& entity) {
        if(entity.hasComponent<ScriptComponent>()) {
            auto id = (void*)(typeid(ScriptComponent).hash_code());
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if(ImGui::TreeNodeEx(id, flags, "Script")) {
                std::string popUpId = guiPopups[GUIPopupIds::ScriptPopup];
                if(ImGui::IsItemClicked(1))
                    ImGui::OpenPopup(popUpId.c_str());
                auto& scriptComponent = entity.getComponent<ScriptComponent>();
                bool isEnabled = scriptComponent.isEnabled;
                if(GUIComponents::checkBox("Enabled", isEnabled))
                    _scene->_sceneRegistry.current->urControl.execute(new SetScriptComponentEnabledStateCommand(entity, isEnabled));
                auto& scriptRegistryKeys = _scene->_scriptDLLRegistry->getKeys();
                int scriptIndex = GUIComponents::combo("Name", scriptComponent.getBehaviorClassName(), scriptRegistryKeys);
                if(scriptIndex != -1)
                    _scene->_sceneRegistry.current->urControl.execute(new SetScriptComponentBehaviorCommand(entity, scriptRegistryKeys[scriptIndex]));
                if(ImGui::BeginPopup(popUpId.c_str())) {
                    auto& availSize = ImGui::GetContentRegionAvail();
                    bool validateScriptFlag = _scene->_sceneState != SceneState::Edit;
                    if(validateScriptFlag) {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    }
                    if(ImGui::Button("Validate", { availSize.x, 0.f })) {
                        _scene->validateScript(scriptComponent.getBehaviorClassName());
                        ImGui::CloseCurrentPopup();
                    }
                    if(validateScriptFlag) {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }
                    if(ImGui::Button("Remove component", { availSize.x, 0.f })) {
                        _scene->_sceneRegistry.current->urControl.execute(new RemoveScriptComponentCommand(entity));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::TreePop();
            }
            showSeparator();
        }
    }
}