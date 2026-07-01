#include "cameras/DeathCinematic.h"

namespace Camera {

    DeathCinematic::DeathCinematic(Events* events) :
        m_Events(events)
    {
        m_IsFirstPerson = false;
    }

    bool DeathCinematic::OnEnter()
    {
        m_Active = true;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnEnterDeathCinematic);
        }
        spdlog::info("DeathCinematic camera entered");
        return true;
    }

    bool DeathCinematic::OnExit()
    {
        m_Active = false;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnExitDeathCinematic);
        }
        spdlog::info("DeathCinematic camera exited");
        return true;
    }

    bool DeathCinematic::OnUpdate()
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
