#include "systems/Logging.h"

namespace Systems {

    Logging::Logging()
    {
        auto sinks = std::initializer_list<spdlog::sink_ptr>{
            std::make_shared<spdlog::sinks::msvc_sink_mt>()
        };
        m_Logger = std::make_shared<spdlog::logger>("ImprovedCameraSF", sinks);
        m_Logger->set_level(spdlog::level::trace);
        m_Logger->flush_on(spdlog::level::trace);
        spdlog::set_default_logger(m_Logger);
    }

    void Logging::OpenRelative(const std::filesystem::path& path)
    {
        try {
            auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true);
            m_Logger->sinks().push_back(fileSink);
        } catch (const spdlog::spdlog_ex& e) {
            spdlog::error("Failed to open log file: {}", e.what());
        }
    }
}
