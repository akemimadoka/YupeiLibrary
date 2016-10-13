#pragma once

#include "../../HandleWrapper.hpp"
#include "Win32Exception.hpp"
#include "WinDef.hpp"
#include <cstdint>

namespace Yupei
{
	extern std::uint32_t WSAGetLastErrorWrapper() noexcept;

	[[noreturn]]
	inline void ThrowWinSockException()
	{
		throw Win32Exception { WSAGetLastErrorWrapper() };
	}

	inline void WinSockTry(Bool b)
	{
		if (b != True)
			ThrowWinSockException();
	}

	struct SocketCloser
	{
		using HandleType = std::uintptr_t;
		static constexpr HandleType InvalidHandle = static_cast<std::uintptr_t>(-1);

		void operator()(HandleType handle) noexcept;
	};

	using Socket = HandleWrapper<SocketCloser>;

	struct WsaEventCloser
	{
		using HandleType = NativeHandle;
		static constexpr HandleType InvalidHandle = {};

		void operator()(HandleType handle) noexcept;
	};

	using WsaEvent = HandleWrapper<WsaEventCloser>;

}
