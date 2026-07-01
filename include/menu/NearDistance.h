#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class NearDistance : public IMenu {

    public:
        NearDistance() = default;
        ~NearDistance() override = default;

        std::string GetName() const override { return "Near Distance"; }
        void Draw() override;
    };
}
