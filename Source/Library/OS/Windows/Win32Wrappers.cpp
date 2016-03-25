#include "Win32Wrappers.hpp"
#include <Windows.h>

namespace Yupei
{
    int CloseHandleWrapper(NativeWindowHandle handle) noexcept
    {
        return ::CloseHandle(handle);
    }
}
