#pragma once

#include <windows.h>

namespace Systems {

    class Window {

    public:
        Window();
        ~Window() = default;

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        bool Setup();
        bool Shutdown();

        HWND GetHandle() const { return m_Handle; }

    private:
        HWND m_Handle = nullptr;
        bool m_Initialized = false;
    };
}
