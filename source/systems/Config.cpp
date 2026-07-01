#include "systems/Config.h"
#include "plugin.h"
#include "version.h"

namespace Systems {

    Config::Config()
    {
        m_General = std::make_unique<Settings::General>();
        m_Hide = std::make_unique<Settings::Hide>();
        m_Fixes = std::make_unique<Settings::Fixes>();
        m_RestrictAngles = std::make_unique<Settings::RestrictAngles>();
        m_Events = std::make_unique<Settings::Events>();
        m_FOV = std::make_unique<Settings::FOV>();
        m_NearDistance = std::make_unique<Settings::NearDistance>();
        m_Headbob = std::make_unique<Settings::Headbob>();
        m_Camera = std::make_unique<Settings::Camera>();
        m_PseudoFP = std::make_unique<Settings::PseudoFP>();
    }

    bool Config::Load()
    {
        LoadDefaults();

        std::filesystem::path configPath = DLLMain::PLUGIN_PATH + std::string(VERSION_PRODUCTNAME_STR) + ".ini";
        mINI::INIFile file(configPath.string());
        mINI::INIStructure ini;

        if (!file.read(ini)) {
            Save();
            return true;
        }

        m_General->Load("General", ini);
        m_Hide->Load("Hide", ini);
        m_Fixes->Load("Fixes", ini);
        m_RestrictAngles->Load("RestrictAngles", ini);
        m_Events->Load("Events", ini);
        m_FOV->Load("FOV", ini);
        m_NearDistance->Load("NearDistance", ini);
        m_Headbob->Load("Headbob", ini);
        m_Camera->Load("Camera", ini);
        m_PseudoFP->Load("PseudoFP", ini);

        return true;
    }

    bool Config::Save()
    {
        std::filesystem::path configPath = DLLMain::PLUGIN_PATH + std::string(VERSION_PRODUCTNAME_STR) + ".ini";
        mINI::INIFile file(configPath.string());
        mINI::INIStructure ini;

        m_General->Save("General", ini);
        m_Hide->Save("Hide", ini);
        m_Fixes->Save("Fixes", ini);
        m_RestrictAngles->Save("RestrictAngles", ini);
        m_Events->Save("Events", ini);
        m_FOV->Save("FOV", ini);
        m_NearDistance->Save("NearDistance", ini);
        m_Headbob->Save("Headbob", ini);
        m_Camera->Save("Camera", ini);
        m_PseudoFP->Save("PseudoFP", ini);

        return file.write(ini);
    }

    bool Config::LoadDefaults()
    {
        *m_General = Settings::General{};
        *m_Hide = Settings::Hide{};
        *m_Fixes = Settings::Fixes{};
        *m_RestrictAngles = Settings::RestrictAngles{};
        *m_Events = Settings::Events{};
        *m_FOV = Settings::FOV{};
        *m_NearDistance = Settings::NearDistance{};
        *m_Headbob = Settings::Headbob{};
        *m_Camera = Settings::Camera{};
        *m_PseudoFP = Settings::PseudoFP{};
        return true;
    }

    bool Config::LoadProfile(const std::string& path)
    {
        mINI::INIFile file(path);
        mINI::INIStructure ini;

        if (!file.read(ini)) {
            return false;
        }

        m_General->Load("General", ini);
        m_Hide->Load("Hide", ini);
        m_Fixes->Load("Fixes", ini);
        m_RestrictAngles->Load("RestrictAngles", ini);
        m_Events->Load("Events", ini);
        m_FOV->Load("FOV", ini);
        m_NearDistance->Load("NearDistance", ini);
        m_Headbob->Load("Headbob", ini);
        m_Camera->Load("Camera", ini);
        m_PseudoFP->Load("PseudoFP", ini);

        return true;
    }

    bool Config::SaveProfile(const std::string& path)
    {
        mINI::INIFile file(path);
        mINI::INIStructure ini;

        m_General->Save("General", ini);
        m_Hide->Save("Hide", ini);
        m_Fixes->Save("Fixes", ini);
        m_RestrictAngles->Save("RestrictAngles", ini);
        m_Events->Save("Events", ini);
        m_FOV->Save("FOV", ini);
        m_NearDistance->Save("NearDistance", ini);
        m_Headbob->Save("Headbob", ini);
        m_Camera->Save("Camera", ini);
        m_PseudoFP->Save("PseudoFP", ini);

        return file.write(ini);
    }

    bool Config::DeleteProfile(const std::string& path)
    {
        return std::filesystem::remove(path);
    }

    std::vector<std::string> Config::GetProfiles() const
    {
        std::vector<std::string> profiles;

        std::filesystem::path profileDir = DLLMain::PLUGIN_PATH + std::string(VERSION_PRODUCTNAME_STR) + "\\Profiles\\";

        if (!std::filesystem::exists(profileDir)) {
            return profiles;
        }

        for (const auto& entry : std::filesystem::directory_iterator(profileDir)) {
            if (entry.path().extension() == ".ini") {
                profiles.push_back(entry.path().stem().string());
            }
        }

        return profiles;
    }
}
