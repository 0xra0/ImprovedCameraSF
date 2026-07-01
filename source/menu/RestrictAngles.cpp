#include "menu/RestrictAngles.h"
#include "plugin.h"
#include "menu/UIMenuHelper.h"

namespace Menu {

    void RestrictAngles::Draw()
    {
        auto config = DLLMain::Plugin::Get()->GetConfig();
        if (!config) return;

        auto ra = config->RestrictAngles();

        ImGui::Text("Restrict Angles Settings");
        ImGui::Separator();

        ImGui::Checkbox("Enable Restrict Angles", &ra->enableRestrictAngles);

        ImGui::Separator();
        ImGui::Text("Sitting");
        ImGui::Checkbox("Restrict Sitting", &ra->restrictSitting);
        ImGui::SliderFloat("Min Pitch", &ra->sittingMinPitch, -90.0f, 0.0f);
        ImGui::SliderFloat("Max Pitch", &ra->sittingMaxPitch, 0.0f, 90.0f);
        ImGui::SliderFloat("Min Yaw", &ra->sittingMinYaw, -360.0f, 0.0f);
        ImGui::SliderFloat("Max Yaw", &ra->sittingMaxYaw, 0.0f, 360.0f);

        ImGui::Separator();
        ImGui::Text("Mounted");
        ImGui::Checkbox("Restrict Mounted", &ra->restrictMounted);
        ImGui::SliderFloat("Min Pitch", &ra->mountedMinPitch, -90.0f, 0.0f);
        ImGui::SliderFloat("Max Pitch", &ra->mountedMaxPitch, 0.0f, 90.0f);
        ImGui::SliderFloat("Min Yaw", &ra->mountedMinYaw, -360.0f, 0.0f);
        ImGui::SliderFloat("Max Yaw", &ra->mountedMaxYaw, 0.0f, 360.0f);

        ImGui::Separator();
        ImGui::Text("Flying");
        ImGui::Checkbox("Restrict Flying", &ra->restrictFlying);
        ImGui::SliderFloat("Min Pitch", &ra->flyingMinPitch, -90.0f, 0.0f);
        ImGui::SliderFloat("Max Pitch", &ra->flyingMaxPitch, 0.0f, 90.0f);
    }
}
