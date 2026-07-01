#include "menu/General.h"
#include "plugin.h"
#include "menu/UIMenuHelper.h"

namespace Menu {

    void General::Draw()
    {
        auto config = DLLMain::Plugin::Get()->GetConfig();
        if (!config) return;

        auto general = config->General();

        ImGui::Text("General Settings");
        ImGui::Separator();

        ImGui::Checkbox("Enable Mod", &general->enableMod);
        Helper::Tooltip("Master toggle for the entire mod");

        ImGui::Checkbox("Enable First Person", &general->enableFirstPerson);
        ImGui::Checkbox("Enable Third Person", &general->enableThirdPerson);
        ImGui::Checkbox("Enable Transition", &general->enableTransition);
        ImGui::Checkbox("First Person With Body", &general->firstPersonWithBody);
        ImGui::Checkbox("Enable Horse", &general->enableHorse);
        ImGui::Checkbox("Enable Ship", &general->enableShip);
        ImGui::Checkbox("Enable Furniture", &general->enableFurniture);

        ImGui::Separator();

        if (ImGui::Button("Save Config")) {
            config->Save();
        }

        ImGui::SameLine();

        if (ImGui::Button("Load Defaults")) {
            config->LoadDefaults();
        }
    }
}
