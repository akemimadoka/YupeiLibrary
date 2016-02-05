#pragma once

#include "algorithm_internal.h"
#include "compare_funtor.h"
#include "iterator.h"
#include "utility.h"
#include "__swap.h"
#include "..\CLib\raw_memory.h"
#include "..\Custom\Assert.h"

namespace Yupei
{
	template<typename InputItT, typename PredT>
	inline PredT for_each(InputItT first, InputItT last, PredT pred)
	{
		for (; first != last; ++first)
			pred(*first);

		return Yupei::move(pred);
	}

	template<typename InputItT, typename PredT>
	inline bool all_of(InputItT first, InputItT last, PredT pred)
	{
		if (first == last) return false;

		for (;first != last;++first)
			if (!pred(*first)) return false;

		return true;
	}
	template<typename InputItT, typename PredT>
	inline bool any_of(InputItT first, InputItT last, PredT pred)
	{
		for (;first != last;++first)
			if (pred(*first)) return true;
		return false;
	}

	template<typename InputItT, typename PredT>
	inline bool none_of(InputItT first, InputItT last, PredT pred)
	{
		for (;first != last;++first)
			if (pred(*first) == true) return false;
		return true;
	}

	template<typename InputItT, typename ObjectT>
	inline InputItT find(InputItT first, InputItT last, const ObjectT& value)
	{
		for (;first != last;++first)
			if (*first == value) return first;
		return last;
	}

	template<typename InputItT, typename PredT>
	inline InputItT find_if(InputItT first, InputItT last, PredT pred)
	{
		for (;first != last;++first)
			if (pred(*first)) return first;
		return last;
	}
	template<typename InputItT, typename PredT>
	inline InputItT find_if_not(InputItT first, InputItT last, PredT pred)
	{
		for (;first != last;++first)
			if (!pred(*first)) return first;
		return last;
	}

	template<typename ForwardIt1T, typename ForwardIt2T, typename PredT>
	inline ForwardIt1T search(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2, ForwardIt2T last2, PredT pred)
	{
		for (;;++first1)
		{
			auto nowIt = first1;
			//begin with nowIt
			for (ForwardIt2T nowIt2 = first2;;++nowIt2, (void)++nowIt)
			{
				if (nowIt2 == last2) return first1;
				if (nowIt == last1) return last1;
				if (!pred(*nowIt, *nowIt2)) break;
			}
		}
		return last1;
	}

	//Searches for the first occurrence of the subsequence of elements [s_first, s_last) 
	//in the range [first, last - (s_last - s_first)). 
	template<typename ForwardIt1T, typename ForwardIt2T>
	inline ForwardIt1T search(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2, ForwardIt2T last2)
	{
		return Yupei::search(first1, last1, first2, last2, Yupei::equal_to<>());
	}

	namespace Internal
	{
		template<typename ForwardIt1T, typename ForwardIt2T, typename PredT>
		inline ForwardIt1T FindEndImp(
			ForwardIt1T first1, ForwardIt1T last1,
			ForwardIt2T first2, ForwardIt2T last2,
			PredT pred,
			forward_iterator_tag,
			forward_iterator_tag)
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


		template<typename ForwardIt1T, typename ForwardIt2T, typename PredT>
		inline ForwardIt1T FindEndImp(
			ForwardIt1T first1, ForwardIt1T last1,
			ForwardIt2T first2, ForwardIt2T last2,
			PredT pred,
			bidirectional_iterator_tag,
			bidirectional_iterator_tag)
		{
			//we can search from last to first
			auto realLast1 = last1;
			auto realLast2 = last2;
			--realLast2;
			while (true)
			{
				while (true)
				{
					//no starting point found, return.
					if (realLast1 == first1) return last1;
					//find the starting point
					if (pred(*--realLast1, *realLast2)) break;
				}
				//realLast1 holds the last element of the last sequence
				//We'll validate the elements before it
				auto tempLast1 = realLast1;
				auto tempLast2 = realLast2;
				while (true)
				{
					if (tempLast2 == first2) return tempLast1;
					if (tempLast1 == first1) return last1;
					if (!pred(*--tempLast1, *--tempLast2)) break;
				}
			}
		}

		template<typename ForwardIt1T, typename ForwardIt2T, typename PredT>
		inline ForwardIt1T FindEndImp(
			ForwardIt1T first1, ForwardIt1T last1,
			ForwardIt2T first2, ForwardIt2T last2,
			PredT pred,
			random_access_iterator_tag,
			random_access_iterator_tag)
		{
			//First, we compare last2 - first2 with last1 - first1
			if (last1 - first1 < last2 - first2) return last1;
			//Then, we can modify the algorithm is bid_iterator:
			//because we know the farest the iterator could reach is first1 + last2 - first2
			auto realLast1 = last1;
			auto realLast2 = last2;
			auto farestIt = first1 + (last2 - first2);
			--realLast2;
			while (true)
			{
				while (true)
				{
					if (realLast1 == farestIt) return last1;
					if (pred(*--realLast1, *realLast2)) break;
				}
				auto tempLast1 = realLast1;
				auto tempLast2 = realLast2;
				while (true)
				{
					if (tempLast2 == first2) return tempLast1;
					if (!pred(*--tempLast2, *--tempLast1)) break;
				}
			}
		}
	}

	//Searches for the last subsequence of elements [s_first, s_last) in the range [first, last).
	template<typename ForwardIt1T, typename ForwardIt2T, typename PredT>
	inline ForwardIt1T find_end(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2, ForwardIt2T last2,
		PredT pred)
	{
		return Internal::FindEndImp(first1, last1, first2, last2, pred, IteratorCategory<ForwardIt1T>(), IteratorCategory<ForwardIt2T>());
	}

	template<typename ForwardIt1T, typename ForwardIt2T>
	inline ForwardIt1T find_end(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2, ForwardIt2T last2)
	{
		return Yupei::find_end(first1, last1, first2, last2, equal_to<>());
	}

	template<typename InputItT, typename ObjectT>
	inline difference_t<InputItT>
		count(InputItT first, InputItT last, const ObjectT& value)
	{
		difference_t<InputItT> res{};
		for (;first != last;++first)
			if (*first == value) ++res;
		return res;
	}

	template<typename InputItT, typename PredT>
	inline difference_t<InputItT>
		count_if(InputItT first, InputItT last, PredT pred)
	{
		difference_t<InputItT> res{};
		for (;first != last;++first)
			if (pred(*first)) ++res;
		return res;
	}

	template<typename InputIt1T, typename InputIt2T, typename PredT>
	inline pair<InputIt1T, InputIt2T>
		mismatch(
			InputIt1T first1, InputIt1T last1,
			InputIt2T first2, InputIt2T last2,
			PredT pred)
	{
		for (;first1 != last1 && first2 != last2;++first1, (void)++first2)
			if (!pred(*first1, *first2)) break;
		return Yupei::make_pair(first1, first2);
	}

	template<typename InputIt1T, typename InputIt2T>
	inline pair<InputIt1T, InputIt2T>
		mismatch(
			InputIt1T first1, InputIt1T last1,
			InputIt2T first2, InputIt2T last2)
	{
		return Yupei::mismatch(first1, last1, first2, last2, equal_to<>());
	}

	template<typename InputIt1T, typename InputIt2T, typename PredT>
	inline pair<InputIt1T, InputIt2T>
		mismatch(
			InputIt1T first1, InputIt1T last1,
			InputIt2T first2,
			PredT pred)
	{
		for (;first1 != last1;++first1, (void)++first2)
			if (!pred(*first1, *first2)) break;
		return make_pair(first1, first2);
	}

	template<typename InputIt1T, typename InputIt2T>
	inline pair<InputIt1T, InputIt2T>
		mismatch(
			InputIt1T first1, InputIt1T last1,
			InputIt2T first2)
	{
		return mismatch(first1, last1, first2, Yupei::equal_to<>());
	}

