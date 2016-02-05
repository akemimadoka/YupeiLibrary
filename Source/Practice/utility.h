#pragma once

#include "utility_internal.h"
#include "type_traits.h"
#include "__reference_wrapper.h"
#include "__swap.h"
#include "integral_sequence.h"

namespace Yupei
{	


	template<typename...>
	class tuple;

	namespace rel_ops
	{
		template<typename Type> 
		bool operator != (const Type& lhs, const Type& rhs)
		{
			return !(lhs == rhs);
		}

		template<typename Type>
		bool operator > (const Type& lhs, const Type& rhs)
		{
			return rhs < lhs;
		}

		template<typename Type>
		bool operator <= (const Type& lhs, const Type& rhs)
		{
			return !(rhs < lhs);
		}

		template<typename Type>
		bool operator >= (const Type& lhs, const Type& rhs)
		{
			return !(lhs < rhs);
		}
	}
	// 20.2.3, exchange:
	//template <class T, class U = T> T exchange(T& obj, U&& new_val);

	template <typename T, typename U = T> 
	T exchange(T& obj, U&& new_val)
	{
		T old_val = Yupei::move(obj);
		obj = Yupei::forward<U>(new_val);
		return old_val;
	}

	template <typename T>
	constexpr conditional_t<!is_nothrow_move_constructible<T>::value 
		&& is_copy_constructible<T>::value,
		const T&, 
		T&&> move_if_noexcept(T& x) noexcept
	{
		return Yupei::move(x);
	}

	

	
}
