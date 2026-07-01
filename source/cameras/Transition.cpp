#include "cameras/Transition.h"

namespace Camera {

    Transition::Transition(Events* events) :
        m_Events(events)
    {
        m_IsFirstPerson = false;
    }

    bool Transition::OnEnter()
    {
        m_Active = true;
        m_TransitionTimer = 0.0f;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnEnterTransition);
        }
        return true;
    }

    bool Transition::OnExit()
    {
        m_Active = false;
        if (m_Events) {
            m_Events->Trigger(EventType::kOnExitTransition);
        }
        return true;
    }

    bool Transition::OnUpdate()
    {
        if (!m_Active) {
            return false;
        }

        m_TransitionTimer += 0.016f;

        if (m_TransitionTimer > 1.0f) {
            OnExit();
            return false;
        }

        return true;
    }
}
