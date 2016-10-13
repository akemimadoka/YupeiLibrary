#pragma once

#include "WinDef.hpp"
#include <string>

namespace Yupei
{
	extern std::uint32_t GetLastErrorWrapper() noexcept;
	extern std::u16string ErrCodeToMessage(std::uint32_t errCode);
}
