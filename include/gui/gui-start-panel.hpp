#ifndef START_PANEL_HPP
#define START_PANEL_HPP

#include "scene/scene.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <imgui-filedialog/ImGuiFileDialog.h>
#include <string>

namespace TWE {
    class GUIStartPanel {
    public:
        GUIStartPanel() = default;
        void initialize();
        void showPanel();
        void setScene(Scene* scene);
    private:
        Scene* _scene;
        ImFont* fontRobotoBold16;
        ImFont* fontRobotoBold32;
        ImFont* fontRobotoBold64;
        std::string title;
    };
}

#endif