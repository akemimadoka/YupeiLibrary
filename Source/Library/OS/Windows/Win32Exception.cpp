#include "Win32Exception.hpp"

namespace Yupei
{
	Win32Exception::Win32Exception(std::uint32_t err)
		:system_error { static_cast<int>(err), std::system_category()},
		errStr_{to_utf8(ErrCodeToMessage(static_cast<DWord>(err)).to_string_view())}
	{}

	const char * Win32Exception::what() const
	{
		return errStr_.c_str();
	}
}