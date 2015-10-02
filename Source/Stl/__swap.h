#pragma once


#include "type_traits.h"
#include "utility_internal.h"

namespace Yupei
{
	template<typename ObjectT>
	void swap(
		ObjectT& lhs, 
		ObjectT& rhs) noexcept(
		is_nothrow_move_constructible<ObjectT>::value&&
		is_nothrow_move_assignable<ObjectT>::value)
	{
		ObjectT temp(Yupei::move(lhs));
		lhs = Yupei::move(rhs);
		rhs = Yupei::move(temp);
	}

	template<class ForwardIt1T, class ForwardIt2T>
	void iter_swap(
		ForwardIt1T a, 
		ForwardIt2T b) noexcept(noexcept(swap(
		*Yupei::declval<ForwardIt1T>(),
		*Yupei::declval<ForwardIt1T>())))
	{
		swap(*a, *b);
	}

	template<class ForwardIt1T, class ForwardIt2T>
	ForwardIt2T
		swap_ranges(
			ForwardIt1T first1, 
			ForwardIt1T last1,
			ForwardIt2T first2) noexcept(noexcept(Yupei::iter_swap(
				Yupei::declval<ForwardIt1T>(),
				Yupei::declval<ForwardIt2T>())))
	{
		for (;first1 != last1;++first1,static_cast<void>(++first2))
			Yupei::iter_swap(first1, first2);
	}


	template<
		typename ObjectT,
		std::size_t N>
		void swap(ObjectT(&lhs)[N], ObjectT(&rhs)[N]) 
		noexcept(noexcept(swap(Yupei::declval<ObjectT&>(), Yupei::declval<ObjectT&>())))
	{
		Yupei::swap_ranges(lhs, lhs + N, rhs);
	}

	/*template<typename Type>
		using is_nothrow_swappable = bool_constant <
		noexcept(swap(declval<Type&>(), declval<Type&>()))>;*/

}