#include "gui/gui-scene-panel.hpp"

namespace TWE {
    void GUIScenePanel::showPanel(Entity& selectedEntity) {
        ImGui::Begin("Scene");
        if(ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
            unselectEntity(selectedEntity);
            _showSceneEntity = {};
            ImGuiFileDialog::Instance()->Close();
        }
        _canOpenWindowPopup = true;
        _showSceneEntityPopup = false;
        bool showSeparatorOnce = true;
        std::string entityPopup = guiPopups[GUIPopupIds::EntityPopup];
        if(_scene)  
            _scene->_sceneRegistry.current->entityRegistry.each([&](entt::entity entity){
                Entity entityInstance = {entity, _scene};
                if(!entityInstance.hasComponent<NameComponent>())
                    return;
                auto& parentChildsComponent = entityInstance.getComponent<ParentChildsComponent>();
                if(parentChildsComponent.parent == entt::null) {
                    if(showSeparatorOnce) {
                        showSeparatorOnce = false;
                        showSeparator(entityInstance, true);
                    }
                    showSceneEntity(entityInstance, selectedEntity);
                }
            });
        std::string menuPopup = guiPopups[GUIPopupIds::MenuPopup];
        if(ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
            ImGui::SetWindowFocus();
            if(_canOpenWindowPopup)
                ImGui::OpenPopup(menuPopup.c_str());
        }
        if(_showSceneEntityPopup)
            ImGui::OpenPopup(entityPopup.c_str());
        showSceneMenuPopup(menuPopup, selectedEntity);
        showSceneEntityPopup(entityPopup, selectedEntity);
        auto& availSize = ImGui::GetContentRegionAvail();
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.f);
        ImGui::Button("##Empty", { availSize.x, availSize.y });
        ImGui::PopStyleVar();
        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(guiDragAndDropTypes[GUIDragAndDropType::EntityItem].c_str())) {
                const wchar_t* item = static_cast<const wchar_t*>(payload->Data);
                std::wstring itemWSTR = item;
                entt::entity entityItem = (entt::entity)std::stoi(itemWSTR);
                if(_scene->_sceneRegistry.current->entityRegistry.valid(entityItem)) {
                    Entity entityItemEnt = { entityItem, _scene };
                    _scene->_sceneRegistry.current->urControl.execute(new DragAndDropEntityCommand(entityItemEnt, {}));
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }

    void GUIScenePanel::setScene(Scene* scene) {
        _scene = scene;
    }

    void GUIScenePanel::showSeparator(Entity& entity, bool isUpper) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.2f);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    void GUIScenePanel::showSceneEntity(Entity& entity, Entity& selectedEntity) {
        auto& nameComponent = entity.getComponent<NameComponent>();
        std::string entityPopup = guiPopups[GUIPopupIds::EntityPopup];
        auto id = (void*)entity.getSource();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | (selectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0);
        bool isOpened = ImGui::TreeNodeEx(id, flags, nameComponent.getName().c_str());
        if(ImGui::IsItemClicked(1)) {
            _canOpenWindowPopup = false;
            _showSceneEntityPopup = true;
            _showSceneEntity = entity;
            ImGuiFileDialog::Instance()->Close();
        }
        if(ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
            if(entity.getSource() != selectedEntity.getSource())
                selectEntity(entity, selectedEntity);
            ImGuiFileDialog::Instance()->Close();
        }
        if(ImGui::BeginDragDropSource()) {
            auto item = std::to_wstring((int)selectedEntity.getSource());
            ImGui::SetDragDropPayload(guiDragAndDropTypes[GUIDragAndDropType::EntityItem].c_str(), item.c_str(), (wcslen(item.c_str()) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }
        if(ImGui::BeginDragDropTarget()) {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(guiDragAndDropTypes[GUIDragAndDropType::EntityItem].c_str())) {
                const wchar_t* item = static_cast<const wchar_t*>(payload->Data);
                std::wstring itemWSTR = item;
                entt::entity entityItem = (entt::entity)std::stoi(itemWSTR);
                if(_scene->_sceneRegistry.current->entityRegistry.valid(entityItem)) {
                    if(entityItem != entity.getSource()) {
                        Entity entityItemEnt = { entityItem, _scene };
                        _scene->_sceneRegistry.current->urControl.execute(new DragAndDropEntityCommand(entityItemEnt, entity));
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        showSeparator(entity);
        if(isOpened) {
            auto& parentChildsComponent = entity.getComponent<ParentChildsComponent>();
            for(auto childEnttity : parentChildsComponent.childs) {
                Entity instance = { childEnttity, _scene };
                showSceneEntity(instance, selectedEntity);
            }
            ImGui::TreePop();
        }
    }

    void GUIScenePanel::showSceneEntityPopup(const std::string& popupId, Entity& selectedEntity) {
        float popUpWidth = 150.f;
        ImGui::SetNextWindowSize({popUpWidth, 0.f});
        if(ImGui::BeginPopup(popupId.c_str())) {
            auto& availSize = ImGui::GetContentRegionAvail();
            if(showCreateEntityMenu(selectedEntity)) {
                addEntityToSelected(selectedEntity);
                _scene->_sceneRegistry.current->urControl.execute(new CreateEntityCommand(selectedEntity, 
                    [&](){ unselectEntity(selectedEntity); }));
            }
            ImGui::Separator();
            if(ImGui::Button("Remove", {availSize.x, 0.f})) {
                auto removeEntity = selectedEntity;
                unselectEntity(selectedEntity);
                _scene->_sceneRegistry.current->urControl.execute(new RemoveEntityCommand(removeEntity, 
                    [&](){
                        unselectEntity(selectedEntity);
                    }
                ));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void GUIScenePanel::addEntityToSelected(Entity& selectedEntity) {
        if(_showSceneEntity.getSource() != entt::null) {
            selectedEntity.getComponent<ParentChildsComponent>().parent = _showSceneEntity.getSource();
            _showSceneEntity.getComponent<ParentChildsComponent>().childs.push_back(selectedEntity.getSource());
        }
    }

    bool GUIScenePanel::showCreateEntityMenu(Entity& selectedEntity) {
        bool isInteracted = false;
        auto& availSize = ImGui::GetContentRegionAvail();
        if(ImGui::BeginMenu("Create entity")) {
            if(ImGui::Button("Entity", {availSize.x, 0.f})) {
                selectEntity(_scene->createEntity(), selectedEntity);
                isInteracted = true;
                ImGui::CloseCurrentPopup();
            }       
            if(ImGui::Button("Cube", {availSize.x, 0.f})) {
                selectEntity(Shape::createCubeEntity(_scene), selectedEntity);
                isInteracted = true;
                ImGui::CloseCurrentPopup();
            }       
            if(ImGui::Button("Plate", {availSize.x, 0.f})) {
                selectEntity(Shape::createPlateEntity(_scene), selectedEntity);
                isInteracted = true;
                ImGui::CloseCurrentPopup();
            }       
            if(ImGui::Button("Camera", {availSize.x, 0.f})) {
                selectEntity(Shape::createCameraEntity(_scene), selectedEntity);
                isInteracted = true;
                ImGui::CloseCurrentPopup();
            }       
            if(ImGui::Button("Point Light", {availSize.x, 0.f})) {
                selectEntity(Shape::createPointLightEntity(_scene), selectedEntity);
                isInteracted = true;
                ImGui::CloseCurrentPopup();
            }            
            if(ImGui::Button("Spot Light", {availSize.x, 0.f})) {
                selectEntity(Shape::createSpotLightEntity(_scene), selectedEntity);
                isInteracted = true;
                ImGui::CloseCurrentPopup();
            }            
            if(ImGui::Button("Dir Light", {availSize.x, 0.f})) {
                selectEntity(Shape::createDirLightEntity(_scene), selectedEntity);
                isInteracted = true;
                ImGui::CloseCurrentPopup();
            }
            if(ImGui::Button("Model", {availSize.x, 0.f})) {
                ImGuiFileDialog::Instance()->OpenDialog("LoadModel", "Choose File", ".obj,.fbx", ".", 1, nullptr);
                ImGui::CloseCurrentPopup();
            }
            if(ImGui::Button("Cubemap", {availSize.x, 0.f})) {
                ImGuiFileDialog::Instance()->OpenDialog("LoadCubemap", "Choose File", ".png,.jpg,.jpeg", ".", 6, nullptr);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndMenu();
        }  
        return isInteracted;
    }

    void GUIScenePanel::showSceneMenuPopup(const std::string& popupId, Entity& selectedEntity) {
        float popUpWidth = 150.f;
        ImGui::SetNextWindowSize({popUpWidth, 0.f});
        if(ImGui::BeginPopup(popupId.c_str())) {
            showCreateEntityMenu(selectedEntity);
            ImGui::EndPopup();
        }
    }

    void GUIScenePanel::selectEntity(Entity& entity, Entity& selectedEntity) {
        unselectEntity(selectedEntity);
        selectedEntity = entity;
        if(selectedEntity.hasComponent<PhysicsComponent>())
            selectedEntity.getComponent<PhysicsComponent>().setShowCollider(true);
    }

    void GUIScenePanel::unselectEntity(Entity& entity) {
        if(entity.getSource() == entt::null)
            return;
        if(entity.hasComponent<PhysicsComponent>())
            entity.getComponent<PhysicsComponent>().setShowCollider(false);
        entity = {};
    }

    bool GUIScenePanel::isSceneEntityPopupOpen() { return _showSceneEntityPopup; }
}