#include "Win32Exception.hpp"

namespace Yupei
{
	Win32Exception::Win32Exception(std::uint32_t err)
		:system_error { static_cast<int>(err), std::system_category()}
	{}
}