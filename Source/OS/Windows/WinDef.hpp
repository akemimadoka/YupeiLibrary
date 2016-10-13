#pragma once

#include <cstdint>

#if !defined(UNICODE)
#define UNICODE
#endif

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

//Win32 Data Types
struct HINSTANCE__;
struct HKEY__;
struct in_addr;
struct in6_addr;

namespace Yupei
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx
	using NativeHandle = void*;
	static constexpr auto InvalidNativeHandleValue = static_cast<std::intptr_t>(-1);

	using Bool = int;
	static constexpr int True = 1;
	static constexpr int False = 0;

	using DWord = unsigned long;
	using NativeInstanceHandle = ::HINSTANCE__*;
	using NativeKeyHandle = ::HKEY__*;
	using NativeModuleHandle = NativeInstanceHandle;

	inline NativeHandle GetInvalidNativeHandle() noexcept
	{
		return reinterpret_cast<NativeHandle>(InvalidNativeHandleValue);
	}
}