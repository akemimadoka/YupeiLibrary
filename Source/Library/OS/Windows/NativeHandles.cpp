#include "NativeHandles.hpp"
#include "WinDef.hpp"
#include "Win32Exception.hpp"
#include <type_traits>		
#include <Windows.h>
#include <WinSock2.h>

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

	void DllHandleCloser::operator()(ModuleHandle handle) noexcept
	{
		static_assert(std::is_same<ModuleHandle, ::HMODULE>::value, "ModuleHandle should be the same with HMODULE");
		(void)::FreeLibrary(handle);
	}

	void SocketCloser::operator()(HandleType handle) noexcept
	{
		(void)::closesocket(static_cast<::SOCKET>(handle));
	}

	void WsaEventCloser::operator()(HandleType handle) noexcept
	{
		static_assert(std::is_same<HandleType, WSAEVENT>::value, "HandleType should be the same with WSAEVENT");
		if (::WSACloseEvent(handle) == FALSE)
			THROW_WINSOCK_EXCEPTION;	//terminate();
	}
}