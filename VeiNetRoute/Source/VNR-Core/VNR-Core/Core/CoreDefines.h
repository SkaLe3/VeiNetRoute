#pragma once

#ifdef VNR_DEBUG
	#define VNR_DEBUGBREAK() __debugbreak()
	#define VNR_ENABLE_ASSERTS
	#define VNR_ENABLE_PROFILING
	#define VNR_LOG_LEVEL_DEBUG
#else
	#define VNR_DEBUGBREAK()
#endif


#define VNR_EXPAND_MACRO(x) x
#define VNR_STRINGIFY_MACRO(x) #x
#define BIT(x) (1 << x)
#define TEXT(quote) L##quote

#define VNR_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) {return this->fn(std::forward<decltype(args)>(args)...);}


#ifdef VNR_ENABLE_ASSERTS
	#define VNR_INTERNAL_ASSERT_IMPL(type, check, msg, ...) {if(!(check)) { VNR##type##ERROR(msg, __VA_ARGS__); VNR_DEBUGBREAK();}}
	#define VNR_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VNR_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define	VNR_INTERNAL_ASSERT_NO_MSG(type, check) VNR_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VNR_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().filename().string(), __LINE__)
	
	#define VNR_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define VNR_INTERNAL_ASSERT_GET_MACRO(...) VNR_EXPAND_MACRO(VNR_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VNR_INTERNAL_ASSERT_WITH_MSG, VNR_INTERNAL_ASSERT_NO_MSG))
	
	#define	VNR_ASSERT(...) VNR_EXPAND_MACRO( VNR_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
	#define	VNR_CORE_ASSERT(...) VNR_EXPAND_MACRO( VNR_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
	#define VNR_ASSERT(...)
	#define VNR_CORE_ASSERT(...)
#endif

#include <memory>
#include <string>
#include <filesystem>
#include <cstdint>

namespace VeiM
{
	using byte = uint8_t;
	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;

	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;
	using size_t = ::std::size_t;

	using String = ::std::string;

   template<typename T>
   using Scope = std::unique_ptr<T>;
   template<typename T, typename ... Args>
   constexpr Scope<T> CreateScope(Args&& ... args)
   {
	   return std::make_unique<T>(std::forward<Args>(args)...);
   }

   template<typename T>
   using Ref = std::shared_ptr<T>;
   template<typename T, typename ... Args>
   constexpr Ref<T> CreateRef(Args&& ... args)
   {
	   return std::make_shared<T>(std::forward<Args>(args)...);
   }
}
#include "VNR-Core/IO/Log.h"