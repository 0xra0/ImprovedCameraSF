#pragma once

#include "cameras/ICamera.h"
#include "cameras/Events.h"
#include "settings/Settings.h"

namespace Patch {

    class ImprovedCameraSF {

    public:
        ImprovedCameraSF();
        ~ImprovedCameraSF() = default;

        ImprovedCameraSF(const ImprovedCameraSF&) = delete;
        ImprovedCameraSF& operator=(const ImprovedCameraSF&) = delete;
        ImprovedCameraSF(ImprovedCameraSF&&) = delete;
        ImprovedCameraSF& operator=(ImprovedCameraSF&&) = delete;

        bool Load(Settings::General* general);

        Camera::ICamera* GetActiveCamera() const { return m_ActiveCamera.get(); }
        Camera::Events* GetEvents() const { return m_Events.get(); }

        void SetActiveCamera(std::unique_ptr<Camera::ICamera> camera);
        void UpdateCamera();

    private:
        std::unique_ptr<Camera::ICamera> m_ActiveCamera;
        std::unique_ptr<Camera::Events> m_Events;
        bool m_Loaded = false;
    };
}
