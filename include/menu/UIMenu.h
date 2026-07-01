#pragma once

#include "menu/IMenu.h"
#include "menu/General.h"
#include "menu/Hide.h"
#include "menu/RestrictAngles.h"
#include "menu/EventsMenu.h"
#include "menu/Fov.h"
#include "menu/NearDistance.h"
#include "menu/Headbob.h"
#include "menu/Camera.h"
#include "menu/Menus.h"

namespace Menu {

    class UIMenu : public IMenu {

    public:
        UIMenu();
        ~UIMenu() override = default;

        std::string GetName() const override { return "Improved Camera SF"; }
        void Draw() override;

    private:
        General m_General;
        HideMenu m_Hide;
        RestrictAngles m_RestrictAngles;
        EventsMenu m_Events;
        Fov m_Fov;
        NearDistance m_NearDistance;
        Headbob m_Headbob;
        Camera m_Camera;
        Menus m_Menus;

        int m_CurrentTab = 0;
    };
}
