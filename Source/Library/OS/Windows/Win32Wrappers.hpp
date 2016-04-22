#pragma once

#include "WinDef.hpp"
#include "..\..\String.hpp"

namespace Yupei
{
	/* extern int CloseHandleWrapper(NativeHandle handle) noexcept;
	 extern NativeHandle LocalFreeWrapper(NativeHandle handle) noexcept;
	 extern int FreeLibraryWrapper(void* moduleHandle) noexcept;*/
	extern std::uint32_t GetLastErrorWrapper() noexcept;
	extern std::uint32_t WSAGetLastErrorWrapper() noexcept;
	extern u16string ErrCodeToMessage(std::uint32_t errCode);
}
