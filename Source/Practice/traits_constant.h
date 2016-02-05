#pragma once

#include <type_traits>

namespace Yupei
{
	using std::integral_constant;

	//C++ 17 bool_constant
	template<bool B>
	using bool_constant = integral_constant<bool, B>;

	using true_type = bool_constant<true>;
	using false_type = bool_constant<false>;

	template<std::size_t N>
	using size_constant = integral_constant<std::size_t, N>;

	namespace Internal
	{
		struct WrapInt
		{
			constexpr WrapInt(int) {}
		};

	}
}