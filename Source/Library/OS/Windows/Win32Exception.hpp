#pragma once

#include <system_error>
#include "WinDef.hpp"
#include "Win32Wrappers.hpp"

namespace Yupei
{
	class Win32Exception : public std::system_error
	{
		utf8_string errStr_;

	public:
		Win32Exception(std::uint32_t err);
		const char* what() const override;
	};

#define THROW_WIN32_EXCEPTION [](){throw Yupei::Win32Exception(Yupei::GetLastErrorWrapper());}()
#define THROW_WINSOCK_EXCEPTION [](){throw Yupei::Win32Exception(Yupei::WSAGetLastErrorWrapper());}()

}