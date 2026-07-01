#include "plugin.h"
#include "version.h"
#include <fstream>
#include <ctime>
#include <cstdarg>
#include <cstdlib>

namespace
{
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
            log << "[dllmain " << buf << "] ";
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

SFSE_PLUGIN_VERSION{
	SFSE::PluginVersionData::kVersion,
	REL::Version(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD).pack(),
	VERSION_PRODUCTNAME_STR,
	VERSION_AUTHOR_STR,
	1, // addressIndependence (UsesSigScanning)
	1, // structureCompatibility (HasNoStructUse)
	{ SFSE::RUNTIME_LATEST.pack() },
	0, // xseMinimum
	0, // reservedNonBreaking
	0  // reservedBreaking
};

void OnSFSEMessage(SFSE::MessagingInterface::Message* message)
{
    LogFormatted("OnSFSEMessage type=%u", message ? message->type : 0);
    switch (message->type) {
    case SFSE::MessagingInterface::kPostPostLoad:
        {
            LogFormatted("OnSFSEMessage kPostPostLoad -> Plugin::Load()");
            DLLMain::Plugin::Get()->Load();
            break;
        }
    default:
        break;
    }
}

SFSE_PLUGIN_LOAD(const SFSE::LoadInterface* sfse)
{
    LogFormatted("SFSE_PLUGIN_LOAD begin");
    SFSE::Init(sfse, { .trampoline = true, .trampolineSize = 1 << 10 });
    LogFormatted("SFSE::Init ok");

    static DLLMain::Plugin plugin;
    if (!plugin.Load()) {
        LogFormatted("initial Plugin::Load failed");
        return false;
    }
    LogFormatted("initial Plugin::Load ok");

    SFSE::GetMessagingInterface()->RegisterListener(OnSFSEMessage);
    LogFormatted("RegisterListener ok");

    return true;
}
