// #include "TitaniumRenderer/Core/Base.h"
#include "TitaniumLogger/Logger/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>

namespace TitaniumLogger {

std::shared_ptr<spdlog::logger> Log::s_RendererLogger;
std::shared_ptr<spdlog::logger> Log::s_DecompilerLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init() {
    std::vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("TitaniumDecompiler.log", true));

    logSinks[0]->set_pattern("%^[%T] %n: %v%$");
    logSinks[1]->set_pattern("[%T] [%l] %n: %v");

    s_RendererLogger = std::make_shared<spdlog::logger>("TitaniumRenderer", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_RendererLogger);
    s_RendererLogger->set_level(spdlog::level::trace);
    s_RendererLogger->flush_on(spdlog::level::trace);

    s_DecompilerLogger = std::make_shared<spdlog::logger>("TitaniumDecompiler", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_DecompilerLogger);
    s_DecompilerLogger->set_level(spdlog::level::trace);
    s_DecompilerLogger->flush_on(spdlog::level::trace);

    s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
	spdlog::register_logger(s_ClientLogger);
	s_ClientLogger->set_level(spdlog::level::trace);
	s_ClientLogger->flush_on(spdlog::level::trace);
}

}  // namespace TitaniumRenderer