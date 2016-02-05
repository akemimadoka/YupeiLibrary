#pragma once

#include "compare_funtor.h"
#include "utility.h"
#include <initializer_list>
#include <cassert>

#if defined(min) || defined(max)
#error YupeiLibrary is incompatiable with Windows' min/max.
#endif

namespace Yupei
{
	template<typename IteratorType, typename FunctorType>
	IteratorType max_element(IteratorType first,
		IteratorType last, FunctorType functor)
	{
		auto res = first;
		for (;first != last;++first)
		{
			if (functor(*res, *first))
				res = first;
		}
		return res;
	}

	template<typename IteratorType, typename FunctorType>
	IteratorType min_element(IteratorType first,
		IteratorType last, FunctorType functor)
	{
		auto res = first;
		for (;first != last;++first)
		{
			if (functor(*first, *res))
				res = first;
		}
		return res;
	}
	
	template<class ForwardIterator, class Compare>
	Yupei::pair<ForwardIterator, ForwardIterator>
		minmax_element(ForwardIterator first, ForwardIterator last, Compare comp)
	{
		pair<ForwardIterator, ForwardIterator> res{ first,first };
		ForwardIterator nextIt = first;
		for (;first != last;++first)
		{
			if ((++nextIt) == last)
			{
				if (comp(*first, *res.first)) res.first = first;
				else if (comp(*res.second, *first)) res.second = first;
				return res;
			}
			else if (comp(*first, *nextIt))
			{
				//first is smaller
				if (comp(*first, *res.first)) res.first = first;
				if (comp(*res.second, *nextIt)) res.second = nextIt;
			}
			else
			{
				//second_obj is smaller
				if (comp(*nextIt, *res.first)) res.first = nextIt;
				if (comp(*res.second, *first)) res.second = first;
			}
		}
		return res;
	}
	template<class ForwardIterator>
	inline Yupei::pair<ForwardIterator, ForwardIterator>
		minmax_element(ForwardIterator first, ForwardIterator last)
	{
		return minmax_element(first, last, Yupei::less<>{});
	}
	template<typename IteratorType>
	inline IteratorType max_element(IteratorType first,
		IteratorType last)
	{
		return Yupei::max_element(first, last, less<>{});
	}
	template<typename IteratorType>
	inline IteratorType min_element(IteratorType first,
		IteratorType last)
	{
		return Yupei::min_element(first, last, less<>{});
	}
	// 25.4.7, minimum and maximum:
	template<class T> 
	inline constexpr const T& min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}
	template<class T, class Compare>
	inline constexpr const T& min(const T& a, const T& b, Compare comp)
	{
		return comp(a, b) ? a : b;
	}
	template<class T>
	inline constexpr const T& max(const T& a, const T& b)
	{
		return b < a ? a : b;
	}
	template<class T, class Compare>
	inline constexpr const T& max(const T& a, const T& b, Compare comp)
	{
		return comp(b, a) ? a : b;
	}
	//workaround for VS2015
	template<class T, class Compare>
	inline constexpr Yupei::pair<const T&, const T&> minmax(const T& a, const T& b, Compare comp)
	{
		return comp(a,b) ? Yupei::make_pair(a, b) : Yupei::make_pair(b, a);
	}
	template<class T>
	inline constexpr Yupei::pair<const T&, const T&> minmax(const T& a, const T& b)
	{
		return minmax(a, b, Yupei::less<>{});
	}
	template<class T>
	inline Yupei::pair<T, T> minmax(std::initializer_list<T> t)
	{
		return minmax(t, Yupei::less<>{});
	}
	template<class T, class Compare>
	inline Yupei::pair<T, T> minmax(std::initializer_list<T> t, Compare comp)
	{
		auto res = Yupei::minmax_element(t.begin(), t.end(),comp);
		return Yupei::make_pair(*res.first, *res.second);
	}
	template<class T>
	inline T min(std::initializer_list<T> t)
	{
		return *(Yupei::min_element(t.begin(), t.end()));
	}
	template<class T>
	inline T max(std::initializer_list<T> t)
	{
		return *(Yupei::max_element(t.begin(), t.end()));
	}
}
