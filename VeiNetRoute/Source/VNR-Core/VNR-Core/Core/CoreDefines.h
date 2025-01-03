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

#define MAX_LOOP_ITERATIONS 10000

#include <vector>
#include <functional>

#define DECLARE_DELEGATE_ONE_PARAM(name, P1TYPE)										 \
struct name																				 \
{																						 \
public:																					 \
	void Add(std::function<void(P1TYPE)> callback) {callbacks.push_back(callback);}		 \
	void Broadcast(P1TYPE p1)															 \
	{																					 \
		for (auto& func : callbacks)													 \
		{																				 \
			func(p1);																	 \
		}																				 \
	}																					 \
	void Invoke(P1TYPE p1)																 \
	{																					 \
		Broadcast(p1);																	 \
		Clear();																		 \
	}																					 \
	void Clear()																		 \
	{																					 \
		callbacks.clear();																 \
	}																					 \
private:																				 \
	std::vector<std::function<void(P1TYPE)>> callbacks;									 \
};	



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

namespace VNR
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
   using UniquePtr = std::unique_ptr<T>;
   template<typename T, typename ... Args>
   constexpr UniquePtr<T> MakeUnique(Args&& ... args)
   {
	   return std::make_unique<T>(std::forward<Args>(args)...);
   }

   template<typename T>
   using SharedPtr = std::shared_ptr<T>;
   template<typename T, typename ... Args>
   constexpr SharedPtr<T> MakeShared(Args&& ... args)
   {
	   return std::make_shared<T>(std::forward<Args>(args)...);
   }
}

#define ENUM_CLASS_CONVERSIONS(EnumType)          \
    constexpr int operator+(EnumType e) { return static_cast<uint8>(e); } \
	constexpr EnumType To##EnumType(int i) { return static_cast<EnumType>(i); }


#include "VNR-Core/IO/Log.h"