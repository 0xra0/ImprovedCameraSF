#include "cameras/Ship.h"

namespace Camera {

    Ship::Ship(Events* events) :
        m_Events(events)
    {
        m_IsFirstPerson = false;
    }

    bool Ship::OnEnter()
    {
        m_Active = true;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnEnterShip);
        }
        spdlog::info("Ship camera entered");
        return true;
    }

    bool Ship::OnExit()
    {
        m_Active = false;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnExitShip);
        }
        spdlog::info("Ship camera exited");
        return true;
    }

    bool Ship::OnUpdate()
    {
        if (!m_Active) {
            return false;
        }

        if (m_Events) {
            m_Events->Trigger(EventType::kOnCameraUpdate);
        }

        return true;
    }
}
