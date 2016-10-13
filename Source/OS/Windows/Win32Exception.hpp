#pragma once

#include <system_error>
#include "WinDef.hpp"
#include "Win32Wrappers.hpp"

namespace Yupei
{
	//TODO: 应该添加行号等详细信息。
	class Win32Exception : public std::system_error
	{
		std::string errStr_;

	public:
		Win32Exception(std::uint32_t err);
		const char* what() const override;
	};

    [[noreturn]]
    inline void ThrowWin32Exception()
    {
        throw Win32Exception { GetLastErrorWrapper() };
    }

    inline void Win32Try(Bool b)
    {
        if (b != True)
            ThrowWin32Exception();
    } 
}