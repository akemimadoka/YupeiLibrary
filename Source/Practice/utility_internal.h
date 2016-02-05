#pragma once

#include "traits_internal.h"

namespace Yupei
{
	template<typename T>
	inline constexpr T&& forward(remove_reference_t<T>& value)
	{
		//lvalue
		return static_cast<T&&>(value);
	}

	template<typename T>
	inline constexpr T&& forward(remove_reference_t<T>&& value)
	{
		//rvalue
		return static_cast<T&&>(value);
	}

	template<typename T>
	constexpr remove_reference_t<T>&& rvalue_cast(T&& value)
	{
		return static_cast<remove_reference_t<T>&&>(value);
	}

	template<typename T>
	constexpr remove_reference_t<T>&& move(T&& value)
	{
		return static_cast<remove_reference_t<T>&&>(value);
	}

	template<typename Type>
	add_rvalue_reference_t<Type> declval() noexcept;

	//The erased_type struct is an empty struct that serves as a placeholder for a type T 
	//in situations where the actual type T is determined at runtime.For example, 
	//the nested type, allocator_type, is an alias for erased_type in classes that use type -
	//erased allocators(see[type.erased.allocator]).
	inline namespace Experimental
	{
		struct erased_type {};
	}
	
}