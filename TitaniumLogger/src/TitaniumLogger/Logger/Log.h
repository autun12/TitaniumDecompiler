#pragma once

#include <memory>

// This ignores all warnings raised inside External headers
#if defined(_MSC_VER)
#pragma warning(push, 0)
#elif defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored \
    "-Wshadow"  // Add any other heavy groups if needed
#endif

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace TitaniumLogger {
class Log {
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetRendererLogger() {
        return s_RendererLogger;
    }

    static std::shared_ptr<spdlog::logger>& GetDecompilerLogger() {
        return s_DecompilerLogger;
    }

    static std::shared_ptr<spdlog::logger>& GetClientLogger() {
        return s_ClientLogger;
    }

private:
    static std::shared_ptr<spdlog::logger> s_RendererLogger;
    static std::shared_ptr<spdlog::logger> s_DecompilerLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
}  // namespace TitaniumLogger

// Core log macros
#define TD_RENDERER_TRACE(...) \
    ::TitaniumLogger::Log::GetRendererLogger()->trace(__VA_ARGS__)
#define TD_RENDERER_INFO(...) \
    ::TitaniumLogger::Log::GetRendererLogger()->info(__VA_ARGS__)
#define TD_RENDERER_WARN(...) \
    ::TitaniumLogger::Log::GetRendererLogger()->warn(__VA_ARGS__)
#define TD_RENDERER_ERROR(...) \
    ::TitaniumLogger::Log::GetRendererLogger()->error(__VA_ARGS__)
#define TD_RENDERER_CRITICAL(...) \
    ::TitaniumLogger::Log::GetRendererLogger()->critical(__VA_ARGS__)

// Client log macros
#define TD_TRACE(...) \
    ::TitaniumLogger::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TD_INFO(...) ::TitaniumLogger::Log::GetClientLogger()->info(__VA_ARGS__)
#define TD_WARN(...) ::TitaniumLogger::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TD_ERROR(...) \
    ::TitaniumLogger::Log::GetClientLogger()->error(__VA_ARGS__)
#define TD_CRITICAL(...) \
    ::TitaniumLogger::Log::GetClientLogger()->critical(__VA_ARGS__)

// Client log macros
#define TD_DECOMP_TRACE(...) \
    ::TitaniumLogger::Log::GetDecompilerLogger()->trace(__VA_ARGS__)
#define TD_DECOMP_INFO(...) \
    ::TitaniumLogger::Log::GetDecompilerLogger()->info(__VA_ARGS__)
#define TD_DECOMP_WARN(...) \
    ::TitaniumLogger::Log::GetDecompilerLogger()->warn(__VA_ARGS__)
#define TD_DECOMP_ERROR(...) \
    ::TitaniumLogger::Log::GetDecompilerLogger()->error(__VA_ARGS__)
#define TD_DECOMP_CRITICAL(...) \
    ::TitaniumLogger::Log::GetDecompilerLogger()->critical(__VA_ARGS__)
