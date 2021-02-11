#pragma once

#include "TitaniumRenderer/Core/Base.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace TitaniumRenderer {

class Log {
public:
    static void Init();

    static Ref<spdlog::logger>& GetCoreLogger() {
        return s_CoreLogger;
    }
    static Ref<spdlog::logger>& GetClientLogger() {
        return s_ClientLogger;
    }

private:
    static Ref<spdlog::logger> s_CoreLogger;
    static Ref<spdlog::logger> s_ClientLogger;
};

}  // namespace TitaniumRenderer

// Core log macros
#define TD_CORE_TRACE(...) ::TitaniumRenderer::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TD_CORE_INFO(...) ::TitaniumRenderer::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TD_CORE_WARN(...) ::TitaniumRenderer::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TD_CORE_ERROR(...) ::TitaniumRenderer::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TD_CORE_CRITICAL(...) ::TitaniumRenderer::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define TD_TRACE(...) ::TitaniumRenderer::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TD_INFO(...) ::TitaniumRenderer::Log::GetClientLogger()->info(__VA_ARGS__)
#define TD_WARN(...) ::TitaniumRenderer::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TD_ERROR(...) ::TitaniumRenderer::Log::GetClientLogger()->error(__VA_ARGS__)
#define TD_CRITICAL(...) ::TitaniumRenderer::Log::GetClientLogger()->critical(__VA_ARGS__)