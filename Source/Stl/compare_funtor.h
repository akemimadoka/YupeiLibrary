#pragma once

#include "utility_internal.h"

namespace Yupei
{
	// 20.9.6, comparisons:

#define DEFINE_COMP(Name,Op) template <class T = void> \
	struct Name\
	{\
		constexpr bool operator()(const T& x, const T& y) const\
		{\
			return x Op y;\
		}\
		typedef T first_argument_type;\
		typedef T second_argument_type;\
		typedef bool result_type;\
	};\
    template <> \
	struct Name<void> \
	{\
		template <class T, class U>\
		constexpr auto operator()(T&& t, U&& u) const\
			-> decltype(Yupei::forward<T>(t) Op Yupei::forward<U>(u))\
		{\
			return Yupei::forward<T>(t) Op Yupei::forward<U>(u);\
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
