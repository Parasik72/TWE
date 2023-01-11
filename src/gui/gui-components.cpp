#include "gui/gui-components.hpp"

namespace TWE {
    void GUIComponents::text(const std::string& label, const std::string& value, float labelColumnWidth) {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        ImGui::Text(value.c_str());
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
    }

    void GUIComponents::image(const std::string& label, ImTextureID imgId, const ImVec2& size, float labelColumnWidth) {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        ImGui::Image(imgId, size);
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
    }

    int GUIComponents::imageButton(const std::string& label, ImTextureID imgId, const ImVec2 &size, std::function<void()> dragAndDropFunc, float labelColumnWidth) {
        int result = -1;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        if(ImGui::ImageButton(imgId, size, {0.f, 0.f}, {1.f, 1.f}, 1))
            result = 0;
        if(ImGui::BeginDragDropTarget()) {
            dragAndDropFunc();
            ImGui::EndDragDropTarget();
        }
        if(imgId != 0) { 
            ImGui::SameLine();
            if(ImGui::Button("Remove", {0, size.y + 2.f}))
                result = 1;
        }
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return result;
    }

    bool GUIComponents::checkBox(const std::string& label, bool& value, float labelColumnWidth) {
        bool isInteracted = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());
        
        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        if(ImGui::Checkbox("##Checkbox", &value))
            isInteracted = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return isInteracted;
    }

    int GUIComponents::combo(const std::string& label, std::string& selected, std::vector<std::string>& options, float labelColumnWidth) {
        int res = -1;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        int size = options.size();
        if(ImGui::BeginCombo("##Combo", selected.c_str())) {
            for(int i = 0; i < size; ++i) {
                bool isSelected = options[i] == selected;
                if(ImGui::Selectable(options[i].c_str(), isSelected))
                    res = i;
                if(isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return res;
    }

    bool GUIComponents::colorEdit3(const std::string& label, glm::vec3& values, float labelColumnWidth) {
        bool isInteracted = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        if(ImGui::ColorEdit3("##ColorEdit3", glm::value_ptr(values)))
            isInteracted = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return isInteracted;
    }

    bool GUIComponents::dragFloat(const std::string& label, float& value, float step, float min, float max, float labelColumnWidth) {
        bool isInteracted = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        if(ImGui::DragFloat("##Value", &value, step, min, max, "%.3f"))
            isInteracted = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return isInteracted;
    }

    bool GUIComponents::dragFloat3(const std::string& label, glm::vec3& values, float step, float resetValue, float min, float max, float labelColumnWidth) {
        bool isInteracted = false;
        ImGuiButtonFlags btnFlags = ImGuiButtonFlags_PressedOnDoubleClick;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelColumnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 3.f });
        float height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
        ImVec2 size = { height + 3.f, height };

        if(ImGui::ButtonEx("X", size, btnFlags)) {
            isInteracted = true;
            values.x = resetValue;
        }
        ImGui::SameLine();
        if(ImGui::DragFloat("##X", &values.x, step, min, max, "%.2f"))
            isInteracted = true;
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if(ImGui::ButtonEx("Y", size, btnFlags)) {
            isInteracted = true;
            values.y = resetValue;
        }
        ImGui::SameLine();
        if(ImGui::DragFloat("##Y", &values.y, step, min, max, "%.2f"))
            isInteracted = true;
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if(ImGui::ButtonEx("Z", size, btnFlags)) {
            isInteracted = true;
            values.z = resetValue;
        }
        ImGui::SameLine();
        if(ImGui::DragFloat("##Z", &values.z, step, min, max, "%.2f"))
            isInteracted = true;
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
        return isInteracted;
    }

    bool GUIComponents::inputAndButton(const std::string& label, std::string& value, const std::string& buttonLabel, float labelColumnWidth) {
        bool isInteracted = false;
        ImGui::PushID(label.c_str());

        ImGui::Text(label.c_str());

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
        ImGui::InputText("##Text", &value);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if(ImGui::Button(buttonLabel.c_str()))
            isInteracted = true;    
        ImGui::PopItemWidth(); 

        ImGui::Columns(1);
        ImGui::PopID();
        return isInteracted;
    }
}