#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class Camera : public IMenu {

    public:
        Camera() = default;
        ~Camera() override = default;

        std::string GetName() const override { return "Camera"; }
        void Draw() override;
    };
}
