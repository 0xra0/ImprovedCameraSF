#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class FirstPerson : public ICamera {

    public:
        FirstPerson(Events* events);
        ~FirstPerson() override = default;

        CameraType GetType() const override { return CameraType::kFirstPerson; }
        std::string GetName() const override { return "FirstPerson"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
        bool m_IsWeaponDrawn = false;
        float m_HeadbobTimer = 0.0f;
    };
}
