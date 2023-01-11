#ifndef GUI_DIRECTORY_PANEL_HPP
#define GUI_DIRECTORY_PANEL_HPP

#include "scene/scene.hpp"
#include "scene/scene-serializer.hpp"
#include "registry/registry.hpp"
#include "stream/script-creator.hpp"
#include "stream/project-creator.hpp"
#include "gui/gui-components.hpp"
#include "gui/gui-types.hpp"
#include "renderer/texture.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <algorithm>

namespace TWE {
    class GUIDirectoryPanel {
    public:
        GUIDirectoryPanel();
        void showPanel(Entity& selectedEntity);
        void setScene(Scene* scene);
        void setProjectData(ProjectData* projectData);
        void setCurrentPath(const std::filesystem::path& curPath);
    private:
        void loadScene(std::filesystem::path& scenePath);
        void loadTextures();
        void showDirectoryMenuPopup(const std::string& popupId);
        void showDirectoryFileMenuPopup(const std::string& popupId, std::filesystem::path& filePath, Entity& selectedEntity);
        bool renderContent(Entity& selectedEntity);
        std::filesystem::path _curPath;
        Scene* _scene;
        ProjectData* _projectData;
        Texture* _dirTexture;
        Texture* _fileTexture;
        std::string _dirImgPath;
        std::string _fileImgPath;
    };
}

#endif