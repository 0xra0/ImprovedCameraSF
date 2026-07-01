#include "systems/Window.h"

namespace Systems {

    Window::Window() {}

    bool Window::Setup()
    {
        if (m_Initialized)
            return true;

        m_Handle = FindWindowA("SFSE", nullptr);
        if (!m_Handle) {
            m_Handle = GetForegroundWindow();
        }

        if (!m_Handle) {
            spdlog::error("Failed to find game window");
            return false;
        }

        m_Initialized = true;
        spdlog::info("Window initialized, handle: 0x{:X}", reinterpret_cast<uintptr_t>(m_Handle));
        return true;
    }

    bool Window::Shutdown()
    {
        m_Handle = nullptr;
        m_Initialized = false;
        return true;
    }
}
