#include "plugin.h"
#include "version.h"
#include <fstream>
#include <ctime>
#include <cstdarg>
#include <cstdlib>

namespace DLLMain {

    namespace {
        static void VLog(const char* fmt, va_list args)
        {
            char* userProfile = nullptr;
            size_t len = 0;
            _dupenv_s(&userProfile, &len, "USERPROFILE");
            std::string path = std::string(userProfile ? userProfile : "") + "\\Documents\\My Games\\Starfield\\SFSE\\ImprovedCameraSF_debug.log";
            free(userProfile);
            std::ofstream log(path, std::ios::app);
            if (log) {
                time_t t = time(nullptr);
                struct tm tm;
                localtime_s(&tm, &t);
                char buf[64];
                strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
                log << "[Plugin " << buf << "] ";
                char msg[768];
                vsprintf_s(msg, fmt, args);
                log << msg << std::endl;
            }
        }

        static void LogFormatted(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            VLog(fmt, args);
            va_end(args);
        }
    }

    Plugin* Plugin::s_Instance = nullptr;

    Plugin::Plugin() :
        m_Name(VERSION_PRODUCTNAME_STR),
        m_Description(VERSION_PRODUCTNAME_DESCRIPTION_STR),
        m_VersionMajor(VERSION_MAJOR),
        m_VersionMinor(VERSION_MINOR),
        m_VersionRevision(VERSION_REVISION),
        m_VersionBuild(VERSION_BUILD)
    {
        if (s_Instance) {
            return;
        }
        s_Instance = this;

        char path[_MAX_PATH]{};
        GetCurrentDirectoryA(_MAX_PATH, path);
        m_Path = std::filesystem::path(path).append("").string();
    }

    Plugin::~Plugin()
    {
        m_Graphics.reset();
        m_Config.reset();
        m_StarfieldSF.reset();
        s_Instance = nullptr;
    }

    bool Plugin::Load()
    {
        LogFormatted("Plugin::Load begin loaded=%d cwd=%s", m_Loaded ? 1 : 0, m_Path.c_str());
        if (!m_Loaded) {
            m_Config = std::make_unique<Systems::Config>();
            m_StarfieldSF = std::make_unique<Patch::StarfieldSF>();
            LogFormatted("allocated config=%p starfield=%p", static_cast<void*>(m_Config.get()), static_cast<void*>(m_StarfieldSF.get()));

            if (m_Config) {
                if (!m_Config->Load()) {
                    LogFormatted("Config::Load failed");
                    return false;
                }
                auto pseudo = m_Config->PseudoFP();
                LogFormatted("Config::Load ok pseudo=%p enabled=%d",
                    static_cast<void*>(pseudo),
                    (pseudo && pseudo->enablePseudoFP) ? 1 : 0);
            }

            if (m_StarfieldSF) {
                if (!m_StarfieldSF->Load(m_Config.get())) {
                    LogFormatted("StarfieldSF::Load failed");
                    return false;
                }
                LogFormatted("StarfieldSF::Load ok");
            }

            m_Loaded = true;
        }
        LogFormatted("Plugin::Load success");
        return true;
    }

    void Plugin::CreateMenu()
    {
    }

    bool Plugin::CheckStarfield()
    {
        spdlog::info("{:*^60}", " CHECKING STARFIELD ");
        return true;
    }

    void Plugin::CheckCompatibility()
    {
    }
}