	template<typename ForwardIt1T, typename ForwardIt2T, typename PredT>
	inline bool is_permutation(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2, ForwardIt2T last2,
		PredT pred)
	{
		auto firstPair = Yupei::mismatch(first1, last1, first2, last2, pred);
		first1 = firstPair.first;
		first2 = firstPair.second;
		//skip all duplicated elements
		for (ForwardIt1T i = first1;i != last1;++first1)
		{
			if (i != find_if(first1, i, pred)) continue;
			//we have skipped all duplicated elements,so we just look at elements after i
			//i.e. 122235692,after the first occurance of '2',the rest of the '2' will be skipped
			auto m = Yupei::count_if(i, last1, pred);
			if (m == 0 || m != Yupei::count_if(first2, last2, pred))
				return false;
		}
		return true;
	}

	template<typename ForwardIt1T, typename ForwardIt2T>
	inline bool is_permutation(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2, ForwardIt2T last2)
	{
		auto firstPair = Yupei::mismatch(first1, last1, first2, last2);
		first1 = firstPair.first;
		first2 = firstPair.second;
		//skip all duplicated elements
		for (ForwardIt1T i = first1;i != last1;++i)
		{
			if (i != Yupei::find(first1, i, *i)) continue;
			//we have skipped all duplicated elements,so we just look at elements after i
			//i.e. 122235692,after the first occurance of '2',the rest of the '2' will be skipped
			auto m = Yupei::count(i, last1, *i);
			if (m == 0 || m != Yupei::count(first2, last2, *i))
				return false;
		}
		return true;
	}

	template<typename ForwardIt1T, typename ForwardIt2T, typename PredT>
	inline bool is_permutation(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2,
		PredT pred)
	{
		if (first1 == last1) return true;
		ForwardIt2T last2 = first2;
		Yupei::advance(last2, Yupei::distance(first1, last1));
		return Yupei::is_permutation(first1, last1, first2, last2, pred);
	}

	template<typename ForwardIt1T, typename ForwardIt2T>
	inline bool is_permutation(
		ForwardIt1T first1, ForwardIt1T last1,
		ForwardIt2T first2)
	{
		if (first1 == last1) return true;
		ForwardIt2T last2 = first2;
		Yupei::advance(last2, Yupei::distance(first1, last1));
		return Yupei::is_permutation(first1, last1, first2, last2);
	}

	//Return Value
	//Iterator to the first element in the range [first, last) that is equal to an element from the range [s_first; s_last). 
	//If no such element is found, last is returned. 
	template<typename InputItT, typename ForwardItT, typename PredT>
	inline InputItT find_first_of(
		InputItT first1, InputItT last1,
		ForwardItT first2, ForwardItT last2,
		PredT pred)
	{
		for (;first1 != last1;++first1)
			for (ForwardItT i = first2;i != last2;++i)
				if (pred(*first1, *i)) return first1;
		return last1;
	}

	template<typename InputItT, typename ForwardItT>
	inline InputItT find_first_of(
		InputItT first1, InputItT last1,
		ForwardItT first2, ForwardItT last2)
	{
		return Yupei::find_first_of(first1, last1, first2, last2, Yupei::equal_to<>());
	}

	template<typename ForwardItT, typename PredT>
	inline ForwardItT adjacent_find(
		ForwardItT first, ForwardItT last,
		PredT pred)
	{
		if (first == last) return last;
		auto next = first;
		++next;
		for (;next != last;++next, (void)++first)
			if (pred(*first, *next)) return first;
		return last;
	}

	template<typename ForwardItT>
	inline ForwardItT adjacent_find(
		ForwardItT first, ForwardItT last)
	{
		return Yupei::adjacent_find(first, last, Yupei::equal_to<>());
	}

	namespace Internal
	{
		template<typename InputIt1T, typename InputIt2T, typename PredT>
		inline bool EqualImp(
			InputIt1T first1, InputIt1T last1,
			InputIt2T first2, InputIt2T last2,
			PredT pred)
		{
			return Yupei::mismatch(first1, last1, first2, last2, pred) == Yupei::make_pair(last1, last2);
		}
	}

	template<typename InputIt1T, typename InputIt2T, typename PredT>
	inline bool equal(
		InputIt1T first1, InputIt1T last1,
		InputIt2T first2, InputIt2T last2,
		PredT pred,
		true_type)
	{
		if (last1 - first1 != last2 - first2) return false;
		return Internal::EqualImp(first1, last1, first2, last2, pred);
	}

	template<
		typename InputIt1T,
		typename InputIt2T,
		typename PredT>
		inline bool equal(
			InputIt1T first1, InputIt1T last1,
			InputIt2T first2, InputIt2T last2,
			PredT pred,
			false_type)
	{
		return Internal::EqualImp(first1, last1, first2, last2, pred);
	}

	template<
		typename InputIt1T,
		typename InputIt2T,
		typename PredT>
		inline bool equal(
			InputIt1T first1, InputIt1T last1,
			InputIt2T first2, InputIt2T last2,
			PredT pred)
	{
		//Complexity: No applications of the corresponding predicate if InputIterator1 and InputIterator2
		//meet the requirements of random access iterators and last1 - first1 != last2 - first2.Otherwise,
		//at most min(last1 - first1, last2 - first2) applications of the corresponding predicate.
		return Yupei::equal(first1, last1, first2, last2, pred,
			bool_constant<
			is_same<random_access_iterator_tag, IteratorCategory<InputIt1T>>::value&&
			is_same<random_access_iterator_tag, IteratorCategory<InputIt2T>>::value>());
	}

	template<typename InputIt1T, typename InputIt2T>
	inline bool equal(
		InputIt1T first1, InputIt1T last1,
		InputIt2T first2, InputIt2T last2)
	{
		return Yupei::equal(first1, last1, first2, last2, Yupei::equal_to<>());
	}

	template<typename InputIt1T, typename InputIt2T, typename PredT>
	inline bool equal(
		InputIt1T first1, InputIt1T last1,
		InputIt2T first2,
		PredT pred)
	{
		for (;first1 != last1;++first1, (void)++first2)
		{
			if (!pred(*first1, *first2)) return false;
		}
		return true;
	}

	template<typename InputIt1T, typename InputIt2T>
	inline bool equal(
		InputIt1T first1, InputIt1T last1,
		InputIt2T first2)
	{
		return Yupei::equal(first1, last1, first2, Yupei::equal_to<>());
	}

	namespace Internal
	{
		template<typename ForwardItT, typename SizeT, typename ObjectT, typename PredT>
		inline ForwardItT search_n(
			ForwardItT first, ForwardItT last,
			SizeT count,
			const ObjectT& value,
			PredT pred,
			random_access_iterator_tag)
		{
			SizeT nCount{};
			auto realLast = last - count;
			while (true)
			{
				while (true)
				{
					if (first == realLast) return last;
					if (pred(*first, value))
					{
						++nCount;
						break;
					}
					++first;
				}
				auto tempFirst = first;
				while (true)
				{
					if (nCount == count) return first;
					if (!pred(*++first, value))
						break;
					++nCount;
				}
				nCount = SizeT{};
			}
			return last;
		}

		template<typename ForwardItT, typename SizeT, typename ObjectT, typename PredT>
		inline ForwardItT search_n(
			ForwardItT first,ForwardItT last,
			SizeT count,
			const ObjectT& value,
			PredT pred,
			forward_iterator_tag)
		{
			SizeT nCount{};
			while (true)
			{
				while (true)
				{
					if (first == last) return last;
					if (pred(*first, value))
					{
						++nCount;
						break;
					}
					++first;
				}
				auto tempFirst = first;
				while (true)
				{
					if (nCount == count) return first;
					if (first == last) return last;
					if (!pred(*++first, value))
						break;
					++nCount;
				}
				nCount = SizeT{};
			}
			return last;
		}
	}

