#include "cameras/Cameras.h"
#include "plugin.h"

namespace Camera {

    Cameras::Cameras()
    {
        auto events = DLLMain::Plugin::Get()->GetStarfieldSF()->GetImprovedCamera()->GetEvents();

        m_Cameras[static_cast<size_t>(CameraType::kFirstPerson)] = std::make_unique<FirstPerson>(events);
        m_Cameras[static_cast<size_t>(CameraType::kThirdPerson)] = std::make_unique<ThirdPerson>(events);
        m_Cameras[static_cast<size_t>(CameraType::kTransition)] = std::make_unique<Transition>(events);
        m_Cameras[static_cast<size_t>(CameraType::kHorse)] = std::make_unique<Horse>(events);
        m_Cameras[static_cast<size_t>(CameraType::kShip)] = std::make_unique<Ship>(events);
        m_Cameras[static_cast<size_t>(CameraType::kFurniture)] = std::make_unique<Furniture>(events);
        m_Cameras[static_cast<size_t>(CameraType::kRagdoll)] = std::make_unique<Ragdoll>(events);
        m_Cameras[static_cast<size_t>(CameraType::kDeathCinematic)] = std::make_unique<DeathCinematic>(events);
    }

    bool Cameras::Load()
    {
        for (auto& camera : m_Cameras) {
            if (camera) {
                camera->OnEnter();
            }
        }

        m_ActiveCamera = m_Cameras[static_cast<size_t>(CameraType::kFirstPerson)].get();
        m_ActiveType = CameraType::kFirstPerson;

        spdlog::info("Cameras loaded successfully");
        return true;
    }

    ICamera* Cameras::GetCamera(CameraType type) const
    {
        return m_Cameras[static_cast<size_t>(type)].get();
    }

    void Cameras::SetActiveCamera(CameraType type)
    {
        if (m_ActiveCamera) {
            m_ActiveCamera->OnExit();
            m_ActiveCamera->SetActive(false);
        }

        m_ActiveCamera = m_Cameras[static_cast<size_t>(type)].get();
        m_ActiveType = type;

        if (m_ActiveCamera) {
            m_ActiveCamera->SetActive(true);
            m_ActiveCamera->OnEnter();
        }
    }

    void Cameras::Update()
    {
        if (m_ActiveCamera && m_ActiveCamera->IsActive()) {
            m_ActiveCamera->OnUpdate();
        }
    }
}
