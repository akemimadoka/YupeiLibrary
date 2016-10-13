#include "WinSock.hpp"
#include "Win32Exception.hpp"
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

namespace Yupei
{
	std::uint32_t WSAGetLastErrorWrapper() noexcept
	{
		return static_cast<std::uint32_t>(::WSAGetLastError());
	}

	void SocketCloser::operator()(HandleType handle) noexcept
	{
		(void)::closesocket(static_cast<::SOCKET>(handle));
	}

	void WsaEventCloser::operator()(HandleType handle) noexcept
	{
		static_assert(std::is_same<HandleType, WSAEVENT>::value, "HandleType should be the same with WSAEVENT");
		WinSockTry(::WSACloseEvent(handle));
	}

}