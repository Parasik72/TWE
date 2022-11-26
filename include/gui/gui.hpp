#ifndef GUI_HPP
#define GUI_HPP

#include "scene/scene.hpp"

#include <glfw3.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui-filedialog/ImGuiFileDialog.h>
#include <ImGuizmo.h>
#include <vector>
#include <string>
#include <functional>

namespace TWE {
    enum class GizmoOperation {
        None = -1,
        Translate = ImGuizmo::OPERATION::TRANSLATE,
        Rotate = ImGuizmo::OPERATION::ROTATE,
        Scale = ImGuizmo::OPERATION::SCALE,
    };

    struct GUISpecification {
        GUISpecification() = default;
        Scene* _scene;
        Entity _selectedEntity;
        GizmoOperation _gizmoOperation;
        int readPixelFBOData;
        bool isFileDialogOpen;
    };

    class GUI{
    public:
        GUI(GLFWwindow *window);
        ~GUI();
        void update();
        void addCheckbox(const char* name, bool& var);
        void addInputText(const char* name, std::string& var);
        void addButton(const char* name, std::function<void()> func);
        void setScene(Scene* scene);
    private:
        void showDockSpace();
        void showScenePanel();
        void showTestPanel();
        void showViewportPanel();
        void showDirectoryPanel();
        void showFileDialog();
        bool showGizmo();
        void processInput();
        void showComponentPanel();
        void showNameComponent();
        void showTransformComponent();
        void showMeshComponent();
        void showMeshRendererComponent();
        void showCameraComponent();
        void showLightComponent();
        void showPhysicsComponent();
        void showScriptComponent();
        bool dragFloat3(const std::string& label, glm::vec3& values, float step, float resetValue, float min = 0.f, float max = 0.f, float labelColumnWidth = 80.f);
        bool dragFloat(const std::string& label, float& value, float step, float min = 0.f, float max = 0.f, float labelColumnWidth = 80.f);
        bool colorEdit3(const std::string& label, glm::vec3& values, float labelColumnWidth = 80.f);
        bool checkBox(const std::string& label, bool& value, float labelColumnWidth = 80.f);
        void text(const std::string& label, const std::string& value, float labelColumnWidth = 80.f);
        int combo(const std::string& label, std::string& selected, std::vector<std::string>& options, float labelColumnWidth = 80.f);
        GUISpecification _specification;
        std::vector<std::pair<const char*, bool&>> _checkBoxes;
        std::vector<std::pair<const char*, std::string&>> _inputTextes;
        std::vector<std::pair<const char*, std::function<void()>>> _buttons;
    };
}

#endif