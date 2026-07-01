#include "menu/NearDistance.h"
#include "plugin.h"
#include "menu/UIMenuHelper.h"

namespace Menu {

    void NearDistance::Draw()
    {
        auto config = DLLMain::Plugin::Get()->GetConfig();
        if (!config) return;

        auto nd = config->NearDistance();

        ImGui::Text("Near Distance Settings");
        ImGui::Separator();

        ImGui::Checkbox("Enable Near Distance", &nd->enableNearDistance);
        ImGui::SliderFloat("Near Distance", &nd->nearDistance, 1.0f, 100.0f, "%.1f");
        Helper::Tooltip("Controls the near clipping plane distance");

        ImGui::SliderFloat("Pitch Threshold", &nd->nearDistancePitchThreshold, 0.0f, 1.0f, "%.2f");
        Helper::Tooltip("Pitch angle threshold for near distance adjustment");

        ImGui::Checkbox("Enable Near Distance Event", &nd->enableNearDistanceEvent);
    }
}
