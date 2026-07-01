#include "starfield/ImprovedCameraSF.h"
#include "starfield/Helper.h"

namespace Patch {

    ImprovedCameraSF::ImprovedCameraSF() {}

    bool ImprovedCameraSF::Load(Settings::General* general)
    {
        if (m_Loaded)
            return true;

        if (!general || !general->enableMod) {
            spdlog::info("Improved Camera SF is disabled in settings");
            return false;
        }

        m_Events = std::make_unique<Camera::Events>();

        m_Loaded = true;
        spdlog::info("Improved Camera SF loaded successfully");
        return true;
    }

    void ImprovedCameraSF::SetActiveCamera(std::unique_ptr<Camera::ICamera> camera)
    {
        m_ActiveCamera = std::move(camera);
    }

    void ImprovedCameraSF::UpdateCamera()
    {
        if (!m_ActiveCamera) {
            return;
        }

        auto playerCamera = RE::PlayerCamera::GetSingleton();
        if (!playerCamera) {
            return;
        }

        m_ActiveCamera->OnUpdate();
    }
}
