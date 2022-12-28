#include "gui/gui.hpp"

namespace TWE {
    GUI::GUI(GLFWwindow *window) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // io.Fonts->AddFontDefault();
        // static const ImWchar icons_ranges[] = { ICON_MIN_IGFS, ICON_MAX_IGFS, 0 };
        // ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
        // ImGui::GetIO().Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_IGFS, 15.0f, &icons_config, icons_ranges);
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        _specification._scene = nullptr;
        _specification._gizmoOperation = GizmoOperation::Translate;
        _specification.isFileDialogOpen = false;
        _specification.isMouseOnViewport = false;
        
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".scene", ImVec4(1.0f, 1.0f, 0.0f, 1.f));
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".project", ImVec4(1.0f, 1.0f, 0.0f, 1.f));
    }

    GUI::~GUI() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GUI::update() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        showDockSpace();
        processInput();
        showFileDialog();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUI::showDockSpace() {
        static bool opt_dockspace = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &opt_dockspace, window_flags);
        ImGui::PopStyleVar(3);
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New project"))
                    ImGuiFileDialog::Instance()->OpenDialog("NewProject", "Choose File", ".project", ".", 1, nullptr);
                if(ImGui::MenuItem("Open project"))
                    ImGuiFileDialog::Instance()->OpenDialog("OpenProject", "Choose File", ".project", ".", 1, nullptr);
                ImGui::Separator();
                if(ImGui::MenuItem("Save scene"))
                    SceneSerializer::serialize(_specification._scene, _specification.projectData->lastScenePath.string());
                if(ImGui::MenuItem("Save scene as"))
                    ImGuiFileDialog::Instance()->OpenDialog("SaveSceneAs", "Choose File", ".scene", 
                        (_specification.projectData->rootPath.string() + '/').c_str(), 1, nullptr);
                if(ImGui::MenuItem("Load scene"))
                    ImGuiFileDialog::Instance()->OpenDialog("LoadScene", "Choose File", ".scene", ".", 1, nullptr);
                ImGui::Separator();
                bool validateScriptsFlag = _specification._scene->_sceneState != SceneState::Edit;
                if(validateScriptsFlag) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                }
                if(ImGui::MenuItem("Validate scripts"))
                    _specification._scene->validateScripts();
                if(validateScriptsFlag) {
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Options")) {
                if(ImGui::MenuItem("Flag: NoSplit",                "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
                    dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; 
                if(ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
                    dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                if(ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
                if(ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
                    dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        showScenePanel();
        showTestPanel();
        showViewportPanel();
        if(!_specification.projectData->rootPath.empty())
            _directory.showPanel();
        _components.showPanel(_specification._selectedEntity);
        ImGui::End();
    }

    void GUI::selectEntity(Entity& entity) {
        unselectEntity();
        _specification._selectedEntity = entity;
        if(_specification._selectedEntity.hasComponent<MeshRendererComponent>())
            _specification._selectedEntity.getComponent<MeshRendererComponent>().showCollider = true;
    }

    void GUI::unselectEntity() {
        if(_specification._selectedEntity.getSource() == entt::null)
            return;
        if(_specification._selectedEntity.hasComponent<MeshRendererComponent>())
            _specification._selectedEntity.getComponent<MeshRendererComponent>().showCollider = false;
        _specification._selectedEntity = {};
    }

    void GUI::showScenePanel() {
        ImGui::Begin("Scene");
        if(ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
            unselectEntity();
            ImGuiFileDialog::Instance()->Close();
        }
        bool canOpenWindowPopup = true;
        std::string entityPopup = "EntityPopup";
        if(_specification._scene) {
            _specification._scene->_entityRegistry.curEntityRegistry->view<NameComponent>().each([&](entt::entity entity, NameComponent& nameComponent){
                auto id = (void*)entity;
                Entity entityInstance = {entity, _specification._scene};
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | (_specification._selectedEntity == entityInstance ? ImGuiTreeNodeFlags_Selected : 0);
                bool isOpened = ImGui::TreeNodeEx(id, flags, nameComponent.getName().c_str());
                if(ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
                    selectEntity(entityInstance);selectEntity(entityInstance);
                    ImGuiFileDialog::Instance()->Close();
                }
                if(ImGui::IsItemClicked(1)) {
                    ImGui::OpenPopup(entityPopup.c_str());
                    canOpenWindowPopup = false;
                    ImGuiFileDialog::Instance()->Close();
                }
                if(isOpened)
                    ImGui::TreePop();
            });
        }
        std::string menuPopup = "MenuPopup";
        if(ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
            ImGui::SetWindowFocus();
            if(canOpenWindowPopup)
                ImGui::OpenPopup(menuPopup.c_str());
        }
        showSceneMenuPopup(menuPopup);
        showSceneEntityPopup(entityPopup);
        ImGui::End();
    }

    void GUI::showSceneEntityPopup(const std::string& popupId) {
        float popUpWidth = 150.f;
        ImGui::SetNextWindowSize({popUpWidth, 0.f});
        if(ImGui::BeginPopup(popupId.c_str())) {
            auto& availSize = ImGui::GetContentRegionAvail();
            if(ImGui::Button("Remove", {availSize.x, 0.f})) {
                _specification._selectedEntity.destroy();
                unselectEntity();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void GUI::showSceneMenuPopup(const std::string& popupId) {
        float popUpWidth = 150.f;
        ImGui::SetNextWindowSize({popUpWidth, 0.f});
        if(ImGui::BeginPopup(popupId.c_str())) {
            auto& availSize = ImGui::GetContentRegionAvail();
            if(ImGui::BeginMenu("Create entity")) {
                if(ImGui::Button("Entity", {availSize.x, 0.f})) {
                    selectEntity(_specification._scene->createEntity());
                    ImGui::CloseCurrentPopup();
                }       
                if(ImGui::Button("Cube", {availSize.x, 0.f})) {
                    selectEntity(Shape::createCubeEntity(_specification._scene));
                    ImGui::CloseCurrentPopup();
                }       
                if(ImGui::Button("Plate", {availSize.x, 0.f})) {
                    selectEntity(Shape::createPlateEntity(_specification._scene));
                    ImGui::CloseCurrentPopup();
                }       
                if(ImGui::Button("Camera", {availSize.x, 0.f})) {
                    selectEntity(Shape::createCameraEntity(_specification._scene));
                    ImGui::CloseCurrentPopup();
                }       
                if(ImGui::Button("Point Light", {availSize.x, 0.f})) {
                    selectEntity(Shape::createPointLightEntity(_specification._scene));
                    ImGui::CloseCurrentPopup();
                }            
                if(ImGui::Button("Spot Light", {availSize.x, 0.f})) {
                    selectEntity(Shape::createSpotLightEntity(_specification._scene));
                    ImGui::CloseCurrentPopup();
                }            
                if(ImGui::Button("Dir Light", {availSize.x, 0.f})) {
                    selectEntity(Shape::createDirLightEntity(_specification._scene));
                    ImGui::CloseCurrentPopup();
                }
                if(ImGui::Button("Model", {availSize.x, 0.f})) {
                    ImGuiFileDialog::Instance()->OpenDialog("LoadModel", "Choose File", ".obj,.fbx", ".", 1, nullptr);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
    }

    void GUI::showTestPanel() {
        ImGui::Begin("Test");
        for(auto inputText : _inputTextes)
            ImGui::InputText(inputText.first, &inputText.second);
        for(auto button : _buttons)
            if(ImGui::Button(button.first))
                button.second();
        for(auto checkbox : _checkBoxes)
            ImGui::Checkbox(checkbox.first, &checkbox.second);
        ImGui::End();
    }

    void GUI::showViewportStatePanel() {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;
        ImGui::Begin("##ViewportState", 0, flags);

        bool playFlag = _specification._scene->_sceneState != SceneState::Edit;
        if(playFlag) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        if(ImGui::Button("Play")) {
            unselectEntity();
            _specification._scene->setState(SceneState::Run);
        }
        if(playFlag) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if(!playFlag) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Stop")) {
            unselectEntity();
            _specification._scene->setState(SceneState::Edit);
        }
        ImGui::SameLine();
        bool pauseFlag = !playFlag || _specification._scene->_sceneState == SceneState::Run;
        if(ImGui::Button(pauseFlag ? "Pause" : "Unpause"))
            if(pauseFlag)
                _specification._scene->setState(SceneState::Pause);
            else
                _specification._scene->_sceneState = SceneState::Run;
        if(!playFlag) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        ImGui::End();
    }

    void GUI::showViewportPanel() {
        ImGui::Begin("Viewport");
        showViewportStatePanel();
        auto& cursorPos = ImGui::GetCursorPos();
        if(ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
            ImGui::SetWindowFocus();
        _specification.isFocusedOnViewport = ImGui::IsWindowFocused();
        ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
        auto frameSize = _specification._scene->_frameBuffer->getSize();
        if(viewPortSize.x != frameSize.first || viewPortSize.y != frameSize.second)
            _specification._scene->_frameBuffer->resize(viewPortSize.x, viewPortSize.y);
        auto frameId = (void*)(uint64_t)_specification._scene->_frameBuffer->getColorAttachment(0);
        ImGui::Image(frameId, viewPortSize, {0, 1}, {1, 0});

        auto& windowSize = ImGui::GetWindowSize();
        auto& minBound = ImGui::GetWindowPos();
        minBound.x += cursorPos.x;
        minBound.y += cursorPos.y;

        auto& mousePos = ImGui::GetMousePos();
        mousePos.x -= minBound.x;
        mousePos.y -= minBound.y;
        mousePos.y = viewPortSize.y - mousePos.y;
        if(_specification._scene->getIsFocusedOnDebugCamera() && !_specification.isFileDialogOpen) {
            bool isUsing = showGizmo();
            if(mousePos.x >= 0.f && mousePos.y >= 0.f && mousePos.x < viewPortSize.x && mousePos.y < viewPortSize.y) {
                _specification.isMouseOnViewport = true;
                if(!isUsing && !ImGuiFileDialog::Instance()->IsOpened() && ImGui::IsMouseClicked(0)) {
                    int data = _specification._scene->_frameBuffer->readPixel(1, (int)mousePos.x, (int)mousePos.y);
                    if(data == -1 || !_specification._scene->_entityRegistry.curEntityRegistry->valid((entt::entity)data))
                        unselectEntity();
                    else
                        selectEntity(Entity{ (entt::entity)data, _specification._scene });
                }
            } else
                _specification.isMouseOnViewport = false;
        }
        ImGui::End();
    }

    void GUI::showFileDialog() {
        if(ImGuiFileDialog::Instance()->Display("NewProject"))  {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string projectName = std::filesystem::path(filePathName).stem().string();
                std::string projectDir = std::filesystem::path(filePathName).parent_path().string();
                if(ProjectCreator::create(projectName, projectDir)) {
                    std::string projectFilePath = projectDir + '/' + projectName + '/' + projectName + ".project";
                    auto projectData = ProjectCreator::load(projectFilePath);
                    if(projectData) {
                        DLLCreator::initPaths(projectData->dllTempDir.string());
                        *_specification.projectData = *projectData;
                        _directory.setCurrentPath(projectData->rootPath);
                        _specification._scene->reset();
                        if(!projectData->lastScenePath.empty())
                            SceneSerializer::deserialize(_specification._scene, projectData->lastScenePath.string());
                    }
                }
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("OpenProject"))  {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string projectFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                auto projectData = ProjectCreator::load(projectFilePath);
                if(projectData) {
                    DLLCreator::initPaths(projectData->dllTempDir.string());
                    *_specification.projectData = *projectData;
                    _directory.setCurrentPath(projectData->rootPath);
                    _specification._scene->reset();
                    if(!projectData->lastScenePath.empty())
                        SceneSerializer::deserialize(_specification._scene, projectData->lastScenePath.string());
                }
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("SaveSceneAs"))  {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                SceneSerializer::serialize(_specification._scene, filePathName);
                _specification.projectData->lastScenePath = filePathName;
                ProjectCreator::save(_specification.projectData);
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("LoadScene"))  {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                unselectEntity();
                _specification._scene->reset();
                SceneSerializer::deserialize(_specification._scene, filePathName);
                _specification.projectData->lastScenePath = filePathName;
                ProjectCreator::save(_specification.projectData);
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))  {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            }
            _specification.isFileDialogOpen = false;
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("LoadModel"))  {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                ModelLoader mLoader;
                ModelLoaderData* model = mLoader.loadModel(filePathName);
                if(model){
                    auto& modelEntities = Shape::createModelEntity(_specification._scene, model);
                    if(!modelEntities.empty())
                        selectEntity(modelEntities.back());
                }
            }
            _specification.isFileDialogOpen = false;
            ImGuiFileDialog::Instance()->Close();
            return;
        }
    }

    bool GUI::showGizmo() {
        if(_specification._gizmoOperation == GizmoOperation::None || _specification._selectedEntity.getSource() == entt::null)
            return false;
        Camera* camera = _specification._scene->_sceneCameraSpecification.camera;
        if(!camera)
            return false;
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        auto& windowPos = ImGui::GetWindowPos();
        auto& windowSize = ImGui::GetWindowSize();
        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

        const glm::mat4& cameraProjection = camera->getProjection();
        glm::mat4 cameraView = camera->getView(_specification._scene->_sceneCameraSpecification.position, 
            _specification._scene->_sceneCameraSpecification.forward, _specification._scene->_sceneCameraSpecification.up);
        auto& selectedEntityTransform = _specification._selectedEntity.getComponent<TransformComponent>();
        auto selectedEntityModel = selectedEntityTransform.model;

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), static_cast<ImGuizmo::OPERATION>(_specification._gizmoOperation),
                             ImGuizmo::LOCAL, glm::value_ptr(selectedEntityModel));

        bool isUsing = ImGuizmo::IsUsing();

        if(isUsing) {
            glm::vec3 position, rotation, size;
            Math::decomposeTransform(selectedEntityModel, position, rotation, size);
            bool hasPhysics = _specification._selectedEntity.hasComponent<PhysicsComponent>();
            selectedEntityTransform.setPosition(position);
            if(hasPhysics)
                _specification._selectedEntity.getComponent<PhysicsComponent>().setPosition(position);
            selectedEntityTransform.setRotation(rotation);
            if(hasPhysics)
                _specification._selectedEntity.getComponent<PhysicsComponent>().setRotation(glm::quat(rotation));
            selectedEntityTransform.setSize(size);
            if(hasPhysics)
                _specification._selectedEntity.getComponent<PhysicsComponent>().setSize(size);
        }

        return isUsing;
    }

    void GUI::processInput() {
        if(_specification.isFocusedOnViewport && !Input::isMouseButtonPressed(Mouse::MOUSE_BUTTON_RIGHT) 
        && _specification._selectedEntity.getSource() != entt::null) {
            if(Input::isKeyPressed(Keyboard::KEY_Q))
                _specification._gizmoOperation = GizmoOperation::None;
            if(Input::isKeyPressed(Keyboard::KEY_W))
                _specification._gizmoOperation = GizmoOperation::Translate;
            if(Input::isKeyPressed(Keyboard::KEY_E))
                _specification._gizmoOperation = GizmoOperation::Rotate;
            if(Input::isKeyPressed(Keyboard::KEY_R))
                _specification._gizmoOperation = GizmoOperation::Scale;
        }
        if(_specification._selectedEntity.getSource() != entt::null) {
            if(Input::isKeyPressed(Keyboard::KEY_DELETE)) {
                _specification._selectedEntity.destroy();
                unselectEntity();
            }
        }
        if(Input::isKeyPressed(Keyboard::KEY_LEFT_CONTROL) && Input::isKeyPressed(Keyboard::KEY_S)) {
            if(!_specification.projectData->lastScenePath.empty())
                SceneSerializer::serialize(_specification._scene, _specification.projectData->lastScenePath.string());
            else
                ImGuiFileDialog::Instance()->OpenDialog("SaveSceneAs", "Choose File", ".scene", 
                    (_specification.projectData->rootPath.string() + '/').c_str(), 1, nullptr);
        }
    }

    void GUI::addCheckbox(const char* name, bool& var) {
        _checkBoxes.push_back({name, var});
    }

    void GUI::addInputText(const char* name, std::string& var) {
        _inputTextes.push_back({name, var});
    }

    void GUI::addButton(const char* name, std::function<void()> func) {
        _buttons.push_back({name, func});
    }

    void GUI::setScene(Scene* scene) {
        _specification._scene = scene;
        _components.setScene(scene);
        _directory.setScene(scene);
    }

    void GUI::setProjectData(ProjectData* projectData) {
        _specification.projectData = projectData;
        _directory.setProjectData(projectData);
    }

    bool GUI::getIsMouseOnViewport() { return _specification.isMouseOnViewport; }
    bool GUI::getIsFocusedOnViewport() { return _specification.isFocusedOnViewport; }
}