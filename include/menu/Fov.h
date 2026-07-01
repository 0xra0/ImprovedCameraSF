#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class Fov : public IMenu {

    public:
        Fov() = default;
        ~Fov() override = default;

        std::string GetName() const override { return "FOV"; }
        void Draw() override;
    };
}
