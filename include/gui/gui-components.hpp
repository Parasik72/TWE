#ifndef GUI_COMPONENTS_HPP
#define GUI_COMPONENTS_HPP

#include <string>
#include <vector>
#include <functional>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TWE {
    class GUIComponents {
    public:
        static bool dragFloat3(const std::string& label, glm::vec3& values, float step, float resetValue, float min = 0.f, float max = 0.f, float labelColumnWidth = 80.f);
        static bool dragFloat(const std::string& label, float& value, float step, float min = 0.f, float max = 0.f, float labelColumnWidth = 80.f);
        static bool colorEdit3(const std::string& label, glm::vec3& values, float labelColumnWidth = 80.f);
        static bool checkBox(const std::string& label, bool& value, float labelColumnWidth = 80.f);
        static bool inputAndButton(const std::string& label, std::string& value, const std::string& buttonLabel, float labelColumnWidth = 80.f);
        static int imageButton(const std::string& label, ImTextureID imgId, const ImVec2 &size, std::function<void()> dragAndDropFunc = [](){}, float labelColumnWidth = 80.f);
        static void text(const std::string& label, const std::string& value, float labelColumnWidth = 80.f);
        static void image(const std::string& label, ImTextureID imgId, const ImVec2 &size, float labelColumnWidth = 80.f);
        static int combo(const std::string& label, std::string& selected, std::vector<std::string>& options, float labelColumnWidth = 80.f);
    };
}

#endif