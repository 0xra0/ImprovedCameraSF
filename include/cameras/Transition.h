#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class Transition : public ICamera {

    public:
        Transition(Events* events);
        ~Transition() override = default;

        CameraType GetType() const override { return CameraType::kTransition; }
        std::string GetName() const override { return "Transition"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
    };
}
