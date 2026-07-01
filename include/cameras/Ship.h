#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class Ship : public ICamera {

    public:
        Ship(Events* events);
        ~Ship() override = default;

        CameraType GetType() const override { return CameraType::kShip; }
        std::string GetName() const override { return "Ship"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
    };
}
