#include "NativeHandles.hpp"
#include "WinDef.hpp"
#include "Win32Exception.hpp"
#include <type_traits>		
#include <Windows.h>

namespace Yupei
{
	const NativeHandle FileHandleCloser::InvalidHandle = GetInvalidNativeHandle();

	void FileHandleCloser::operator()(NativeHandle handle) noexcept
	{
		static_assert(std::is_same<NativeHandle, ::HANDLE>::value, "NativeHandle should be the same with HANDLE");
		(void)::CloseHandle(handle);
	}

	void LocalMemoryHandleCloser::operator()(NativeHandle handle) noexcept
	{
		static_assert(std::is_same<NativeHandle, ::HLOCAL>::value, "NativeHandle should be the same with HLOCAL");
		(void)::LocalFree(handle);
	}

	void NativeModuleHandleCloser::operator()(NativeModuleHandle handle) noexcept
	{
		static_assert(std::is_same<NativeModuleHandle, ::HMODULE>::value, "ModuleHandle should be the same with HMODULE");
		(void)::FreeLibrary(handle);
	}
	
	void RegKeyCloser::operator()(HandleType handle) noexcept
	{
		static_assert(std::is_same<HandleType, ::HKEY>::value, "HandleType should be the same with HKEY");
		(void)::RegCloseKey(handle);
	}
}