#pragma once

#include "starfield/StarfieldSF.h"
#include "systems/Config.h"
#include "systems/Graphics.h"
#include "systems/Logging.h"

namespace DLLMain {

    constexpr auto PLUGIN_PATH = "Data\\SFSE\\Plugins\\";

    class Plugin {

    public:
        Plugin();
        ~Plugin();

        Plugin(const Plugin&) = delete;
        Plugin& operator=(const Plugin&) = delete;
        Plugin(Plugin&&) = delete;
        Plugin& operator=(Plugin&&) = delete;

        static Plugin* Get() { return s_Instance; };

    public:
        const std::string& Name() const { return m_Name; }
        const std::string& Description() const { return m_Description; }
        const std::string& Path() const { return m_Path; }
        std::uint32_t VersionMajor() const { return m_VersionMajor; }
        std::uint32_t VersionMinor() const { return m_VersionMinor; }
        std::uint32_t VersionRevision() const { return m_VersionRevision; }
        std::uint32_t VersionBuild() const { return m_VersionBuild; }
        bool IsGraphicsInitialized() const { return m_GraphicsInitialized; }

        Patch::StarfieldSF* GetStarfieldSF() const { return m_StarfieldSF.get(); }
        Systems::Config* GetConfig() const { return m_Config.get(); }
        Systems::Graphics* GetGraphics() const { return m_Graphics.get(); }

    public:
        bool Load();
        void CreateMenu();
        bool CheckStarfield();
        void CheckCompatibility();

    public:
        std::string m_Path{};

    private:
        std::string m_Name{};
        std::string m_Description{};
        std::uint32_t m_VersionMajor = 0;
        std::uint32_t m_VersionMinor = 0;
        std::uint32_t m_VersionRevision = 0;
        std::uint32_t m_VersionBuild = 0;
        bool m_GraphicsInitialized = false;

        Systems::Logging m_Logging;

        std::unique_ptr<Patch::StarfieldSF> m_StarfieldSF = nullptr;
        std::unique_ptr<Systems::Config> m_Config = nullptr;
        std::unique_ptr<Systems::Graphics> m_Graphics = nullptr;

        bool m_Loaded = false;
        bool m_InitializeMenu = false;

    private:
        static Plugin* s_Instance;
    };

}
