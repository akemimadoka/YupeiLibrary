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

	namespace Internal
	{
		struct WrapInt
		{
			constexpr WrapInt(int) {}
		};

	}
}