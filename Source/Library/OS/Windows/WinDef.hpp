#pragma once

#include <cstdint>

#define UNICODE

//Win32 Stuff
struct HINSTANCE__;

namespace Yupei
{
    //https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx

    using NativeWindowHandle = void*;   
    static constexpr auto InvalidWindowHandleValue = static_cast<std::intptr_t>(-1);

	using DWord = unsigned long;

	using InstanceHandle = ::HINSTANCE__*;
	using ModuleHandle = InstanceHandle;

	inline NativeWindowHandle GetInvalidWindowHandle() noexcept
	{
		return reinterpret_cast<NativeWindowHandle>(InvalidWindowHandleValue);
	}

    //TODO: undef GetMessage stuff.
}