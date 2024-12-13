#pragma once

#include "VNR-Core/Core/CoreDefines.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace VNR
{
	class Log
	{
	public:
		static void Init();

		static SharedPtr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static SharedPtr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static SharedPtr<spdlog::logger> s_CoreLogger;
		static SharedPtr<spdlog::logger> s_ClientLogger;

	};

// Core
#define VNR_CORE_TRACE(...)		::VNR::Log::GetCoreLogger()->trace(__VA_ARGS__) 
#define VNR_CORE_INFO(...)		::VNR::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VNR_CORE_WARN(...)		::VNR::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VNR_CORE_ERROR(...)		::VNR::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VNR_CORE_CRITICAL(...)	::VNR::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client
#define VNR_TRACE(...)			::VNR::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VNR_INFO(...)			::VNR::Log::GetClientLogger()->info(__VA_ARGS__)
#define VNR_WARN(...)			::VNR::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VNR_ERROR(...)			::VNR::Log::GetClientLogger()->error(__VA_ARGS__)
#define VNR_CRITICAL(...)		::VNR::Log::GetClientLogger()->critical(__VA_ARGS__)
}