#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class ThirdPerson : public ICamera {

    public:
        ThirdPerson(Events* events);
        ~ThirdPerson() override = default;

        CameraType GetType() const override { return CameraType::kThirdPerson; }
        std::string GetName() const override { return "ThirdPerson"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
    };
}
