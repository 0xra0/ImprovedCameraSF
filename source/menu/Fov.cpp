#include "menu/Fov.h"
#include "plugin.h"
#include "menu/UIMenuHelper.h"

namespace Menu {

    void Fov::Draw()
    {
        auto config = DLLMain::Plugin::Get()->GetConfig();
        if (!config) return;

        auto fov = config->FOV();

        ImGui::Text("FOV Settings");
        ImGui::Separator();

        ImGui::Checkbox("Enable FOV Override", &fov->enableOverrideFOV);
        ImGui::Checkbox("Enable Event FOV", &fov->enableEventFOV);

        ImGui::Separator();
        ImGui::SliderFloat("First Person FOV", &fov->firstPersonFOV, 30.0f, 120.0f, "%.1f");
        ImGui::SliderFloat("Third Person FOV", &fov->thirdPersonFOV, 30.0f, 120.0f, "%.1f");
        ImGui::SliderFloat("Horse FOV", &fov->horseFOV, 30.0f, 120.0f, "%.1f");
        ImGui::SliderFloat("Ship FOV", &fov->shipFOV, 30.0f, 120.0f, "%.1f");
        ImGui::SliderFloat("Furniture FOV", &fov->furnitureFOV, 30.0f, 120.0f, "%.1f");
    }
}
