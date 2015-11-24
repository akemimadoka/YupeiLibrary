#pragma once


#include "compare_funtor.h"
#include "iterator.h"
#include "pointer_traits.h"
#include "SmartPointers\unique_ptr.h"
#include "SmartPointers\shared_ptr.h"

#include <new>
#include <memory>


namespace Yupei
{
	
	using std::allocator;
	using std::align;

	using std::raw_storage_iterator;

	template <class InputItT, class ForwardItT>
	ForwardItT uninitialized_copy(InputItT first, 
		InputItT last,
		ForwardItT result)
	{
		for (; first != last; ++result, ++first)
			::new (static_cast<void*>(Yupei::addressof(*result)))
			value_type_t<ForwardItT>(*first);
		return result;
	}

	template <class InputItT, class SizeT, class ForwardItT>
	ForwardItT uninitialized_copy_n(InputItT first, SizeT n,
		ForwardItT result)
	{
		for (; n > 0; ++result, ++first, --n) {
			::new (static_cast<void*>(Yupei::addressof(*result)))
				value_type_t<ForwardItT>(*first);
		}
		return result;
	}

	template <class ForwardItT, class T>
	void uninitialized_fill(ForwardItT first, ForwardItT last,
		const T& x)
	{
		for (; first != last; ++first)
			::new (static_cast<void*>(Yupei::addressof(*first)))
			value_type_t<ForwardItT>(x);
	}

	template <class ForwardItT, class Size, class T>
	ForwardItT uninitialized_fill_n(ForwardItT first, Size n, const T& x)
	{
		for (; n--; ++first)
			::new (static_cast<void*>(Yupei::addressof(*first)))
			value_type_t<ForwardItT>(x);
		return first;
	}

	

	
}

