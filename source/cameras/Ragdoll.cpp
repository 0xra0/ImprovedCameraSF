#include "cameras/Ragdoll.h"

namespace Camera {

    Ragdoll::Ragdoll(Events* events) :
        m_Events(events)
    {
        m_IsFirstPerson = false;
    }

    bool Ragdoll::OnEnter()
    {
        m_Active = true;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnEnterRagdoll);
        }
        spdlog::info("Ragdoll camera entered");
        return true;
    }

    bool Ragdoll::OnExit()
    {
        m_Active = false;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnExitRagdoll);
        }
        spdlog::info("Ragdoll camera exited");
        return true;
    }

    bool Ragdoll::OnUpdate()
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
