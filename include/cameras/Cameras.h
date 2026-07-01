#pragma once

#include "cameras/FirstPerson.h"
#include "cameras/ThirdPerson.h"
#include "cameras/Transition.h"
#include "cameras/Horse.h"
#include "cameras/Ship.h"
#include "cameras/Furniture.h"
#include "cameras/Ragdoll.h"
#include "cameras/DeathCinematic.h"

namespace Camera {

    class Cameras {

    public:
        Cameras();
        ~Cameras() = default;

        Cameras(const Cameras&) = delete;
        Cameras& operator=(const Cameras&) = delete;
        Cameras(Cameras&&) = delete;
        Cameras& operator=(Cameras&&) = delete;

        bool Load();

        ICamera* GetCamera(CameraType type) const;
        ICamera* GetActiveCamera() const { return m_ActiveCamera; }

        void SetActiveCamera(CameraType type);
        void Update();

    private:
        std::array<std::unique_ptr<ICamera>, static_cast<size_t>(CameraType::kTotal)> m_Cameras;
        ICamera* m_ActiveCamera = nullptr;
        CameraType m_ActiveType = CameraType::kFirstPerson;
    };
}
