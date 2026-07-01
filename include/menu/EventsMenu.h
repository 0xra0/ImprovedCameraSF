#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class EventsMenu : public IMenu {

    public:
        EventsMenu() = default;
        ~EventsMenu() override = default;

        std::string GetName() const override { return "Events"; }
        void Draw() override;
    };
}
