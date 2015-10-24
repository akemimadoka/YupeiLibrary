#pragma once

#include "type_traits.h"

namespace Yupei
{
	namespace Internal
	{
		template<typename T>
		inline T* address_of_impl(T& arg, true_type)
		{
			return arg;
		}

		template<typename T>
		inline T* address_of_impl(T& arg, false_type)
		{
			return reinterpret_cast<T*>(
				&const_cast<char&>(
					reinterpret_cast<const volatile char&>(arg)));
		}
	}

	template<typename T>
	inline T* addressof(T& arg)
	{
		return Internal::address_of_impl(arg, is_function<remove_pointer_t<T>>{});
	}

}
