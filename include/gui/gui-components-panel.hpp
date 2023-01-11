#ifndef GUI_COMPONENTS_PANEL_HPP
#define GUI_COMPONENTS_PANEL_HPP

#include "scene/scene.hpp"
#include "scene/shape.hpp"

#include "gui/gui-components.hpp"
#include "gui/gui-types.hpp"

#include "registry/registry.hpp"
#include "undo-redo/ur-commands/ur-commands.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <imgui-filedialog/ImGuiFileDialog.h>

namespace TWE {
    class GUIComponentsPanel {
    public:
        GUIComponentsPanel();
        void showPanel(Entity& entity);
        void setScene(Scene* scene);
    private:
        void showFileDialog(Entity& entity);
        void showNameComponent(Entity& entity);
        void showTransformComponent(Entity& entity);
        void showMeshComponent(Entity& entity);
        void showMeshRendererComponent(Entity& entity);
        void showCameraComponent(Entity& entity);
        void showLightComponent(Entity& entity);
        void showPhysicsComponent(Entity& entity);
        void showScriptComponent(Entity& entity);
        void showAddComponentMenu(Entity& entity);
        void showSeparator();
        Scene* _scene;
    };
}

#endif