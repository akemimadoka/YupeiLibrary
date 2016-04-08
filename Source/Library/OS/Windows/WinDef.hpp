#pragma once

#include <cstdint>

#if !defined(UNICODE)
#define UNICODE
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

//Win32 Stuff
struct HINSTANCE__;

namespace Yupei
{
    //https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx

    using NativeHandle = void*;   
    static constexpr auto InvalidNativeHandleValue = static_cast<std::intptr_t>(-1);

	using DWord = unsigned long;

	using InstanceHandle = ::HINSTANCE__*;
	using ModuleHandle = InstanceHandle;

	inline NativeHandle GetInvalidNativeHandle() noexcept
	{
		return reinterpret_cast<NativeHandle>(InvalidNativeHandleValue);
	}

    //TODO: undef GetMessage stuff.
}