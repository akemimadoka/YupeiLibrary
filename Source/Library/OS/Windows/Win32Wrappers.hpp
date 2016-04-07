#pragma once

#include "WinDef.hpp"
#include "..\..\String.hpp"

namespace Yupei
{
    extern int CloseHandleWrapper(NativeWindowHandle handle) noexcept;
	extern NativeWindowHandle LocalFreeWrapper(NativeWindowHandle handle) noexcept;
	extern int FreeLibraryWrapper(void* moduleHandle) noexcept;
	extern std::uint32_t GetLastErrorWrapper() noexcept;
	extern utf16_string ErrCodeToMessage(std::uint32_t errCode);
}
