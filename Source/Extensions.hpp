#pragma once

#include <utility>
#include <memory>

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

#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str)	\
	TOKEN_PASTE(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str)	\
	TOKEN_PASTE(str, __LINE__)
#endif

#define exforward(x) std::forward<decltype((x))>((x))

namespace Yupei
{
    using byte = unsigned char;

	template<typename T, typename DeleteFn, typename... ParamsT>
	std::unique_ptr<T, DeleteFn> make_unique_del(DeleteFn&& fn, ParamsT&&... params)
	{
		return std::unique_ptr<T, DeleteFn>{new T(std::forward<ParamsT>(params)...), std::forward<DeleteFn>(fn)};
	}
}
	
