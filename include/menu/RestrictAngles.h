#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class RestrictAngles : public IMenu {

    public:
        RestrictAngles() = default;
        ~RestrictAngles() override = default;

        std::string GetName() const override { return "Restrict Angles"; }
        void Draw() override;
    };
}
