#include "menu/EventsMenu.h"
#include "plugin.h"
#include "menu/UIMenuHelper.h"

namespace Menu {

    void EventsMenu::Draw()
    {
        auto config = DLLMain::Plugin::Get()->GetConfig();
        if (!config) return;

        auto events = config->Events();

        ImGui::Text("Event Override Settings");
        ImGui::Separator();

        ImGui::Checkbox("Enable Event Overrides", &events->enableOverrideEvents);

        ImGui::Separator();

        for (auto& [eventName, state] : events->eventStates) {
            ImGui::Checkbox(eventName.c_str(), &state);
        }
    }
}