	template<typename ForwardItT,typename SizeT,typename ObjectT,typename PredT>
		inline ForwardItT search_n(
			ForwardItT first,ForwardItT last,
			SizeT count,
			const ObjectT& value,
			PredT pred)
	{
		return Internal::search_n(first, last, count, value, pred, IteratorCategory<ForwardItT>());
	}

	template<typename ForwardItT,typename SizeT,typename ObjectT>
		inline ForwardItT search_n(
			ForwardItT first,ForwardItT last,
			SizeT count,
			const ObjectT& value)
	{
		return Yupei::search_n(first, last, count, value, Yupei::equal_to<>());
	}

	namespace Internal
	{

		template<typename It1T,typename It2T>
			using could_use_memmove = bool_constant<is_same<remove_cv_t<value_type_t<It1T>>, remove_cv_t<value_type_t<It2T>>>::value &&
			is_pod<value_type_t<It1T>>::value && is_pod<value_type_t<It2T>>::value &&
			is_pointer<It1T>::value && is_pointer<It2T>::value>;

		template<typename InputItT,typename OutputItT>
			inline OutputItT CopyImp(
				InputItT first,InputItT last,
				OutputItT result,
				true_type)//both are POD pointers
		{
			//they are pointers!!
			std::ptrdiff_t dist = last - first;
			Yupei::memmove(static_cast<void*>(result), static_cast<const void*>(first), static_cast<std::size_t>(dist));
			return result + dist;
		}

		template<typename InputItT,typename OutputItT>
			inline OutputItT CopyImp(
				InputItT first,InputItT last,
				OutputItT result,
				false_type)//sorry,they aren't both pointers
		{
			for (;first != last;++first, (void)++result)
				*result = *first;
			return result;
		}
	}

	template<typename InputItT,typename OutputItT>
		inline OutputItT copy(
			InputItT first,InputItT last,
			OutputItT result)
	{
		return Internal::CopyImp(first, last, result, Internal::could_use_memmove<InputItT, OutputItT>());
	}

	namespace Internal
	{
		template<typename InputItT,typename SizeT,typename OutputItT>
			inline OutputItT CopyNImp(
				InputItT first,
				SizeT n,
				OutputItT result,
				true_type)
		{
			Yupei::memmove(static_cast<void*>(result), static_cast<const void*>(first), n * sizeof(value_type_t<InputItT>));
			return result + n;
		}

		template<typename InputItT,typename SizeT,typename OutputItT>
			inline OutputItT CopyNImp(
				InputItT first,
				SizeT n,
				OutputItT result,
				false_type)
		{
			for (SizeT i{};i < n;++i, (void)++first, (void)result)
				*first = *result;
			return result;
		}
	}

	template<typename InputItT,typename SizeT,typename OutputItT>
		inline OutputItT copy_n(
			InputItT first,
			SizeT n,
			OutputItT result)
	{
		return Internal::CopyNImp(first, n, result, Internal::could_use_memmove<InputItT, OutputItT>());
	}

	template<typename InputItT,typename OutputItT,typename PredT>
		inline OutputItT copy_if(
			InputItT first,InputItT last,
			OutputItT result,
			PredT pred)
	{
		for (;first != last;++first, (void)++result)
			if (pred(*first)) *result = *first;
		return result;
	}

	namespace Internal
	{
		template<typename BidIt1T,typename BidIt2T>
		inline BidIt2T CopyBackwardImp(
				BidIt1T first,BidIt1T last,
				BidIt2T result,
				true_type)
		{
			std::ptrdiff_t dist = last - first;
			Yupei::memmove(static_cast<void*>(result - dist), static_cast<const void*>(first), dist * sizeof(value_type_t<BidIt1T>));
			return result - dist;
		}

		template<typename BidIt1T,typename BidIt2T>
			inline BidIt2T CopyBackwardImp(
				BidIt1T first,BidIt1T last,
				BidIt2T result,
				false_type)
		{
			for (;last != first;--last, (void)--result)
				*result = *last;
			return result;
		}
	}

	template<typename BidIt1T,typename BidIt2T>
	inline BidIt2T copy_backward(
			BidIt1T first,BidIt1T last,
			BidIt2T result)
	{
		return Internal::CopyBackwardImp(first, last, result, Internal::could_use_memmove<BidIt1T, BidIt2T>());
	}

	namespace Internal
	{
		template<typename InputItT,typename OutputItT>
		inline OutputItT MoveImp(
				InputItT first,InputItT last,
				OutputItT result,
				true_type)//both are POD pointers
		{
			//they are pointers!!
			std::ptrdiff_t dist = last - first;
			Yupei::memmove(static_cast<void*>(result), static_cast<const void*>(first), static_cast<std::size_t>(dist));
			return result + dist;
		}

		template<typename InputItT,typename OutputItT>
		inline OutputItT MoveImp(
				InputItT first,InputItT last,
				OutputItT result,
				false_type)//sorry,they aren't both pointers
		{
			for (;first != last;++first, (void)++result)
				*result = Yupei::move(*first);
			return result;
		}
	}

	template<typename InputItT,typename OutputItT>
	inline OutputItT move(
			InputItT first,InputItT last,
			OutputItT result)
	{
		return Internal::MoveImp(first, last, result, Internal::could_use_memmove<InputItT, OutputItT>());
	}

	namespace Internal
	{
		template<typename BidIt1T,typename BidIt2T>
		inline BidIt2T MoveBackwardImp(
				BidIt1T first,BidIt1T last,
				BidIt2T result,
				true_type)
		{
			std::ptrdiff_t dist = last - first;
			Yupei::memmove(static_cast<void*>(result - dist), static_cast<const void*>(first), dist * sizeof(value_type_t<BidIt1T>));
			return result - dist;
		}

		template<typename BidIt1T,typename BidIt2T>
		inline BidIt2T MoveBackwardImp(
				BidIt1T first,BidIt1T last,
				BidIt2T result,
				false_type)
		{
			for (;last != first;)
				*--result = Yupei::move(*--last);
			return result;
		}
	}

	template<typename BidIt1T,typename BidIt2T>
		inline BidIt2T move_backward(
			BidIt1T first,BidIt1T last,
			BidIt2T result)
	{
		return Internal::MoveBackwardImp(first, last, result, Internal::could_use_memmove<BidIt1T, BidIt2T>());
	}

	template<typename InputItT,typename OutputItT,typename UnaryOpT>
	inline OutputItT transform(
			InputItT first,InputItT last,
			OutputItT result,
			UnaryOpT op)
	{
		for (;first != last;++first, (void)++result)
			*result = op(*first);
		return result;
	}

	template<typename InputIt1T,typename InputIt2T,typename OutputItT,typename BinaryOpT>
		inline OutputItT transform(
			InputIt1T first1,
			InputIt1T last1,
			InputIt2T first2,
			OutputItT result,
			BinaryOpT op)
	{
		for (;first1 != last1;++first1, (void)first2, (void)++result)
			*result = op(*first1, *first2);
		return result;
	}

	template<typename ForwardItT,typename ObjectT>
	inline void replace(
			ForwardItT first,
			ForwardItT last,
			const ObjectT& old_value,
			const ObjectT& new_value)
	{
		for (;first != last;++first)
			if (*first == old_value)
				*first = new_value;
	}

	template<typename ForwardItT,typename PredT,typename ObjectT>
	inline void replace_if(
			ForwardItT first,ForwardItT last,
			PredT pred,
			const ObjectT& new_value)
	{
		for (;first != last;++first)
			if (pred(*first))
				*first = new_value;
	}

