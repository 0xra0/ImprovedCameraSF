#include "starfield/Addresses.h"
#include "RE/IDs.h"

namespace Patch {

    Addresses::Addresses() {}

    bool Addresses::Load()
    {
        if (m_Loaded)
            return true;

        m_Addresses["ForceFirstPerson"] = RE::ID::PlayerCamera::ForceFirstPerson.address();
        m_Addresses["ForceThirdPerson"] = RE::ID::PlayerCamera::ForceThirdPerson.address();
        m_Addresses["PlayerCameraSingleton"] = RE::ID::PlayerCamera::Singleton.address();
        // TESCamera::Update is resolved at runtime from vtable (REL::ID 430192, slot 03)
        // No static address available - handled in Hooks::Setup()

        m_Loaded = true;
        spdlog::info("Addresses loaded successfully");
        return true;
    }

    uintptr_t Addresses::GetAddress(const std::string& name) const
    {
        auto it = m_Addresses.find(name);
        if (it != m_Addresses.end()) {
            return it->second;
        }
        spdlog::warn("Address not found: {}", name);
        return 0;
    }

    void Addresses::SetAddress(const std::string& name, uintptr_t address)
    {
        m_Addresses[name] = address;
    }
}
