#ifndef GUI_TYPES_HPP
#define GUI_TYPES_HPP

#include <vector>
#include <string>

namespace TWE {
    enum GUIDragAndDropType {
        DirectoryItem,
        EntityItem
    };

    extern std::vector<std::string> guiDragAndDropTypes;

    enum GUIPopupIds {
        EntityPopup,
        MenuPopup,
        DirectoryFileMenuPopup,
        DirectoryMenuPopup,
        AddComponentPopup,
        MeshPopup,
        MeshRendererPopup,
        CameraPopup,
        LightPopup,
        PhysicsPopup,
        ScriptPopup
    };

    extern std::vector<std::string> guiPopups;
}

#endif