#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace Camera {

    enum class CameraType : uint32_t {
        kFirstPerson = 0,
        kThirdPerson,
        kTransition,
        kHorse,
        kShip,
        kFurniture,
        kRagdoll,
        kDeathCinematic,
        kTotal
    };

    enum class CameraEvent : uint32_t {
        kEnter = 0,
        kExit,
        kUpdate,
        kForceFirstPerson,
        kForceThirdPerson,
        kTotal
    };

    class ICamera {

    public:
        ICamera() = default;
        virtual ~ICamera() = default;

        ICamera(const ICamera&) = delete;
        ICamera& operator=(const ICamera&) = delete;
        ICamera(ICamera&&) = delete;
        ICamera& operator=(ICamera&&) = delete;

        virtual CameraType GetType() const = 0;
        virtual std::string GetName() const = 0;

        virtual bool OnEnter() = 0;
        virtual bool OnExit() = 0;
        virtual bool OnUpdate() = 0;

        virtual bool IsActive() const { return m_Active; }
        virtual void SetActive(bool active) { m_Active = active; }

        virtual bool IsFirstPerson() const { return m_IsFirstPerson; }
        virtual void SetFirstPerson(bool fp) { m_IsFirstPerson = fp; }

    protected:
        bool m_Active = false;
        bool m_IsFirstPerson = true;
        float m_TransitionTimer = 0.0f;
    };
}
