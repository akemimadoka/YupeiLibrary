#pragma once

#include "algorithm_internal.h"
#include "compare_funtor.h"
#include "iterator.h"
#include "utility.h"

namespace Yupei
{
	template<
		typename InputItT,
		typename PredT>
	PredT for_each(
		InputItT first,
		InputItT last,
		PredT pred)
	{
		for (; first != last; ++first)
			pred(*first);
		
		return Yupei::move(pred);
	}

	template<
		typename InputItT,
		typename PredT>
		bool all_of(
			InputItT first,
			InputItT last,
			PredT pred)
	{
		if (first == last) return false;

		for (;first != last;++first)
			if (!pred(*first)) return false;
		
		return true;
	}
	template<
		typename InputItT,
		typename PredT>
		bool any_of(
			InputItT first,
			InputItT last,
			PredT pred)
	{
		for (;first != last;++first)
			if (pred(*first)) return true;
		return false;
	}

	template<
		typename InputItT,
		typename PredT>
		bool none_of(
			InputItT first,
			InputItT last,
			PredT pred)
	{
		for (;first != last;++first)
			if (pred(*first) == true) return false;
		return true;
	}

	template<
		typename InputItT,
		typename ObjectT>
		InputItT find(
			InputItT first,
			InputItT last,
			const ObjectT& value)
	{
		for (;first != last;++first)
			if (*first == value) return first;
		return last;
	}
	template<
		typename InputItT,
		typename PredT>
		InputItT find_if(
			InputItT first,
			InputItT last,
			PredT pred)
	{
		for (;first != last;++first)
			if (pred(*first)) return first;
		return last;
	}
	template<
		typename InputItT,
		typename PredT>
		InputItT find_if_not(
			InputItT first,
			InputItT last,
			PredT pred)
	{
		for (;first != last;++first)
			if (!pred(*first)) return first;
		return last;
	}
	
	template<
		typename ForwardIt1T,
		typename ForwardIt2T,
		typename PredT>
		ForwardIt1T search(
			ForwardIt1T first1,
			ForwardIt1T last1,
			ForwardIt2T first2,
			ForwardIt2T last2,
			PredT pred)
	{
		for (;first1 != last1;++first1)
		{
			auto nowIt = first1;
			//begin with nowIt
			for (ForwardIt2T nowIt2 = first2;;++nowIt2, (void)++nowIt)
			{
				if (nowIt2 == last2) return first1;
				if (!pred(*nowIt,*nowIt2) break;
			}
		}
		return last1;
	}

	//Searches for the first occurrence of the subsequence of elements [s_first, s_last) 
	//in the range [first, last - (s_last - s_first)). 
	template<
		typename ForwardIt1T,
		typename ForwardIt2T>
		ForwardIt1T search(
			ForwardIt1T first1,
			ForwardIt1T last1,
			ForwardIt2T first2,
			ForwardIt2T last2)
	{
		return Yupei::search(first1, last1, first2, last2, Yupei::equal_to<>());
	}

	//Searches for the last subsequence of elements [s_first, s_last) in the range [first, last).
	template<
		typename ForwardIt1T,
		typename ForwardIt2T,
		typename PredT>
		ForwardIt1T find_end(
			ForwardIt1T first1,
			ForwardIt1T last1,
			ForwardIt2T first2,
			ForwardIt2T last2,
			PredT pred)
	{
		if (first2 == last2) return last;
		ForwardIt1T result;
		while (true)
		{
			//just call search repeatedly to find the last subsequence.
			auto new_result = Yupei::search(first1, last1, first2, last2, pred);
			if (new_result == last1) return result;
			else
			{
				//update the result,and move the first1 forward
				result = new_result;
				first1 = result;
				++first1;
			}
		}
	}

	template<
		typename ForwardIt1T,
		typename ForwardIt2T>
		ForwardIt1T find_end(
			ForwardIt1T first1,
			ForwardIt1T last1,
			ForwardIt2T first2,
			ForwardIt2T last2)
	{
		return Yupei::find_end(first1, last1, first2, last2, Yupei::equal_to<>());
	}

	template<
		typename InputItT,
		typename ObjectT>
	DifferenceType<InputItT> 
		count(
			InputItT first,
			InputItT last,
			const ObjectT& value)
	{
		DifferenceType<InputItT> res{};
		for (;first != last;++first)
			if (*first == value) ++res;
		return res;
	}

	template<
		typename InputItT,
		typename PredT>
		DifferenceType<InputItT>
		count_if(
			InputItT first,
			InputItT last,
			PredT pred)
	{
		DifferenceType<InputItT> res{};
		for (;first != last;++first)
			if (pred(*first)) ++res;
		return res;
	}

	template<
		typename InputIt1T,
		typename InputIt2T,
		typename PredT>
		pair<InputIt1T, InputIt2T>
		mismatch(
			InputIt1T first1,
			InputIt1T last1,
			InputIt2T first2,
			InputIt2T last2,
			PredT pred)
	{
		for (;first1 != last1 && first2 != last2;++first1, (void)++first2)
			if (!pred(*first1, *first2)) break;
		return make_pair(first1, first2);
	}

	template<
		typename InputIt1T,
		typename InputIt2T>
		pair<InputIt1T, InputIt2T>
		mismatch(
			InputIt1T first1,
			InputIt1T last1,
			InputIt2T first2,
			InputIt2T last2)
	{
		return Yupei::mismatch(first1, last1, first2, last2, Yupei::equal_to<>());
	}

	template<
		typename InputIt1T,
		typename InputIt2T,
		typename PredT>
	pair<InputIt1T,InputIt2T>
		mismatch(
			InputIt1T first1,
			InputIt1T last1,
			InputIt2T first2,
			PredT pred)
	{
		auto newFirst2 = first2;
		auto last2 = Yupei::advance(newFirst2, Yupei::distance(first1, last1));
		return mismatch(first1, last1, first2, last2, Yupei::equal_to<>());
	}

	template<
		typename InputIt1T,
		typename InputIt2T>
		pair<InputIt1T, InputIt2T>
		mismatch(
			InputIt1T first1,
			InputIt1T last1,
			InputIt2T first2)
	{
		return mismatch(first1, last1, first2, Yupei::equal_to<>());
	}
	
	template<
		typename ForwardIt1T,
		typename ForwardIt2T,
		typename PredT>
		bool is_permutation(
			ForwardIt1T first1,
			ForwardIt1T last1,
			ForwardIt2T first2,
			ForwardIt2T last2,
			PredT pred)
	{
		auto firstPair = Yupei::mismatch(first1, last1, first2, last2, pred);
		first1 = firstPair.first;
		first2 = firstPair.second;
		//skip all duplicated elements
		for (ForwardIt1T i = first1;i != last1;++first1)
		{
			if (i != find(first1, i, *i)) continue;
			//we have skipped all duplicated elements,so we just look at elements after i
			//i.e. 122235692,after the first occurance of '2',the rest of the '2' will be skipped
			auto m = Yupei::count(i, last1, *i);
			if (m == 0 || m != Yupei::count(first2, last2, *i))
				return false;
		}
	}
}
