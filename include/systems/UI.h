#pragma once

#include <functional>
#include <imgui.h>

namespace Systems {

    class UI {

    public:
        using RenderCallback = std::function<void()>;

        UI();
        ~UI() = default;

        UI(const UI&) = delete;
        UI& operator=(const UI&) = delete;
        UI(UI&&) = delete;
        UI& operator=(UI&&) = delete;

        bool Setup();
        bool Shutdown();
        void Render();

        void AddRenderCallback(RenderCallback callback);
        void RemoveRenderCallback(RenderCallback callback);

        bool IsVisible() const { return m_Visible; }
        void SetVisible(bool visible) { m_Visible = visible; }
        void Toggle() { m_Visible = !m_Visible; }

    private:
        bool SetupImGui();
        bool ShutdownImGui();

        bool m_Visible = false;
        bool m_Initialized = false;
        std::vector<RenderCallback> m_RenderCallbacks;
    };
}
