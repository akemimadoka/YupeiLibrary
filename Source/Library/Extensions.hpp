#pragma once

#define DISABLECOPY(TYPE) \
	TYPE(const TYPE&) = delete;\
	TYPE& operator=(const TYPE&) = delete;

#define DEFAULTCOPY(TYPE) \
	TYPE(const TYPE&) = default;\
	TYPE& operator=(const TYPE&) = default;

#define DISABLEMOVE(TYPE) \
	TYPE(TYPE&&) = delete;\
	TYPE& operator=(TYPE&&) = delete;

#define DEFAULTMOVE(TYPE) \
	TYPE(TYPE&&) noexcept = default;\
	TYPE& operator=(TYPE&&) noexcept = default;

namespace Yupei
{
    using byte = unsigned char;
}
	
