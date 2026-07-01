#include "systems/UI.h"
#include "systems/Graphics.h"
#include "plugin.h"

namespace Systems {

    UI::UI() {}

    bool UI::Setup()
    {
        if (m_Initialized)
            return true;

        if (!SetupImGui())
            return false;

        m_Initialized = true;
        spdlog::info("UI initialized successfully");
        return true;
    }

    bool UI::Shutdown()
    {
        ShutdownImGui();
        m_Initialized = false;
        return true;
    }

    void UI::Render()
    {
        if (!m_Visible)
            return;

        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("Improved Camera SF", &m_Visible, ImGuiWindowFlags_NoCollapse);

        for (const auto& callback : m_RenderCallbacks) {
            if (callback) {
                callback();
            }
        }

        ImGui::End();

        ImGui::Render();
    }

    void UI::AddRenderCallback(RenderCallback callback)
    {
        m_RenderCallbacks.push_back(std::move(callback));
    }

    void UI::RemoveRenderCallback(RenderCallback callback)
    {
        auto& vec = m_RenderCallbacks;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if (it->target_type() == callback.target_type()) {
                vec.erase(it);
                break;
            }
        }
    }

    bool UI::SetupImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;

        ImGui::StyleColorsDark();

        return true;
    }

    bool UI::ShutdownImGui()
    {
        ImGui::DestroyContext();
        return true;
    }
}
