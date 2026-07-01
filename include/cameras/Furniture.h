#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"

namespace Camera {

    class Furniture : public ICamera {

    public:
        Furniture(Events* events);
        ~Furniture() override = default;

        CameraType GetType() const override { return CameraType::kFurniture; }
        std::string GetName() const override { return "Furniture"; }

        bool OnEnter() override;
        bool OnExit() override;
        bool OnUpdate() override;

    private:
        Events* m_Events;
    };
}
