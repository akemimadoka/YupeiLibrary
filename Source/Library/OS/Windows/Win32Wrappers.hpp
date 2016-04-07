#pragma once

#include "WinDef.hpp"

namespace Yupei
{
    extern int CloseHandleWrapper(NativeWindowHandle handle) noexcept;
	extern std::uint32_t GetLastErrorWrapper() noexcept;
}
