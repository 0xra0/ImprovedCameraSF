#include "menu/Menus.h"
#include "plugin.h"

namespace Menu {

    void Menus::Draw()
    {
        ImGui::Text("Menu Toggles");
        ImGui::Separator();

        if (ImGui::Button("Open Full Editor")) {
        }
    }
}
