#include "cameras/Events.h"

namespace Camera {

    Events::Events() {}

    void Events::Register(EventType type, EventCallback callback)
    {
        m_Callbacks[type].push_back(std::move(callback));
    }

    void Events::Unregister(EventType type, EventCallback callback)
    {
        auto it = m_Callbacks.find(type);
        if (it != m_Callbacks.end()) {
            auto& vec = it->second;
            for (auto vecIt = vec.begin(); vecIt != vec.end(); ++vecIt) {
                if (vecIt->target_type() == callback.target_type()) {
                    vec.erase(vecIt);
                    break;
                }
            }
        }
    }

    void Events::Trigger(EventType type) const
    {
        auto it = m_Callbacks.find(type);
        if (it != m_Callbacks.end()) {
            for (const auto& callback : it->second) {
                if (callback) {
                    callback();
                }
            }
        }
    }
}
