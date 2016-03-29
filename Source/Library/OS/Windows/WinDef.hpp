#pragma once

#include <cstdint>

namespace Yupei
{
    //https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx

    using NativeWindowHandle = void*;   
    static constexpr auto InvalidWindowHandleValue = static_cast<std::intptr_t>(-1);

	inline NativeWindowHandle GetInvalidWindowHandle() noexcept
	{
		return reinterpret_cast<NativeWindowHandle>(InvalidWindowHandleValue);
	}

    //TODO: undef GetMessage stuff.
}