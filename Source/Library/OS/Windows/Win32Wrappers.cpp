#include "Win32Wrappers.hpp"
#include <Windows.h>

namespace Yupei
{
    int CloseHandleWrapper(NativeWindowHandle handle) noexcept
    {
        return ::CloseHandle(handle);
    }

	std::uint32_t GetLastErrorWrapper() noexcept
	{
		return static_cast<std::uint32_t>(GetLastError());
	}
}
