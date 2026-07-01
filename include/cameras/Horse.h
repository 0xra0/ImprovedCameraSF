#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class Horse : public ICamera {

    public:
        Horse(Events* events);
        ~Horse() override = default;

        CameraType GetType() const override { return CameraType::kHorse; }
        std::string GetName() const override { return "Horse"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
    };
}
