#include "gui/gui-directory-panel.hpp"

namespace TWE {
    GUIDirectoryPanel::GUIDirectoryPanel() {
        _projectData = new ProjectData();
        _curPath = _projectData->rootPath;
        loadTextures();
    }

    void GUIDirectoryPanel::loadTextures() {
        _dirImgPath = "../../include/gui/images/folder.png";
        _fileImgPath = "../../include/gui/images/file.png";
        TextureSpecification dirSpec(_dirImgPath, 0, TextureType::Texture2D, TextureInOutFormat::RGBA);
        TextureSpecification fileSpec(_fileImgPath, 0, TextureType::Texture2D, TextureInOutFormat::RGBA);
        _dirTexture = new Texture();
        _dirTexture->setTexture(dirSpec);
        _fileTexture = new Texture();
        _fileTexture->setTexture(fileSpec);
    }

    void GUIDirectoryPanel::setProjectData(ProjectData* projectData) {
        _projectData = projectData;
        _curPath = projectData->rootPath;
    }

    void GUIDirectoryPanel::setCurrentPath(const std::filesystem::path& curPath) {
        _curPath = curPath;
    }

    void GUIDirectoryPanel::showPanel(Entity& selectedEntity) {
        ImGui::Begin("Directory");

        bool isInteracted = renderContent(selectedEntity);

        std::string directoryMenuPopup = guiPopups[GUIPopupIds::DirectoryMenuPopup];
        if(!isInteracted && ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
            ImGui::SetWindowFocus();
            ImGui::OpenPopup(directoryMenuPopup.c_str());
        }
        showDirectoryMenuPopup(directoryMenuPopup);
        ImGui::End();
    }

    void GUIDirectoryPanel::setScene(Scene* scene) {
        _scene = scene;
    }

