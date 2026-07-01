#include "cameras/Horse.h"

namespace Camera {

    Horse::Horse(Events* events) :
        m_Events(events)
    {
        m_IsFirstPerson = false;
    }

    bool Horse::OnEnter()
    {
        m_Active = true;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnEnterHorse);
        }
        spdlog::info("Horse camera entered");
        return true;
    }

    bool Horse::OnExit()
    {
        m_Active = false;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnExitHorse);
        }
        spdlog::info("Horse camera exited");
        return true;
    }

    bool Horse::OnUpdate()
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
