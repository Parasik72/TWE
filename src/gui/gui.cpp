#include "gui/gui.hpp"

namespace TWE {
    GUI::GUI(GLFWwindow *window) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("../../include/gui/fonts/Roboto-Bold.ttf", 16.f);
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        _specification._scene = nullptr;
        _specification._gizmoOperation = GizmoOperation::Translate;
        _specification.isMouseOnViewport = false;

        _start.initialize();
        
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
        showFileDialog();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUI::showDockSpace() {
        static bool opt_dockspace = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags window_flags =  ImGuiWindowFlags_NoDocking;
        if(!_specification.projectData->rootPath.empty())
            window_flags |= ImGuiWindowFlags_MenuBar;
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
        if(_specification.projectData->rootPath.empty()) {
            _start.showPanel();
            ImGui::End();
            return;
        }
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New project"))
                    ImGuiFileDialog::Instance()->OpenDialog("NewProject", "Choose File", ".project", 
                        (_specification.projectData->rootPath.string() + '/').c_str(), 1, nullptr);
                if(ImGui::MenuItem("Open project"))
                    ImGuiFileDialog::Instance()->OpenDialog("OpenProject", "Choose File", ".project", 
                        (_specification.projectData->rootPath.string() + '/').c_str(), 1, nullptr);
                ImGui::Separator();
                if(ImGui::MenuItem("Save scene")) {
                    if(!_specification.projectData->lastScenePath.empty())
                        SceneSerializer::serialize(_specification._scene, _specification.projectData->lastScenePath.string(), _specification.projectData);
                    else
                        ImGuiFileDialog::Instance()->OpenDialog("SaveSceneAs", "Choose File", ".scene", 
                            (_specification.projectData->rootPath.string() + '/').c_str(), 1, nullptr);
                }
                if(ImGui::MenuItem("Save scene as"))
                    ImGuiFileDialog::Instance()->OpenDialog("SaveSceneAs", "Choose File", ".scene", 
                        (_specification.projectData->rootPath.string() + '/').c_str(), 1, nullptr);
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
                ImGui::Separator();
                if(ImGui::MenuItem("Build"))
                    if(!_specification.projectData->lastScenePath.empty())
                        BuildCreator::create(_specification.projectData, _specification.projectData->lastScenePath);
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
        showTestPanel();
        showViewportPanel();
        _scene.showPanel(_specification._selectedEntity);
        _directory.showPanel(_specification._selectedEntity);
        _components.showPanel(_specification._selectedEntity);
        processInput();
        ImGui::End();
    }

    void GUI::selectEntity(Entity& entity) {
        unselectEntity();
        _specification._selectedEntity = entity;
        if(_specification._selectedEntity.hasComponent<PhysicsComponent>())
            _specification._selectedEntity.getComponent<PhysicsComponent>().setShowCollider(true);
    }

