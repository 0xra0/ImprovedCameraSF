#include "starfield/StarfieldSF.h"
#include "systems/Config.h"
#include "plugin.h"
#include "starfield/Hooks.h"
#include <fstream>
#include <ctime>
#include <cstdarg>
#include <cstdlib>

namespace Patch {

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
                log << "[StarfieldSF " << buf << "] ";
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

    StarfieldSF::StarfieldSF() {}

    bool StarfieldSF::Load(Systems::Config* config)
    {
        if (m_Loaded)
            return true;

        LogFormatted("StarfieldSF::Load begin config=%p", static_cast<void*>(config));

        // Step 1: Addresses
        m_Addresses = std::make_unique<Addresses>();
        if (!m_Addresses->Load()) {
            LogFormatted("Addresses::Load failed");
            return false;
        }
        LogFormatted("Addresses::Load ok");

        // Step 2: Hooks (sets up TESCamera::Update hook from vtable)
        m_Hooks = std::make_unique<Hooks>();
        if (!m_Hooks->Setup()) {
            LogFormatted("Hooks::Setup failed");
            return false;
        }
        LogFormatted("Hooks::Setup ok");

        // Step 3: ImprovedCameraSF
        m_ImprovedCamera = std::make_unique<ImprovedCameraSF>();
        if (!m_ImprovedCamera->Load(config->General())) {
            LogFormatted("ImprovedCameraSF::Load failed");
            return false;
        }
        LogFormatted("ImprovedCameraSF::Load ok");

        // Step 5: EventsStarfield (permanent task - overwrites currentState + zoom)
        m_Events = std::make_unique<EventsStarfield>();
        if (!m_Events->Setup()) {
            LogFormatted("EventsStarfield::Setup failed");
            return false;
        }
        LogFormatted("EventsStarfield::Setup ok");

        m_Loaded = true;
        LogFormatted("StarfieldSF::Load success");
        return true;
    }
}
