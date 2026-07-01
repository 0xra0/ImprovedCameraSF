#pragma once

#include <string>
#include <imgui.h>

namespace Menu {

    class IMenu {

    public:
        IMenu() = default;
        virtual ~IMenu() = default;

        IMenu(const IMenu&) = delete;
        IMenu& operator=(const IMenu&) = delete;
        IMenu(IMenu&&) = delete;
        IMenu& operator=(IMenu&&) = delete;

        virtual std::string GetName() const = 0;
        virtual void Draw() = 0;

        bool IsOpen() const { return m_Open; }
        void SetOpen(bool open) { m_Open = open; }
        void Toggle() { m_Open = !m_Open; }

    protected:
        bool m_Open = false;
    };
}
