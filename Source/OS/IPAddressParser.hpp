#pragma once

#include "../Config.hpp"
#include <optional>

#if defined(YPWINDOWS)
#include "Windows/Win32IPAddressParser.hpp"
#else
#error "Unsupported."
#endif

namespace Yupei
{

}