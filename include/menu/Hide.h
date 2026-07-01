#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class HideMenu : public IMenu {

    public:
        HideMenu() = default;
        ~HideMenu() override = default;

        std::string GetName() const override { return "Hide"; }
        void Draw() override;
    };
}
