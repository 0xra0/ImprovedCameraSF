#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class Ragdoll : public ICamera {

    public:
        Ragdoll(Events* events);
        ~Ragdoll() override = default;

        CameraType GetType() const override { return CameraType::kRagdoll; }
        std::string GetName() const override { return "Ragdoll"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
    };
}
