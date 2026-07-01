#pragma once

#include <cstdint>

namespace RE {
    class NiAVObject;
}

namespace Patch {

    RE::NiAVObject* GetCachedHeadBone();

    class EventsStarfield {

    public:
        EventsStarfield();
        ~EventsStarfield() = default;

        EventsStarfield(const EventsStarfield&) = delete;
        EventsStarfield& operator=(const EventsStarfield&) = delete;
        EventsStarfield(EventsStarfield&&) = delete;
        EventsStarfield& operator=(EventsStarfield&&) = delete;

        bool Setup();
        bool Remove();

    private:
        bool m_Setup = false;
    };

}
