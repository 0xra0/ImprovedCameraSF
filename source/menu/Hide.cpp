#include "menu/Hide.h"
#include "plugin.h"
#include "menu/UIMenuHelper.h"

namespace Menu {

    void HideMenu::Draw()
    {
        auto config = DLLMain::Plugin::Get()->GetConfig();
        if (!config) return;

        auto hide = config->Hide();

        ImGui::Text("Hide Settings");
        ImGui::Separator();

        ImGui::Checkbox("Hide Body (First Person)", &hide->hideBodyFirstPerson);
        Helper::Tooltip("Hides the player's body in first person");

        ImGui::Checkbox("Hide Head (First Person)", &hide->hideHeadFirstPerson);
        Helper::Tooltip("Hides the player's head in first person");

        ImGui::Checkbox("Hide Equipment (First Person)", &hide->hideEquipmentFirstPerson);
        Helper::Tooltip("Hides equipment in first person");

        ImGui::Checkbox("Show Body In Menu", &hide->showBodyInMenu);
        Helper::Tooltip("Shows the body in menus (inventory, etc.)");

        ImGui::Separator();

        ImGui::Checkbox("Hide Weapon (First Person)", &hide->hideWeaponFirstPerson);
        ImGui::Checkbox("Hide Weapon (Third Person)", &hide->hideWeaponThirdPerson);
        ImGui::Checkbox("Hide Movement (First Person)", &hide->hideMovementFirstPerson);
    }
}
