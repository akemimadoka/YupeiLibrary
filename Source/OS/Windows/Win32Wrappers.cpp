#include "WinDef.hpp"
#include "Win32Wrappers.hpp"
#include "NativeHandles.hpp"
#include <Windows.h>

namespace Yupei
{
	std::uint32_t GetLastErrorWrapper() noexcept
	{
		return static_cast<std::uint32_t>(::GetLastError());
	}

	std::u16string ErrCodeToMessage(std::uint32_t errCode)
	{
		const auto systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		LocalMemoryHandle localMemory;
		auto isOk = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		{}, static_cast<DWORD>(errCode), systemLocale, reinterpret_cast<wchar_t*>(localMemory.AddressOf()), {}, {});
		if (!isOk)
		{
			ModuleHandle dllHandle { ::LoadLibraryEx(L"netmsg.dll", {}, DONT_RESOLVE_DLL_REFERENCES) };
			if (dllHandle)
				isOk = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
				{}, static_cast<DWORD>(errCode), systemLocale, reinterpret_cast<wchar_t*>(localMemory.AddressOf()), {}, {});
		}
		if (isOk && localMemory)
		{
			return { reinterpret_cast<char16_t*>(static_cast<wchar_t*>(::LocalLock(localMemory.Get()))) };
		}
		return { u"Couldn't find error message." };
	}
}
