#pragma once

#include "menu/IMenu.h"

namespace Menu {

    class General : public IMenu {

    public:
        General() = default;
        ~General() override = default;

        std::string GetName() const override { return "General"; }
        void Draw() override;
    };
}
