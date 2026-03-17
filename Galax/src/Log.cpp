#include "Log.h"

// Static member definitions
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

void Log::Init() {
	// Patterns can be changed https://github.com/gabime/spdlog/wiki/Custom-formatting
	// %^ = start color range %T = time %n = logger name %v = actual message  %$ = end color range
	spdlog::set_pattern("%^[%T] %n: %v%$");
	//%l = level info

	// Create the core logger
	s_CoreLogger = spdlog::stdout_color_mt("Galax");
	s_CoreLogger->set_level(spdlog::level::trace);
}