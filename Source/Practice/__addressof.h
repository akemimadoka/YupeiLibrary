#pragma once

#include "type_traits.h"

namespace Yupei
{
	namespace Internal
	{
		template<typename T>
		inline T* AddressOfImpl(T& arg, true_type) noexcept
		{
			return arg;
		}

		template<typename T>
		inline T* AddressOfImpl(T& arg, false_type) noexcept
		{
			return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(arg)));
		}
	}

	template<typename T>
	inline T* addressof(T& arg) noexcept
	{
		return Internal::AddressOfImpl(arg, is_function<T>{});
	}

}
