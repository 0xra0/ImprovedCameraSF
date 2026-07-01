#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class Menus : public IMenu {

    public:
        Menus() = default;
        ~Menus() override = default;

        std::string GetName() const override { return "Menus"; }
        void Draw() override;
    };
}
