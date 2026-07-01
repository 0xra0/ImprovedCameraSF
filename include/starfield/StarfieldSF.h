#pragma once

#include "starfield/Addresses.h"
#include "starfield/EventsStarfield.h"
#include "starfield/Hooks.h"
#include "starfield/ImprovedCameraSF.h"
#include "systems/Config.h"

namespace Patch {

    class StarfieldSF {

    public:
        StarfieldSF();
        ~StarfieldSF() = default;

        StarfieldSF(const StarfieldSF&) = delete;
        StarfieldSF& operator=(const StarfieldSF&) = delete;
        StarfieldSF(StarfieldSF&&) = delete;
        StarfieldSF& operator=(StarfieldSF&&) = delete;

        bool Load(Systems::Config* config);

        Addresses* GetAddresses() const { return m_Addresses.get(); }
        Hooks* GetHooks() const { return m_Hooks.get(); }
        ImprovedCameraSF* GetImprovedCamera() const { return m_ImprovedCamera.get(); }
        EventsStarfield* GetEvents() const { return m_Events.get(); }

    private:
        std::unique_ptr<Addresses> m_Addresses;
        std::unique_ptr<Hooks> m_Hooks;
        std::unique_ptr<ImprovedCameraSF> m_ImprovedCamera;
        std::unique_ptr<EventsStarfield> m_Events;
        bool m_Loaded = false;
    };
}
