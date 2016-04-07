#pragma once

#include <system_error>
#include "WinDef.hpp"
#include "Win32Wrappers.hpp"

namespace Yupei
{
	class Win32Exception : public std::system_error
	{
	public:
		Win32Exception(std::uint32_t err)
			:system_error{{static_cast<int>(err), std::system_category()}}
		{}
	};

#define THROW_WIN32_EXCEPTION [](){throw Win32Exception(Yupei::GetLastErrorWrapper());}()
}