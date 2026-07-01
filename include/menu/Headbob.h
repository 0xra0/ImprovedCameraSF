#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class Headbob : public IMenu {

    public:
        Headbob() = default;
        ~Headbob() override = default;

        std::string GetName() const override { return "Headbob"; }
        void Draw() override;
    };
}
