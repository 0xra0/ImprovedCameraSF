#include "menu/Headbob.h"
#include "plugin.h"
#include "menu/UIMenuHelper.h"

namespace Menu {

    void Headbob::Draw()
    {
        auto config = DLLMain::Plugin::Get()->GetConfig();
        if (!config) return;

        auto hb = config->Headbob();

        ImGui::Text("Headbob Settings");
        ImGui::Separator();

        ImGui::Checkbox("Enable Headbob", &hb->enableHeadbob);
        ImGui::Checkbox("Enable Translation", &hb->enableHeadbobTranslation);
        ImGui::Checkbox("Enable Rotation", &hb->enableHeadbobRotation);

        ImGui::Separator();
        ImGui::Checkbox("Sneaking", &hb->enableHeadbobSneaking);
        ImGui::Checkbox("Sprinting", &hb->enableHeadbobSprinting);
        ImGui::Checkbox("Swimming", &hb->enableHeadbobSwimming);
        ImGui::Checkbox("Walking", &hb->enableHeadbobWalking);
        ImGui::Checkbox("Running", &hb->enableHeadbobRunning);
        ImGui::Checkbox("Siding", &hb->enableHeadbobSiding);

        ImGui::Separator();
        ImGui::SliderFloat("Frequency", &hb->headbobFrequency, 0.1f, 5.0f, "%.1f");
        ImGui::SliderFloat("Intensity", &hb->headbobIntensity, 0.1f, 5.0f, "%.1f");
        ImGui::SliderFloat("Sneak Multiplier", &hb->headbobSneakMultiplier, 0.0f, 2.0f, "%.1f");
        ImGui::SliderFloat("Sprint Multiplier", &hb->headbobSprintMultiplier, 0.0f, 2.0f, "%.1f");
        ImGui::SliderFloat("Swim Multiplier", &hb->headbobSwimMultiplier, 0.0f, 2.0f, "%.1f");
    }
}
