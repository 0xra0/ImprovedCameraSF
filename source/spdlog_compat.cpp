#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/details/os.h>

#include <string>
#include <vector>

namespace spdlog {
namespace details {
namespace os {

// Pre-built CommonLibSF was compiled against an older spdlog ABI
// where utf8_to_wstrbuf took (string_view, wstring&) instead of (wstring_view, wmemory_buf_t&)
void utf8_to_wstrbuf(std::string_view str, std::wstring& target)
{
    if (str.empty()) {
        target.clear();
        return;
    }
    int len = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
    if (len > 0) {
        target.resize(static_cast<std::size_t>(len));
        MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), target.data(), len);
    } else {
        target.clear();
    }
}

void wstr_to_utf8buf(std::wstring_view wstr, std::string& target)
{
    if (wstr.empty()) {
        target.clear();
        return;
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (len > 0) {
        target.resize(static_cast<std::size_t>(len));
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), target.data(), len, nullptr, nullptr);
    } else {
        target.clear();
    }
}

} // namespace os
} // namespace details
} // namespace spdlog

// Provide REX::Impl::Log and REX::Impl::Fail stubs to replace LOG.cpp.obj
// from the pre-built commonlib-shared.lib (which has incompatible spdlog ABI).
// The pre-built lib's LOG.cpp.obj was compiled against an older spdlog where
// string_view_t = std::string_view and payload was memory_buf_t.
#include <REX/LOG.h>

namespace REX::Impl
{
    void Log(const std::source_location a_loc, const ELogLevel a_level, const std::string_view a_fmt)
    {
        const auto loc = spdlog::source_loc{ a_loc.file_name(), static_cast<std::int32_t>(a_loc.line()), a_loc.function_name() };
        switch (a_level) {
            case ELogLevel::Trace:
                spdlog::default_logger_raw()->log(loc, spdlog::level::trace, spdlog::string_view_t{a_fmt.data(), a_fmt.size()});
                break;
            case ELogLevel::Debug:
                spdlog::default_logger_raw()->log(loc, spdlog::level::debug, spdlog::string_view_t{a_fmt.data(), a_fmt.size()});
                break;
            case ELogLevel::Info:
                spdlog::default_logger_raw()->log(loc, spdlog::level::info, spdlog::string_view_t{a_fmt.data(), a_fmt.size()});
                break;
            case ELogLevel::Warning:
                spdlog::default_logger_raw()->log(loc, spdlog::level::warn, spdlog::string_view_t{a_fmt.data(), a_fmt.size()});
                break;
            case ELogLevel::Error:
                spdlog::default_logger_raw()->log(loc, spdlog::level::err, spdlog::string_view_t{a_fmt.data(), a_fmt.size()});
                break;
            case ELogLevel::Critical:
                spdlog::default_logger_raw()->log(loc, spdlog::level::critical, spdlog::string_view_t{a_fmt.data(), a_fmt.size()});
                break;
        }
    }

    void Log(const std::source_location a_loc, const ELogLevel a_level, const std::wstring_view a_fmt)
    {
        const auto to_utf8 = [](const std::wstring_view a_wide) -> std::string {
            if (a_wide.empty()) {
                return {};
            }
            std::string out;
            out.reserve(a_wide.size());
            for (const wchar_t ch : a_wide) {
                if (ch >= 0 && ch <= 0x7F) {
                    out.push_back(static_cast<char>(ch));
                } else {
                    out.push_back('?');
                }
            }
            return out;
        };

        const auto loc = spdlog::source_loc{ a_loc.file_name(), static_cast<std::int32_t>(a_loc.line()), a_loc.function_name() };
        const auto utf8 = to_utf8(a_fmt);
        const auto msg = std::string_view{ utf8 };
        switch (a_level) {
            case ELogLevel::Trace:
                spdlog::default_logger_raw()->log(loc, spdlog::level::trace, spdlog::string_view_t{msg.data(), msg.size()});
                break;
            case ELogLevel::Debug:
                spdlog::default_logger_raw()->log(loc, spdlog::level::debug, spdlog::string_view_t{msg.data(), msg.size()});
                break;
            case ELogLevel::Info:
                spdlog::default_logger_raw()->log(loc, spdlog::level::info, spdlog::string_view_t{msg.data(), msg.size()});
                break;
            case ELogLevel::Warning:
                spdlog::default_logger_raw()->log(loc, spdlog::level::warn, spdlog::string_view_t{msg.data(), msg.size()});
                break;
            case ELogLevel::Error:
                spdlog::default_logger_raw()->log(loc, spdlog::level::err, spdlog::string_view_t{msg.data(), msg.size()});
                break;
            case ELogLevel::Critical:
                spdlog::default_logger_raw()->log(loc, spdlog::level::critical, spdlog::string_view_t{msg.data(), msg.size()});
                break;
        }
    }

    void Fail(const std::source_location a_loc, const std::string_view a_fmt)
    {
        Log(a_loc, ELogLevel::Critical, a_fmt);
        __debugbreak();
    }

    void Fail(const std::source_location a_loc, const std::wstring_view a_fmt)
    {
        Log(a_loc, ELogLevel::Critical, a_fmt);
        __debugbreak();
    }
}
