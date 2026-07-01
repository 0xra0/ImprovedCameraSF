#pragma once

#include "settings/Settings.h"

namespace Systems {

    class Config {

    public:
        Config();
        ~Config() = default;

        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;

        bool Load();
        bool Save();

        Settings::General* General() const { return m_General.get(); }
        Settings::Hide* Hide() const { return m_Hide.get(); }
        Settings::Fixes* Fixes() const { return m_Fixes.get(); }
        Settings::RestrictAngles* RestrictAngles() const { return m_RestrictAngles.get(); }
        Settings::Events* Events() const { return m_Events.get(); }
        Settings::FOV* FOV() const { return m_FOV.get(); }
        Settings::NearDistance* NearDistance() const { return m_NearDistance.get(); }
        Settings::Headbob* Headbob() const { return m_Headbob.get(); }
        Settings::Camera* Camera() const { return m_Camera.get(); }
        Settings::PseudoFP* PseudoFP() const { return m_PseudoFP.get(); }

        bool LoadProfile(const std::string& path);
        bool SaveProfile(const std::string& path);
        bool DeleteProfile(const std::string& path);
        std::vector<std::string> GetProfiles() const;
        bool LoadDefaults();

        std::unique_ptr<Settings::General> m_General;
        std::unique_ptr<Settings::Hide> m_Hide;
        std::unique_ptr<Settings::Fixes> m_Fixes;
        std::unique_ptr<Settings::RestrictAngles> m_RestrictAngles;
        std::unique_ptr<Settings::Events> m_Events;
        std::unique_ptr<Settings::FOV> m_FOV;
        std::unique_ptr<Settings::NearDistance> m_NearDistance;
        std::unique_ptr<Settings::Headbob> m_Headbob;
        std::unique_ptr<Settings::Camera> m_Camera;
        std::unique_ptr<Settings::PseudoFP> m_PseudoFP;
    };
}
