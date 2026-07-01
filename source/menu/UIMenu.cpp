#include "menu/UIMenu.h"
#include "plugin.h"

namespace Menu {

    UIMenu::UIMenu() {}

    void UIMenu::Draw()
    {
        if (!m_Open)
            return;

        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin(GetName().c_str(), &m_Open, ImGuiWindowFlags_NoCollapse);

        const char* tabs[] = {
            "General", "Hide", "Fixes", "Restrict Angles",
            "Events", "FOV", "Near Distance", "Headbob", "Camera"
        };

        if (ImGui::BeginTabBar("MainTabs")) {
            if (ImGui::BeginTabItem("General")) {
                m_General.Draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Hide")) {
                m_Hide.Draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Fixes")) {
                ImGui::Text("Fixes settings");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Restrict Angles")) {
                m_RestrictAngles.Draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Events")) {
                m_Events.Draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("FOV")) {
                m_Fov.Draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Near Distance")) {
                m_NearDistance.Draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Headbob")) {
                m_Headbob.Draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Camera")) {
                m_Camera.Draw();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}