    bool GUIDirectoryPanel::renderContent(Entity& selectedEntity) {
        bool isInteracted = false;
        int cleanPos = _projectData->rootPath.string().find(_projectData->rootPath.filename().string());
        std::string curPath = _curPath.string().substr(cleanPos, _curPath.string().length());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 10.f });
        std::string curPathTitle = "Current path: " + curPath;
        ImGui::Text(curPathTitle.c_str());
        ImGui::PopStyleVar();
        if(_curPath != _projectData->rootPath) {
            ImGui::SameLine();
            float backBtnHeight = 15.f;
            if(ImGui::Button("Back"))
                _curPath = _curPath.parent_path();
        }
        ImGui::Separator();

        float contentCellSize = 75.f;
        float padding = 16.f;
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columns = max(static_cast<int>(panelWidth / contentCellSize), 1);

        ImGui::Columns(columns, 0, false);
        auto dirIt = std::filesystem::directory_iterator(_curPath);
        ImTextureID dirTextureId = (void*)(uint64_t)_dirTexture->getId(0);
        ImTextureID fileTextureId = (void*)(uint64_t)_fileTexture->getId(0);
        static std::filesystem::path fileMenuPath;
        std::string directoryFileMenuPopup = guiPopups[GUIPopupIds::DirectoryFileMenuPopup];
        int i = 0;
        std::vector<std::filesystem::directory_entry> entries;
        for(auto& entry : dirIt)
            entries.push_back(entry);  
        std::sort(entries.begin(), entries.end(), [](const std::filesystem::directory_entry& entry1, const std::filesystem::directory_entry& entry2) {
            return entry1.is_directory() && !entry2.is_directory();
        });
        for(auto& entry : entries) {
            auto& path = entry.path();
            ImGui::PushID(i++);

            std::string fileName = path.filename().string();
            ImGui::ImageButton(entry.is_directory() ? dirTextureId : fileTextureId, { contentCellSize - padding, contentCellSize - padding }, {0, 1}, {1, 0});
            if(ImGui::BeginDragDropSource()) {
                const wchar_t* item = path.c_str();
                ImGui::SetDragDropPayload(guiDragAndDropTypes[GUIDragAndDropType::DirectoryItem].c_str(), item, (wcslen(item) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }
            ImGui::PopID();
            if(ImGui::IsItemHovered()) {
                if(ImGui::IsMouseDoubleClicked(0)) {
                    ImGui::SetWindowFocus();
                    isInteracted = true;
                    std::string extension = path.extension().string();
                    if(entry.is_directory())
                        _curPath /= path.filename();
                    else if(extension == ".scene") {
                        selectedEntity = {};
                        _scene->reset();
                        SceneSerializer::deserialize(_scene, path.string());
                        _projectData->lastScenePath = std::filesystem::relative(path, _projectData->rootPath);
                        ProjectCreator::save(_projectData, _scene->_scriptDLLRegistry);
                    }
                } else if(ImGui::IsMouseClicked(1)) {
                    ImGui::SetWindowFocus();
                    isInteracted = true;
                    fileMenuPath = path;
                    ImGui::OpenPopup(directoryFileMenuPopup.c_str());
                }
            }
            ImGui::TextWrapped(fileName.c_str());

            ImGui::NextColumn();
        }
        showDirectoryFileMenuPopup(directoryFileMenuPopup, fileMenuPath, selectedEntity);
        return isInteracted;
    }

    void GUIDirectoryPanel::showDirectoryFileMenuPopup(const std::string& popupId, std::filesystem::path& filePath, Entity& selectedEntity) {
        float popUpWidth = 150.f;
        ImGui::SetNextWindowSize({popUpWidth, 0.f});
        if(ImGui::BeginPopup(popupId.c_str())) {
            auto& availSize = ImGui::GetContentRegionAvail();
            std::string fileExtension = filePath.extension().string();
            if(fileExtension == ".scene") {
                if(ImGui::Button("Load scene", {availSize.x, 0.f})) {
                    selectedEntity = {};
                    _scene->reset();
                    SceneSerializer::deserialize(_scene, filePath.string());
                    _projectData->lastScenePath = std::filesystem::relative(filePath, _projectData->rootPath);
                    ProjectCreator::save(_projectData, _scene->_scriptDLLRegistry);
                    ImGui::CloseCurrentPopup();
                }
            }
            bool validateScriptFlag = _scene->_sceneState != SceneState::Edit;
            if(fileExtension == ".hpp") {
                if(validateScriptFlag) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                }
                if(ImGui::Button("Validate", {availSize.x, 0.f})) {
                    std::string fileName = filePath.stem().string();
                    _scene->validateScript(fileName);
                    ImGui::CloseCurrentPopup();
                }
                if(validateScriptFlag) {
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();
                }
            }
            if(validateScriptFlag) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            if(ImGui::Button("Show in explorer", {availSize.x, 0.f})) {
                std::string cmdCommand = "explorer " + std::filesystem::absolute(filePath).parent_path().string();
                system(cmdCommand.c_str());
                ImGui::CloseCurrentPopup();
            }
            if(ImGui::Button("Remove", {availSize.x, 0.f})) {
                if(fileExtension == ".hpp") {
                    std::string fileName = filePath.stem().string();
                    if(_scene->_scriptDLLRegistry->has(fileName)) {
                        auto scriptDLLData = _scene->_scriptDLLRegistry->get(fileName);
                        if(scriptDLLData) {
                            _scene->unbindScript(&_scene->_sceneRegistry.edit.entityRegistry, scriptDLLData);
                            _scene->unbindScript(&_scene->_sceneRegistry.run.entityRegistry, scriptDLLData);
                            DLLCreator::freeDLLFunc(*scriptDLLData);
                            _scene->_scriptDLLRegistry->erase(fileName);
                            DLLCreator::removeScript(fileName);
                            ProjectCreator::save(_projectData, _scene->_scriptDLLRegistry);
                        }
                    }
                }
                if(std::filesystem::is_directory(filePath) && !std::filesystem::is_empty(filePath))
                    std::cout << "Not empty folder can't be deleted.\n";
                else
                    std::filesystem::remove(filePath);
                ImGui::CloseCurrentPopup();
            }
            if(validateScriptFlag) {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
            ImGui::EndPopup();
        }
    }

    void GUIDirectoryPanel::showDirectoryMenuPopup(const std::string& popupId) {
        float popUpWidth = 150.f;
        ImGui::SetNextWindowSize({popUpWidth, 0.f});
        if(ImGui::BeginPopup(popupId.c_str())) {
            auto& availSize = ImGui::GetContentRegionAvail();
            if(ImGui::BeginMenu("Create folder")) {
                static std::string createFolderName;
                if(GUIComponents::inputAndButton("Folder name", createFolderName, "Create")) {
                    std::filesystem::create_directory(_curPath / createFolderName);
                    createFolderName.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Create scene")) {
                static std::string sceneName;
                if(GUIComponents::inputAndButton("Scene name", sceneName, "Create")) {
                    Scene* newScene = new Scene(0.f, 0.f);
                    SceneSerializer::serialize(newScene, _curPath.string() + '/' + sceneName + ".scene", _projectData);
                    delete newScene;
                    sceneName.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Create script")) {
                static std::string scriptName;
                if(GUIComponents::inputAndButton("Script name", scriptName, "Create")) {
                    auto checkScriptUniqName = _scene->_scriptDLLRegistry->get(scriptName);
                    if(!checkScriptUniqName) {
                        std::string scriptDirectoryPath = _curPath.string();
                        if(ScriptCreator::create(scriptName, scriptDirectoryPath)) {
                            auto dllData = new DLLLoadData(DLLCreator::compileScript(scriptName, scriptDirectoryPath));
                            if(dllData->isValid)
                                _scene->_scriptDLLRegistry->add(scriptName, dllData);
                            ProjectCreator::save(_projectData, _scene->_scriptDLLRegistry);
                            scriptName.clear();
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
    }
}