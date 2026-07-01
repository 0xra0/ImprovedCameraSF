#pragma once

#include <imgui.h>
#include <string>
#include <vector>

namespace Menu::Helper {

    inline void Tooltip(const std::string& text)
    {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(text.c_str());
            ImGui::EndTooltip();
        }
    }

    inline bool ToggleButton(const std::string& label, bool* value)
    {
        if (*value) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));
        }

        bool clicked = ImGui::Button(label.c_str(), ImVec2(60, 0));

        ImGui::PopStyleColor(2);

        if (clicked) {
            *value = !*value;
        }

        return clicked;
    }

    inline void HelpMarker(const std::string& desc)
    {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}
