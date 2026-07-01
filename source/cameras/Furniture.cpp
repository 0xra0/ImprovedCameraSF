#include "cameras/Furniture.h"

namespace Camera {

    Furniture::Furniture(Events* events) :
        m_Events(events)
    {
        m_IsFirstPerson = false;
    }

    bool Furniture::OnEnter()
    {
        m_Active = true;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnEnterFurniture);
        }
        spdlog::info("Furniture camera entered");
        return true;
    }

    bool Furniture::OnExit()
    {
        m_Active = false;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnExitFurniture);
        }
        spdlog::info("Furniture camera exited");
        return true;
    }

    bool Furniture::OnUpdate()
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
