#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

// Took this from my Luxia Engine

class Log
{
private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
};

// Macros, in code you can call GX_INFO("message"); and itll log to the core logger!
#define GX_TRACE(...)    ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define GX_INFO(...)     ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define GX_WARN(...)     ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define GX_ERROR(...)    ::Log::GetCoreLogger()->error(__VA_ARGS__)
