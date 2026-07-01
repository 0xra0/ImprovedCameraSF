#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class DeathCinematic : public ICamera {

    public:
        DeathCinematic(Events* events);
        ~DeathCinematic() override = default;

        CameraType GetType() const override { return CameraType::kDeathCinematic; }
        std::string GetName() const override { return "DeathCinematic"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
    };
}
