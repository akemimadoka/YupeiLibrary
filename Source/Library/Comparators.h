#pragma once

#include <utility>  //for std::forward
#include <cstdint>

namespace Yupei
{
#define DEFINE_COMP(Name,Op) template <typename T = void> \
	struct Name\
	{\
		constexpr bool operator()(const T& x, const T& y) const\
		{\
			return x Op y;\
		}\
	};\
    template <> \
	struct Name<void> \
	{\
		template <class T, class U>\
		constexpr auto operator()(T&& t, U&& u) const\
			-> decltype(std::forward<T>(t) Op std::forward<U>(u))\
		{\
			return std::forward<T>(t) Op std::forward<U>(u);\
		}\
		typedef int is_transparent;\
	};

    DEFINE_COMP(equal_to, == )
    DEFINE_COMP(not_equal_to, != )
    DEFINE_COMP(greater, >)
    DEFINE_COMP(less, <)
    DEFINE_COMP(greater_equal, >= )
    DEFINE_COMP(less_equal, <= )

#undef DEFINE_COMP
}
