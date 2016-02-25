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

#define TOKEN_PASTEx(x, y) x ## y
#define TOKEN_PASTE(x, y) TOKEN_PASTEx(x, y)

#define STRx(s) #s
#define STR(s) STRx(s)

namespace Yupei
{
    using byte = unsigned char;
}
	