	template<typename InputItT,typename OutputItT,typename ObjectT>
		inline OutputItT replace_copy(
			InputItT first,InputItT last,
			OutputItT result,
			const ObjectT& old_value,
			const ObjectT& new_value)
	{
		for (;first != last;++first, (void)++last)
		{
			if (*first == old_value)
				*result = new_value;
			else *result = *first;
		}
		return result;
	}

	template<typename InputItT,typename OutputItT,typename PredT,typename ObjectT>
		inline OutputItT replace_copy_if(
			InputItT first,InputItT last,
			OutputItT result,
			PredT pred,
			const ObjectT& new_value)
	{
		for (;first != last;++first, (void)++last)
		{
			if (pred(*first))
				*result = new_value;
			else *result = *first;
		}
		return result;
	}

	namespace Internal
	{
		template<typename FwdItT, typename ObjectT>
		inline void FillImp(FwdItT first, FwdItT last, const ObjectT& v)
		{
			for (;first != last;++first)
				*first = v;
		}

		inline void FillImp(char* first, char* last, char v)
		{
			Yupei::memset(first, static_cast<int>(v), last - first);
		}

		inline void FillImp(signed char* first, signed char* last, signed char v)
		{
			Yupei::memset(first, static_cast<int>(v), last - first);
		}

		inline void FillImp(unsigned char* first, unsigned char* last, unsigned char v)
		{
			Yupei::memset(first, static_cast<int>(v), last - first);
		}
	}

	template<typename FwdItT, typename ObjectT>
	inline void fill(FwdItT first, FwdItT last, const ObjectT& v)
	{
		return Internal::FillImp(first, last, v);
	}

	namespace Internal
	{
		template<typename FwdItT, typename SizeT, typename ObjectT>
		inline FwdItT FillNImp(FwdItT first, SizeT n, const ObjectT& v)
		{
			for (;0 < n;--n, (void)++first)
				*first = v;
			return first;
		}

		inline char* FillNImp(char* first, std::size_t n, char v)
		{
			Yupei::memset(first, static_cast<int>(v), n);
			return first + n;
		}

		inline signed char* FillNImp(signed char* first, std::size_t n, signed char v)
		{
			Yupei::memset(first, static_cast<int>(v), n);
			return first + n;
		}

		inline unsigned char* FillNImp(unsigned char* first, std::size_t n, unsigned char v)
		{
			Yupei::memset(first, static_cast<int>(v), n);
			return first + n;
		}
	}

	template<typename FwdItT, typename SizeT, typename ObjectT>
	inline void fill_n(FwdItT first, SizeT n, const ObjectT& v)
	{
		return Internal::FillNImp(first, n, v);
	}

	template<typename ForwardItT,typename GenT>
		inline void generate(
			ForwardItT first,ForwardItT last,
			GenT gen)
	{
		for (;first != last;++first)
			*first = gen();
	}

	template<typename OutputItT,typename SizeT,typename GenT>
	inline void generate(
			OutputItT first,
			SizeT n,
			GenT gen)
	{
		for (SizeT i{};i < n;++i)
			*first++ = gen();
	}

	template<typename ForwardItT,typename PredT>
	inline ForwardItT unique(
			ForwardItT first,ForwardItT last,
			PredT pred)
	{
		first = Yupei::adjacent_find(first, last, pred);
		if (first != last)
		{
			ForwardItT frontIt = first;
			for (++frontIt;frontIt != last;++frontIt)
			{
				if (!pred(*first, *frontIt))
					*++first = Yupei::move(*frontIt);
			}
			++first;
		}
		return first;
	}

	template<typename ForwardItT>
	inline ForwardItT unique(ForwardItT first,ForwardItT last)
	{
		return Yupei::unique(first, last, Yupei::equal_to<>());
	}

	namespace Internal
	{
		template<typename InputItT,typename OutputItT,typename PredT>
			inline OutputItT UniqueCopyImp(
				InputItT first,InputItT last,
				OutputItT result,
				PredT pred,
				input_iterator_tag)
		{
			if (first != last)
			{
				value_type_t<InputItT> temp(*first);
				*result = temp;
				++result;
				for (++first;first != last;++first)
				{
					if (!pred(temp, *first))
					{
						temp = *first;
						*result = temp;
						++result;
					}
				}
			}
			return ++result;
		}

		template<typename InputItT,typename OutputItT,typename PredT>
			inline OutputItT UniqueCopyImp(
				InputItT first,InputItT last,
				OutputItT result,
				PredT pred,
				forward_iterator_tag)
		{
			if (first != last)
			{
				auto frontIt = first;
				*result = *first;
				++result;
				for (++frontIt;frontIt != last;++frontIt)
				{
					if (!pred(*first, *frontIt))
					{
						*result = *frontIt;
						++result;
						first = frontIt;
					}
				}
			}
			return result;
		}

	}

	template<typename InputItT,typename OutputItT,typename PredT>
		inline OutputItT unique_copy(
			InputItT first,InputItT last,
			OutputItT result,
			PredT pred)
	{
		return Internal::UniqueCopyImp(first, last, result, pred, IteratorCategory<InputItT>());
	}

	namespace Internal
	{
		template<typename BidItT>
		inline void ReverseImp(BidItT first, BidItT last, bidirectional_iterator_tag)
		{
			while (first != last)
			{
				if (first == --last)
					break;
				Yupei::iter_swap(first, last);
				++first;
			}
		}

		template<typename BidItT>
		inline void ReverseImp(BidItT first, BidItT last, random_access_iterator_tag)
		{
			if (first != last)
				for (;first < --last;++first)
					Yupei::iter_swap(first, last);
		}
	}

	template<typename BidItT>
	inline void reverse(BidItT first, BidItT last)
	{
		return Internal::ReverseImp(first, last, IteratorCategory<BidItT>());
	}

	template<typename BidItT,typename OutputItT>
		inline OutputItT reverse_copy(
			BidItT first,BidItT last,
			OutputItT result)
	{
		for (;--last != first;++result)
			*result = *last;
	}

	namespace Internal
	{
		template<typename ForwardItT>
		inline ForwardItT RotateImp(
			ForwardItT first,ForwardItT middle,
			ForwardItT last,
			forward_iterator_tag)
		{
			auto i = middle;
			while (true)
			{
				Yupei::iter_swap(first, i);
				++first;
				++i;
				if (i == last)
					break;//We need to record the return value.
				if (first == middle)
					middle = i;
			}
			auto ret = first;
			if (first != middle)
			{
				i = middle;
				while (true)
				{
					Yupei::iter_swap(first, i);
					++first;
					++i;
					if (first == middle)
					{
						if (i == last) break;
						middle = i;
					}
					else if (i == last)
						i = middle;
				}
			}

			return ret;
		}

		template<typename BidIt>
		inline BidIt RotateImp(
			BidIt first,BidIt middle,BidIt last,
			bidirectional_iterator_tag)
		{
			Yupei::reverse(first, middle);
			Yupei::reverse(middle, last);
			for (;first != middle && last != middle;++first;--last)
			{
				Yupei::iter_swap(first, --last);
				++first;
			}
			Yupei::reverse(first, last);
			return (middle == first) ? last : first;
		}

		template<typename IntegralT>
		inline IntegralT Gcd(IntegralT a, IntegralT b)
		{
			do
			{
				auto t = a % b;
				a = b;
				b = t;
			} while (b);
			return a;
		}

		template<typename RandomItT>
		inline void RotateCycle(
			RandomItT first, RandomItT last, RandomItT initial, 
			difference_t<RandomItT> shift)
		{
			value_type_t<RandomItT> v(Yupei::move(*initial));
			auto ptr1 = initial;
			auto ptr2 = ptr1 + shift;
			while (ptr2 != initial)
			{
				*ptr1 = Yupei::move(*ptr2);
				ptr1 = ptr2;
				if (last - ptr2 > shift)
					ptr2 += shift;
				else
					ptr2 = first + (shift - (last - ptr2));
			}
			*ptr1 = Yupei::move(v);
		}

