#pragma once

#include <system_error>
#include "WinDef.hpp"
#include "Win32Wrappers.hpp"

namespace Yupei
{
	class Win32Exception : public std::system_error
	{
	public:
		Win32Exception(std::uint32_t err);
	};

#define THROW_WIN32_EXCEPTION [](){throw Yupei::Win32Exception(Yupei::GetLastErrorWrapper());}()
}