    void GUI::unselectEntity() {
        if(_specification._selectedEntity.getSource() == entt::null)
            return;
        if(_specification._selectedEntity.hasComponent<PhysicsComponent>())
            _specification._selectedEntity.getComponent<PhysicsComponent>().setShowCollider(false);
        _specification._selectedEntity = {};
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
        if(_specification._scene->getIsFocusedOnDebugCamera() && !ImGuiFileDialog::Instance()->IsOpened()) {
            bool isUsing = showGizmo();
            if(mousePos.x >= 0.f && mousePos.y >= 0.f && mousePos.x < viewPortSize.x && mousePos.y < viewPortSize.y) {
                _specification.isMouseOnViewport = true;
                if(!isUsing && !_scene.isSceneEntityPopupOpen() && ImGui::IsMouseClicked(0)) {
                    int data = _specification._scene->_frameBuffer->readPixel(1, (int)mousePos.x, (int)mousePos.y);
                    if(data == -1 || !_specification._scene->_sceneRegistry.current->entityRegistry.valid((entt::entity)data))
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
                    unselectEntity();
                    _specification._scene->reset();
                    _specification._scene->_scriptDLLRegistry->clean();
                    std::string projectFilePath = projectDir + '/' + projectName + '/' + projectName + ".project";
                    auto projectData = ProjectCreator::load(projectFilePath, _specification._scene->_scriptDLLRegistry);
                    if(projectData) {
                        DLLCreator::initPaths(projectData->dllTempDir.string());
                        *_specification.projectData = *projectData;
                        _directory.setCurrentPath(projectData->rootPath);
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
                unselectEntity();
                _specification._scene->reset();
                _specification._scene->_scriptDLLRegistry->clean();
                auto projectData = ProjectCreator::load(projectFilePath, _specification._scene->_scriptDLLRegistry);
                if(projectData) {
                    std::filesystem::path dllTempDir = projectData->rootPath.string() + '/' + projectData->dllTempDir.string();
                    DLLCreator::initPaths(dllTempDir.string());
                    *_specification.projectData = *projectData;
                    _directory.setCurrentPath(projectData->rootPath);
                    if(!projectData->lastScenePath.empty() && std::filesystem::exists(projectData->rootPath / projectData->lastScenePath)) {
                        std::filesystem::path lastScenePath = projectData->rootPath.string() + '/' + projectData->lastScenePath.string();
                        SceneSerializer::deserialize(_specification._scene, lastScenePath.string());
                    }
                }
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("SaveSceneAs"))  {
            if(ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                SceneSerializer::serialize(_specification._scene, filePathName, _specification.projectData);
                _specification.projectData->lastScenePath = filePathName;
                ProjectCreator::save(_specification.projectData, _specification._scene->_scriptDLLRegistry);
            }
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
                    if(!modelEntities.empty()) {
                        selectEntity(modelEntities.back());
                        _scene.addEntityToSelected(_specification._selectedEntity);
                        _specification._scene->_sceneRegistry.current->urControl.execute(new CreateEntityCommand(_specification._selectedEntity, 
                            [&](){ unselectEntity(); }));
                    }
                }
            }
            ImGuiFileDialog::Instance()->Close();
            return;
        }
        if(ImGuiFileDialog::Instance()->Display("LoadCubemap"))  {
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
                        selectEntity(Shape::createCubemapEntity(_specification._scene, attachemnts));
                        _scene.addEntityToSelected(_specification._selectedEntity);
                        _specification._scene->_sceneRegistry.current->urControl.execute(new CreateEntityCommand(_specification._selectedEntity, 
                            [&](){ unselectEntity(); }));
                    }
                }
            }
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
        auto selectedEntityModel = selectedEntityTransform.getModel();

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), static_cast<ImGuizmo::OPERATION>(_specification._gizmoOperation),
                             ImGuizmo::LOCAL, glm::value_ptr(selectedEntityModel));

        bool isUsing = ImGuizmo::IsUsing();

        static bool addToURControl = false;
        static TransformComponent oldState;
        static TransformComponent newState;
        if(!addToURControl)
            oldState = selectedEntityTransform;

        if(isUsing) {
            glm::vec3 position, rotation, size;
            Math::decomposeTransform(selectedEntityModel, position, rotation, size);
            switch (_specification._gizmoOperation) {
            case GizmoOperation::Translate:
                addToURControl = true;
                selectedEntityTransform.setPosition(position);
                newState = selectedEntityTransform;
                break;
            case GizmoOperation::Rotate:
                addToURControl = true;
                selectedEntityTransform.setRotation(rotation);
                newState = selectedEntityTransform;
                break;
            case GizmoOperation::Scale:
                addToURControl = true;
                selectedEntityTransform.setSize(size);
                newState = selectedEntityTransform;
                break;
            }
        }
        if(Input::mouseButtonAction(Mouse::MOUSE_BUTTON_LEFT) == Action::RELEASE && addToURControl) {
            addToURControl = false;
            if(oldState.transform != newState.transform)
                _specification._scene->_sceneRegistry.current->urControl.execute(
                    new ChangeTransformComponentState(_specification._selectedEntity, oldState, newState));
        }

        return isUsing;
    }

    void GUI::processInput() {
        if(_specification.isFocusedOnViewport && !Input::isMouseButtonPressed(Mouse::MOUSE_BUTTON_RIGHT) 
        && _specification._selectedEntity.getSource() != entt::null) {
            if(Input::isKeyPressedOnce(Keyboard::KEY_Q))
                _specification._gizmoOperation = GizmoOperation::None;
            if(Input::isKeyPressedOnce(Keyboard::KEY_W))
                _specification._gizmoOperation = GizmoOperation::Translate;
            if(Input::isKeyPressedOnce(Keyboard::KEY_E))
                _specification._gizmoOperation = GizmoOperation::Rotate;
            if(Input::isKeyPressedOnce(Keyboard::KEY_R))
                _specification._gizmoOperation = GizmoOperation::Scale;
        }
        if(Input::isKeyPressedOnce(Keyboard::KEY_DELETE)) {
            auto removeEntity = _specification._selectedEntity;
            unselectEntity();
            _specification._scene->_sceneRegistry.current->urControl.execute(new RemoveEntityCommand(removeEntity, 
                [&](){
                    unselectEntity();
                }
            ));
        }
        if(Input::isKeyPressed(Keyboard::KEY_LEFT_CONTROL)) {
            if(Input::isKeyPressedOnce(Keyboard::KEY_Z))
                _specification._scene->_sceneRegistry.current->urControl.undo();
            if(Input::isKeyPressedOnce(Keyboard::KEY_Y))
                _specification._scene->_sceneRegistry.current->urControl.redo();
            if(Input::isKeyPressedOnce(Keyboard::KEY_D))
                _specification._scene->_sceneRegistry.current->urControl.execute(new CopyEntityCommand(_specification._selectedEntity, {}, [&](){ unselectEntity(); }));
            if(Input::isKeyPressedOnce(Keyboard::KEY_S)) {
                if(!_specification.projectData->lastScenePath.empty()) {
                    std::filesystem::path scenePath = _specification.projectData->rootPath.string() + '/' + _specification.projectData->lastScenePath.string();
                    SceneSerializer::serialize(_specification._scene, scenePath.string(), _specification.projectData);
                }
                else
                    ImGuiFileDialog::Instance()->OpenDialog("SaveSceneAs", "Choose File", ".scene", 
                        (_specification.projectData->rootPath.string() + '/').c_str(), 1, nullptr);
            }
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
        _start.setScene(scene);
        _scene.setScene(scene);
    }

    void GUI::setProjectData(ProjectData* projectData) {
        _specification.projectData = projectData;
        _directory.setProjectData(projectData);
    }

    bool GUI::getIsMouseOnViewport() { return _specification.isMouseOnViewport; }
    bool GUI::getIsFocusedOnViewport() { return _specification.isFocusedOnViewport; }
}