		template<typename RandItT>
		inline RandItT RotateImp(
			RandItT first,RandItT middle,RandItT last,
			random_access_iterator_tag)
		{
			auto n = Gcd(last - first, middle - first);
			while (n--)
				RotateCycle(first, last, first + n, middle - first);
			return last - (middle - first);
		}
	}

	template<typename ForwardItT>
	inline ForwardItT rotate(ForwardItT first,ForwardItT middle,ForwardItT last)
	{
		return Internal::RotateImp(first, middle, last, iterator_category_t<ForwardItT>());
	}

	template<typename ForwardItT,typename OutputItT>
	inline OutputItT rotate_copy(
			ForwardItT first,ForwardItT middle,ForwardItT last,
			OutputItT result)
	{
		return Yupei::copy(first, middle, Yupei::copy(middle, last, result));
	}

	//shuffle TODO

	template<typename InputItT,typename PredT>
	bool is_partitioned(
			InputItT first,InputItT last,
			PredT pred)
	{
		for (;first != last;++first)
			if (!pred(*first)) break;
		if (first == last) return true;
		++first;
		for (;first != last;++first)
			if (pred(*first))
				return false;
		return true;
	}

	namespace Internal
	{
		template<typename ForwardItT,typename PredT>
		ForwardItT PartitionImpl(
				ForwardItT first,ForwardItT last,
				PredT pred,
				forward_iterator_tag)
		{

			while (true)
			{
				if (first == last)
					break;
				if (!pred(*first))
					break;
				++first;
			}
			for (auto p = first;++p != last;)
			{
				if (pred(*p)) Yupei::iter_swap(first, p);
				++first;
			}
			return first;
		}


		template<typename BidItT,typename PredT>
		BidItT PartitionImpl(
				BidItT first,BidItT last,
				PredT pred,
				bidirectional_iterator_tag)
		{
			while (true)
			{
				while (true)
				{
					if (first == last)
						break;
					if (!pred(*first))
						break;
					++first;
				}
				do
				{
					if (first == --last)
						return first;
				} while (!pred(*last));
				Yupei::iter_swap(first, last);
				++first;
			}
			return first;
		}

		template<typename RandItT,typename PredT>
		inline RandItT PartitionImpl(
				RandItT first,RandItT last,
				PredT pred,
				random_access_iterator_tag)
		{
			while (true)
			{
				while (pred(*first)) ++first;
				while (!pred(*--last));
				if (first < last) Yupei::iter_swap(first, last);
				else break;
			}
			return first;
		}
	}

	template<typename ForwardItT,typename PredT>
	inline ForwardItT partition(
			ForwardItT first,ForwardItT last,
			PredT pred)
	{
		return Internal::PartitionImpl(first, last, pred, IteratorCategory<ForwardItT>());
	}

	//partition_copy TODO

	template<typename ForwardItT,typename PredT>
	ForwardItT partition_point(ForwardItT first, ForwardItT last, PredT pred)
	{
		auto count = Yupei::distance(first, last);
		while (count > 0)
		{
			auto newCount = count / 2;
			auto mid = first;
			Yupei::advance(mid, newCount);
			if (pred(*mid))
			{
				first = ++mid;
				count -= (newCount + 1);
			}
			else count = newCount;
		}
		return first;
	}

	template<typename BidItT,typename PredT>
	void insertion_sort(BidItT first, BidItT last, PredT pred)
	{
		if (first != last)
		{
			for (auto nextIt = first;++nextIt != last;)
			{
				value_type_t<BidItT> temp(Yupei::move(*nextIt));
				auto nextIt2 = nextIt;
				if (pred(temp, *first))
				{
					Yupei::move_backward(first, nextIt, ++nextIt2);
					*first = Yupei::move(temp);
				}
				else
				{
					for (auto backIt = nextIt2;pred(temp, *--backIt);nextIt2 = backIt)
						*nextIt2 = Yupei::move(*backIt);
					*nextIt2 = Yupei::move(temp);
				}
			}
		}
	}

	template<typename BidItT>
	inline void insertion_sort(BidItT first, BidItT last)
	{
		return Yupei::insertion_sort(first, last, Yupei::less<>());
	}

	namespace Internal
	{
		template<typename RandomItT,typename DistT,typename ObjectT,typename CompT>
		void PushHeapImp(
				RandomItT first,
				DistT holeIndex,
				DistT topIndex,
				ObjectT&& v,
				CompT comp)
		{
			DistT parent = (holeIndex - 1) / 2;
			while (holeIndex > topIndex && comp(first[parent], v))
			{
				first[holeIndex] = Yupei::move(first[parent]);
				holeIndex = parent;
				parent = (holeIndex - 1) / 2;
			}
			first[holeIndex] = Yupei::move(v);
		}
	}

	template<typename RandomItT,typename CompT>
	inline void push_heap(RandomItT first, RandomItT last, CompT comp)
	{
		auto v = Yupei::move(last[-1]);
		return Internal::PushHeapImp(first, last - first - 1, difference_t<RandomItT>(0), Yupei::move(v), comp);
	}

	template<typename RandomItT>
	inline void push_heap(RandomItT first, RandomItT last)
	{
		Yupei::push_heap(first, last, Yupei::less<>());
	}

	namespace Internal
	{
		template<typename RandomItT,typename DistT,typename ObjectT,typename CompT>
		void AdjustHeap(
				RandomItT first,
				DistT holeIndex,
				DistT len,
				ObjectT&& v,
				CompT comp)
		{
			auto topIndex = holeIndex;
			difference_t<RandomItT> childIndex = 2 * holeIndex + 2;
			while (childIndex < len)
			{
				if (comp(first[childIndex], first[childIndex - 1])) --childIndex;
				first[holeIndex] = first[childIndex];
				holeIndex = childIndex;
				childIndex = 2 * childIndex + 2;
			}
			//childIndex is right child *at first*,if childIndex == len,it indicates that left child is the only child.
			if (childIndex == len)
			{
				first[holeIndex] = Yupei::move(first[childIndex - 1]);
				holeIndex = childIndex - 1;
			}
			Internal::PushHeapImp(first, holeIndex, topIndex, Yupei::move(v), comp);

		}

		template<typename RandomItT, typename ObjectT, typename CompareT>
		inline void PopHeapImp(RandomItT first, RandomItT last, RandomItT result, ObjectT&& v, CompareT comp)
		{
			*result = Yupei::move(*first);
			return Internal::AdjustHeap(first, difference_t<RandomItT>(0), last - first - 1, Yupei::move(v), comp);
		}
	}

	template<typename RandomItT,typename CompT>
		inline void pop_heap(RandomItT first, RandomItT last, CompT comp)
	{
		auto val{ Yupei::move(last[-1]) };
		return Internal::PopHeapImp(first, last, last - 1, Yupei::move(val), comp);
	}

	template<typename RandomItT>
	inline void pop_heap(RandomItT first, RandomItT last)
	{
		Yupei::pop_heap(first, last, less<>());
	}

