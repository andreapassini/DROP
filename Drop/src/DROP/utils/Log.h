#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Drop
{

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define LOG_CORE_TRACE(...)    ::Drop::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)     ::Drop::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)     ::Drop::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)    ::Drop::Log::GetCoreLogger()->error(__FILE__, "\t", __FUNCTION__, "\t", __LINE__, "\t", __VA_ARGS__)
#define LOG_CORE_FATAL(...)    ::Drop::Log::GetCoreLogger()->fatal(__FILE__, "\t", __FUNCTION__, "\t", __LINE__, "\t", __VA_ARGS__)

// Client log macros
#define LOG_TRACE(...)	      ::Drop::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)	      ::Drop::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)	      ::Drop::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)	      ::Drop::Log::GetClientLogger()->error(__FILE__, "\t", __FUNCTION__, "\t", __LINE__, "\t", __VA_ARGS__)
#define LOG_FATAL(...)	      ::Drop::Log::GetClientLogger()->fatal(__FILE__, "\t", __FUNCTION__, "\t", __LINE__, "\t", __VA_ARGS__)