	template<typename RandomItT,typename CompT>
	void make_heap(RandomItT first, RandomItT last, CompT comp)
	{
		if (last - first < 2) return;
		auto len = last - first;
		auto holeIndex = (len - 2) / 2;
		while (true)
		{
			auto temp = Yupei::move(first[holeIndex]);
			Internal::AdjustHeap(first, holeIndex, len, temp, comp);
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

	template<typename RandomItT>
	inline void make_heap(RandomItT first, RandomItT last)
	{
		return Yupei::make_heap(first, last,less<>());
	}


	template<typename RandomItT,typename CompT>
	inline RandomItT is_heap_until(RandomItT first, RandomItT last, CompT comp)
	{
		auto len = last - first;
		//compare each child with its parent
		if (len >= 2)
		{
			for (difference_t<RandomItT> off = 0;++off < len;)
				if (comp(first[(off - 1) / 2], first[off])) return first + off;
		}
		return last;
	}

	template<typename RandomItT>
	inline RandomItT is_heap_until(RandomItT first, RandomItT last)
	{
		return Yupei::is_heap_until(first, last, less<>());
	}

	template<typename RandomItT,typename CompT>
	inline bool is_heap(RandomItT first, RandomItT last, CompT comp)
	{
		return Yupei::is_heap_until(first, last, comp) == last;
	}

	template<typename RandomItT>
	inline bool is_heap(RandomItT first, RandomItT last)
	{
		return Yupei::is_heap(first, last, less<>());
	}

	template<typename RandomItT,typename CompT>
	inline void sort_heap(RandomItT first, RandomItT last, CompT comp)
	{
		YPASSERT(Yupei::is_heap(first, last, comp), "Invalid Heap!");
		while (last - first > 1)
		{
			pop_heap(first, last, comp);
			--last;
		}
	}

	template<typename RandomItT>
	inline void sort_heap(RandomItT first, RandomItT last)
	{
		Yupei::sort_heap(first, last, less<>());
	}

	template<typename InputIt1T,typename InputIt2T,typename OutputItT,typename CompareT>
	OutputItT merge(
			InputIt1T first1,InputIt1T last1,
			InputIt2T first2,InputIt2T last2,
			OutputItT result,
			CompareT compare)
	{
		if (first1 != last1 && first2 != last2)
		{
			while (true)
			{
				if (compare(*first1, *first2))
				{
					*result++ = *first1++;
					if (first1 == last1) break;
				}
				else
				{
					*result++ = *first2++;
					if (first2 == last2) break;
				}
			}
		}
		//copy any tail
		result = Yupei::copy(first1, last1, result);
		return Yupei::copy(first2, last2, result);
	}

	template<typename InputIt1T,typename InputIt2T,typename OutputItT>
		inline OutputItT merge(
			InputIt1T first1,InputIt1T last1,
			InputIt2T first2,InputIt2T last2,
			OutputItT result)
	{
		return Yupei::merge(first1, last1, first2, last2, result, less<>());
	}

	namespace Internal
	{
		constexpr std::size_t kInsertionSortCutOff = 32;

		template<typename BidIt1T,typename BidIt2T,typename BidIt3T,typename CompareT>
		BidIt3T MergeBackward(
				BidIt1T first1,BidIt1T last1,
				BidIt2T first2,BidIt2T last2,
				BidIt3T dest,
				CompareT comp,
				bool isInplace = false)
		{
			if (first1 != last1 && first2 != last2)
			{
				while (true)
				{
					if (comp(*--last2, *--last1))
					{
						*--dest = Yupei::move(*last1);
						++last2;
						if (first1 == last1) break;
					}
					else
					{
						*--dest = Yupei::move(*last2);
						++last1;
						if (first2 == last2) break;
					}
				}
			}

			dest = Yupei::move_backward(first2, last2, dest);
			if (!isInplace)
				dest = Yupei::move_backward(first1, last1, dest);
			return dest;
		}

		template<typename BidIt1T,typename BidIt2T,typename BidIt3T,typename CompareT>
			BidIt3T Merge(
				BidIt1T first1,BidIt1T last1,
				BidIt2T first2,BidIt2T last2,
				BidIt3T dest,
				CompareT comp,
				bool isInplace = false)
		{
			if (first1 != last1 && first2 != last2)
			{
				while (true)
				{
					if (comp(*first1, *first2))
					{
						*dest++ = Yupei::move(*first1++);
						if (first1 == last1) break;
					}
					else
					{
						*dest++ = Yupei::move(*first2++);
						if (first2 == last2) break;
					}
				}
			}

			dest = Yupei::move(first1, last1, dest);
			if (!isInplace)
				dest = Yupei::move(first2, last2, dest);
			return dest;
		}

		template<typename BidirectT,typename OutputItT,typename CompareT>
		void ChunkMerge(
				BidirectT first,BidirectT last,
				OutputItT dest,
				difference_t<BidirectT> chunk,
				difference_t<BidirectT> count,
				CompareT comp)
		{
			//merge two groups ,each group has chunk elements
			for (auto dist = chunk * 2;dist <= count;count -= dist)
			{
				auto mid1 = first;
				Yupei::advance(mid1, chunk);
				auto mid2 = mid1;
				Yupei::advance(mid2, chunk);

				dest = Yupei::merge(first, mid1, mid1, mid2, dest, comp);
				first = mid2;
			}
			if (count <= chunk)
				Yupei::move(first, last, dest);
			else
			{
				auto mid1 = first;
				Yupei::advance(mid1, chunk);

				Yupei::merge(first, mid1, mid1, last, dest, comp);
			}
		}

		template<typename BidItT,typename PredT>
		void BufferedMerge(
				BidItT first,BidItT mid,BidItT last,
				difference_t<BidItT> count1,
				difference_t<BidItT> count2,
				temp_iterator<value_type_t<BidItT>>& buf,
				PredT pred)
		{
			if (count1 == 0 || count2 == 0)
				;//no need to merge
			else if (count1 + count2 == 2)
			{
				if (pred(*mid, *first))
					Yupei::iter_swap(mid, first);
			}
			else if (count1 < count2 && count1 <= buf.size())
			{
				Yupei::move(first, mid, buf.init());
				Internal::Merge(buf.begin(), buf.end(), mid, last, first, pred, true);
			}
			else if (count2 <= buf.size())
			{
				Yupei::move(mid, last, buf.init());
				Internal::MergeBackward(first, mid, buf.begin(), buf.end(), last, pred, true);
			}
		}

		template<typename BidItT,typename CompT>
			void BufferedMergeSort(
				BidItT first, BidItT last, 
				difference_t<BidItT> count,
				temp_iterator<value_type_t<BidItT>>& buf, 
				CompT comp)
		{
			auto mid = first;
			for (auto restCount = count;restCount >= kInsertionSortCutOff;restCount -= kInsertionSortCutOff)
			{
				auto partLast = mid;
				Yupei::advance(partLast, kInsertionSortCutOff);
				Yupei::insertion_sort(mid, partLast, comp);
				mid = partLast;
			}
			Yupei::insertion_sort(mid, last, comp);
			buf.allocate();
			for (std::ptrdiff_t chunk =
				static_cast<std::ptrdiff_t>(kInsertionSortCutOff);chunk < count;chunk *= 2)
			{
				//merged into tempbuf
				ChunkMerge(first, last, buf.init(), chunk, count, comp);
				//merged back
				ChunkMerge(buf.begin(), buf.end(), first, chunk *= 2, count, comp);
			}
		}

		template<typename BidItT,typename PredT>
		inline void StableSortImp(
				BidItT first,BidItT last,
				difference_t<BidItT> count,
				temp_iterator<value_type_t<BidItT>>& buf,
				PredT pred)
		{
			if (count <= kInsertionSortCutOff)
				Yupei::insertion_sort(first, last, pred);
			else
			{
				auto halfCount = (count + 1) / 2;
				auto mid = first;
				Yupei::advance(mid, halfCount);
				Internal::BufferedMergeSort(first, mid, halfCount, buf, pred);
				Internal::BufferedMergeSort(mid, last, count - halfCount, buf, pred);
				Internal::BufferedMerge(first, mid, last, halfCount, count - halfCount, buf, pred);
			}
		}

		template<typename RandomItT,typename PredT>
		inline void StableSortImp(RandomItT start, RandomItT end, PredT pred)
		{
			if (start != end)
			{
				difference_t<RandomItT> dist = Yupei::distance(start, end);
				auto buf = temp_iterator<value_type_t<RandomItT>>{ (dist + 1) / 2 };
				Internal::StableSortImp(start, end, dist, buf, pred);
			}
		}

		template<typename RandomItT,typename PredT>
			RandomItT UnguardedPartition(
				RandomItT first,RandomItT last,
				PredT pred)
		{
			--last;//the last element
			const auto& pivot = *--last;
			auto last2 = last;
			while (true)
			{
				while (pred(*++first, pivot))
					;
				while (pred(pivot, *--last))
					;
				if (!(first < last))
				{
					Yupei::iter_swap(first, last2);
					return first;
				}
				Yupei::iter_swap(first, last);
			}
		}

		template<typename RandomItT,typename PredT>
		void Median3(RandomItT first, RandomItT mid, RandomItT last, PredT pred)
		{
			if (pred(*mid, *first))
				Yupei::iter_swap(first, mid);
			if (pred(*--last, *mid))
			{
				Yupei::iter_swap(mid, last);
				if (pred(*mid, *first))
					Yupei::iter_swap(mid, first);
			}
			Yupei::iter_swap(mid, --last);
		}

		template<typename RandomItT,typename PredT>
		void SortImp(RandomItT first, RandomItT last, difference_t<RandomItT> ideal, PredT pred)
		{
			difference_t<RandomItT> count;
			for (;kInsertionSortCutOff < (count = last - first) && ideal > 0;)
			{
				//partition
				Median3(first, first + (last - first) / 2, last, pred);
				auto mid = Internal::UnguardedPartition(first, last, pred);
				ideal /= 2;
				ideal += ideal / 2;

				if (mid - first < last - mid)
				{
					Internal::SortImp(first, mid, ideal, pred);
					first = mid;
				}
				else
				{
					Internal::SortImp(mid, last, ideal, pred);
					last = mid;
				}
			}

			if (count > kInsertionSortCutOff)
			{
				//ideal == 0,use HeapSort
				Yupei::make_heap(first, last, pred);
				Yupei::sort_heap(first, last, pred);
			}

			else if (count >= 2)
				Yupei::insertion_sort(first, last, pred);
		}

	}

	template<typename RandomItT,typename PredT>
	inline void stable_sort(RandomItT first, RandomItT last, PredT pred)
	{
		return Internal::StableSortImp(first, last, pred);
	}

	template<typename RandomItT>
	inline void stable_sort(RandomItT first, RandomItT last)
	{
		return Yupei::stable_sort(first, last, less<>());
	}

	template<typename RandomItT,typename PredT>
	inline void sort(RandomItT first, RandomItT last, PredT pred)
	{
		return Internal::SortImp(first, last, last - first, pred);
	}

	template<typename RandomItT>
	inline void sort(RandomItT first, RandomItT last)
	{
		return Yupei::sort(first, last, less<>());
	}

	template<typename FwdItT,typename PredT>
	inline FwdItT is_sorted_until(FwdItT first, FwdItT last, PredT pred)
	{
		if (first != last)
			for (FwdItT frontIt = first;++frontIt != last;++first)
				if (pred(*frontIt, *first)) return frontIt;
		return last;
	}

	template<typename FwdItT>
	inline FwdItT is_sorted_until(FwdItT first, FwdItT last)
	{
		return Yupei::is_sorted_until(first, last, less<>());
	}

	template<typename FwdItT,typename PredT>
	inline bool is_sorted(FwdItT first, FwdItT last, PredT pred)
	{
		return Yupei::is_sorted_until(first, last, pred) == last;
	}

	template<typename FwdItT>
	inline bool is_sorted(FwdItT first, FwdItT last)
	{
		return Yupei::is_sorted(first, last, less<>());
	}

	template<typename BidItT,typename PredT>
	BidItT stable_partition(BidItT first, BidItT last, PredT pred)
	{
		auto dist = Yupei::distance(first, last);
		if (dist == 0) return first;
		if (dist == 1) return pred(*first) ? last : first;
		auto buf = temp_iterator<value_type_t<BidItT>>{ dist };
		buf.allocate();
		auto tmp = first;
		for (;first != last;++first)
		{
			if (pred(*first))
				*tmp++ = Yupei::move(*first);
			else
				*buf++ = Yupei::move(*first);
		}
		Yupei::move(buf.begin(), buf.end(), tmp);
		return tmp;
	}

	template<typename BidItT>
	inline BidItT stable_partition(BidItT first, BidItT last)
	{
		return Yupei::stable_partition(first, last, less<>());
	}

	template<typename FwdItT, typename ObjectT, typename PredT>
	FwdItT lower_bound(FwdItT first, FwdItT last, const ObjectT& v, PredT pred)
	{
		auto count = Yupei::distance(first, last);

		while (count > 0)
		{
			auto halfCount = count / 2;
			auto mid = first;
			Yupei::advance(mid, halfCount);
			if (pred(*mid, v))
			{
				first = ++mid;
				count -= (halfCount + 1);
			}
			else count = halfCount;
		}

		return first;
	}

	template<typename FwdItT, typename ObjectT, typename PredT>
	FwdItT upper_bound(FwdItT first, FwdItT last, const ObjectT& v, PredT pred)
	{
		auto count = Yupei::distance(first, last);

		while (count > 0)
		{
			auto halfCount = count / 2;
			auto mid = first;
			Yupei::advance(mid, halfCount);
			if (!pred(*mid, v))
			{
				first = ++mid;
				count -= (halfCount + 1);
			}
			else count = halfCount;
		}

		return first;
	}

	template<typename FwdItT,typename ObjectT,typename PredT>
		pair<FwdItT, FwdItT> equal_range(FwdItT first, FwdItT last, const ObjectT& v, PredT pred)
	{
		auto count = Yupei::distance(first, last);

		while (count > 0)
		{
			auto halfCount = count / 2;
			auto mid = first;
			Yupei::advance(mid, halfCount);

			if (pred(*mid, v))
			{
				first = ++mid;
				count -= (halfCount + 1);
			}
			else if (pred(v, *mid))
				count = halfCount;
			else
			{
				auto first2 = Yupei::lower_bound(first, mid, v, pred);
				Yupei::advance(first, count);
				auto last2 = Yupei::upper_bound(++mid, first, v, pred);
				return Yupei::make_pair(first2, last2);
			}
		}
		return Yupei::make_pair(first, first);
	}

	template<typename FwdItT, typename ObjectT, typename CompareT>
	inline bool binary_search(FwdItT first, FwdItT last, const ObjectT& v, CompareT comp)
	{
		auto it = *Yupei::lower_bound(first, last, v, comp);
		return it != last && !comp(v, *it);
	}

	template<typename FwdItT, typename ObjectT>
	inline FwdItT lower_bound(FwdItT first, FwdItT last, const ObjectT& v)
	{
		return Yupei::lower_bound(first, last, v, less<>());
	}

	template<typename FwdItT, typename ObjectT>
	inline FwdItT upper_bound(FwdItT first, FwdItT last, const ObjectT& v)
	{
		return Yupei::upper_bound(first, last, v, less<>());
	}

	template<typename FwdItT,typename ObjectT>
		inline pair<FwdItT, FwdItT> equal_range(FwdItT first, FwdItT last, const ObjectT& v)
	{
		return Yupei::equal_range(first, last, v, less<>());
	}

	template<typename FwdItT, typename ObjectT>
	inline bool binary_search(FwdItT first, FwdItT last, const ObjectT& v)
	{
		return Yupei::binary_search(first, last, v, less<>());
	}

	template<typename RandItT, typename PredT>
	void nth_element(RandItT first, RandItT nth, RandItT last, PredT pred)
	{
		if (nth == last) return;
		for (;last - first > Internal::kInsertionSortCutOff;)
		{
			Internal::Median3(first, first + (last - first) / 2, last, pred);
			auto mid = Internal::UnguardedPartition(first, last, pred);
			if (mid <= nth)
				first = mid;
			else last = mid;
		}
		Yupei::insertion_sort(first, last, pred);
	}

	template<typename RandItT>
	inline void nth_element(RandItT first, RandItT nth, RandItT last)
	{
		return Yupei::nth_element(first, nth, last, less<>());
	}

	template<typename RandItT, typename CompareT>
	void partial_sort(RandItT first, RandItT middle, RandItT last, CompareT comp)
	{
		Yupei::make_heap(first, middle, comp);
		for (auto it = middle; it < last;++it)
		{
			if (comp(*it, *first))
			{
				auto val{ Yupei::move(*it) };
				Internal::PopHeapImp(first, middle, it, Yupei::move(val), comp);
			}
		}
		Yupei::sort_heap(first, middle, comp);
	}

	template<typename RandItT>
	inline void partial_sort(RandItT first, RandItT middle, RandItT last)
	{
		return Yupei::partial_sort(first, middle, last, less<>());
	}

	template<typename InputItT, typename RandItT, typename CompareT>
	RandItT partial_sort_copy(
		InputItT first, InputItT last,
		RandItT result_first, RandItT result_last, 
		CompareT comp)
	{
		auto mid2 = result_first;
		for (;first != last && mid2 != result_last;++first, (void)++mid2)
			*first = *mid2;
		Yupei::make_heap(result_first, mid2, comp);
		for (;first != last;++first)
		{
			if (comp(*first, *result_first))
				Internal::AdjustHeap(result_first, difference_t<RandItT>(0), mid2 - result_first, value_type_t<RandItT>(*first), comp);
		}
		Yupei::sort_heap(result_first, mid2, comp);
		return mid2;
	}

	template<typename InputItT, typename RandItT>
	inline RandItT partial_sort_copy(
		InputItT first, InputItT last,
		RandItT result_first, RandItT result_last)
	{
		return Yupei::partial_sort_copy(first, last, result_first, result_last, less<>());
	}

	//set operations

	//Returns: true if [first2,last2) is empty or if every element in the range [first2,last2) is contained
	//in the range[first1, last1).Returns false otherwise.
	template<typename InputIt1T, typename InputIt2T, typename CompareT>
	bool includes(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2,
		CompareT comp)
	{
		for (;first1 != last1 && first2 != last2;)
		{
			if (comp(*first2, *first1))
				return false;
			else if (comp(*first1, *first2))
				++first1;
			else
			{
				++first1;
				++first2;
			}
		}
		return first2 == last2;
	}

	template<typename InputIt1T, typename InputIt2T>
	inline bool includes(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2)
	{
		return Yupei::includes(first1, last1, first2, last2, less<>());
	}

	template<typename InputIt1T, typename InputIt2T, typename OutputItT, typename CompareT>
	OutputItT set_union(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2, 
		OutputItT result, 
		CompareT comp)
	{
		while (first1 != last1 && first2 != last2)
		{
			if (comp(*first1, *first2))
			{
				*result = *first1;
				++first1;
			}
			else if (comp(*first2, *first1))
			{
				*result = *first2;
				++first2;
			}
			else
			{
				*result = *first1;
				++first1;
				++first2;
			}
			++result;
		}
		return Yupei::copy(first2, last2, Yupei::copy(first1, last1, result));
	}

	template<typename InputIt1T, typename InputIt2T, typename OutputItT>
	inline OutputItT set_union(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2,
		OutputItT result)
	{
		return Yupei::set_union(first1, last1, first2, last2, result, less<>());
	}

	template<typename InputIt1T, typename InputIt2T, typename OutputItT, typename CompareT>
	OutputItT set_intersection(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2, 
		OutputItT result, 
		CompareT comp)
	{
		for (;first1 != last1 && first2 != last2;)
		{
			if (comp(*first2, *first1))
				++first2;
			else if (comp(*first1, *first2))
				++first1;
			else
			{
				*result++ = *first1;
				++first1;
				++first2;
			}
		}
		return result;
	}

	template<typename InputIt1T, typename InputIt2T, typename OutputItT>
	inline OutputItT set_intersection(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2, 
		OutputItT result)
	{
		return Yupei::set_intersection(first1, last1, first2, last2, result, less<>());
	}


	template<typename InputIt1T, typename InputIt2T, typename OutputItT, typename CompareT>
	OutputItT set_difference(InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2, OutputItT result, CompareT comp)
	{
		for (;first1 != last1 && first2 != last2;)
		{
			if (comp(*first2, *first1))
				++first2;
			else if (comp(*first1, *first2))
			{
				*result = *first1;
				++result;
				++first1;
			}
			else
			{
				++first1;
				++first2;
			}
		}
		return Yupei::copy(first1, last1, result);
	}


	template<typename InputIt1T, typename InputIt2T, typename OutputItT>
	inline OutputItT set_difference(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2,
		OutputItT result)
	{
		return Yupei::set_difference(first1, last1, first2, last2, result, less<>());
	}

	template<typename InputIt1T, typename InputIt2T, typename OutputItT, typename CompareT>
	OutputItT set_symmetric_difference(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2, 
		OutputItT result, 
		CompareT comp)
	{
		for (;first1 != last1 && first2 != last2;)
		{
			if (comp(*first2, *first1))
			{
				*result = *first2;
				++first2;
				++result;
			}
			else if (comp(*first1, *first2))
			{
				*result = *first1;
				++result;
				++first1;
			}
			else
			{
				++first1;
				++first2;
			}
		}
		return Yupei::copy(first2, last2, result, Yupei::copy(first1, last1, result));
	}

	template<typename InputIt1T, typename InputIt2T, typename OutputItT>
	inline OutputItT set_symmetric_difference(
		InputIt1T first1, InputIt2T last1,
		InputIt2T first2, InputIt2T last2,
		OutputItT result)
	{
		return Yupei::set_symmetric_difference(first1, last1, first2, last2, result, less<>());
	}

	template<typename BidirectItT,typename CompareT>
	bool next_permutation(BidirectItT first, BidirectItT last, CompareT comp)
	{
		if (first == last) return false;
		auto it = last;
		if (--it == first) return false;
		while (true)
		{
			auto afterit = it;
			--it;
			if (comp(*it, *afterit))
			{
				auto j = last;
				while (!comp(*it, *--j))
					;
				Yupei::iter_swap(it, j);
				Yupei::reverse(afterit, last);
				return true;
			}
			else if (it == first)
			{
				reverse(first, last);
				return false;
			}
		}
	}

	template<typename BidirectItT>
	inline bool next_permutation(BidirectItT first, BidirectItT last)
	{
		return Yupei::next_permutation(first, last, less<>());
	}

	template<typename BidirectItT, typename CompareT>
	bool prev_permutation(BidirectItT first, BidirectItT last, CompareT comp)
	{
		if (first == last) return false;
		auto it = last;
		if (--it == first) return false;
		while (true)
		{
			auto afterit = it;
			--it;
			if (comp(*afterit,*it))
			{
				auto j = last;
				while (!comp(*--j,*it))
					;
				Yupei::iter_swap(it, j);
				Yupei::reverse(afterit, last);
				return true;
			}
			else if (it == first)
			{
				reverse(first, last);
				return false;
			}
		}
	}

	template<typename BidirectItT>
	inline bool prev_permutation(BidirectItT first, BidirectItT last)
	{
		return Yupei::prev_permutation(first, last, less<>());
	}
}
