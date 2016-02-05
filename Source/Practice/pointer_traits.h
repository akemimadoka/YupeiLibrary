#pragma once

#include "type_traits.h"
#include "__addressof.h"

namespace Yupei
{
	namespace Internal
	{
		template<typename Ptr>
		struct GetPtrFirstType;

		template<template<typename...>class PtrT, typename FirstT,typename... Args>
		struct GetPtrFirstType<PtrT<FirstT,Args...>>{ using type = FirstT; };

		template<typename Ptr,typename U>
		struct GetRebindPtrType;

		template<template<typename...>class PtrT,typename U, typename FirstT, typename... Args>
		struct GetRebindPtrType<PtrT<FirstT, Args...>,U> { using type = PtrT<U,Args...>; };

		template<typename PtrT,typename = void>
		struct GetElementType : GetPtrFirstType<PtrT> {};

		template<typename PtrT>
		struct GetElementType<PtrT, void_t<typename PtrT::element_type>> { using type = typename PtrT::element_type; };

		template<typename PtrT>
		using PtrHasDifferenceType = typename PtrT::difference_type;

		template<typename PtrT,typename U, typename = void>
		struct GetPtrBindType : GetRebindPtrType<PtrT,U> {};

		template<typename PtrT,typename U>
		struct GetPtrBindType<PtrT,U, void_t<typename PtrT::template rebind<U>>> { using type = typename PtrT::template rebind<U>; };

	}
	
	template<class Ptr>
	struct pointer_traits
	{
		using pointer = Ptr;
		using element_type = typename Internal::GetElementType<Ptr>::type;
		using difference_type = deteced_or_t<std::ptrdiff_t, Internal::PtrHasDifferenceType, Ptr>;

		template<class U>
		using rebind = typename Internal::GetRebindPtrType<Ptr, U>;

		static pointer pointer_to(conditional_t<is_void<element_type>::value,void*,element_type&> r)
		{
			return pointer::pointer_to(r);
		}
	};

	template<class T>
	struct pointer_traits<T*>
	{
		using pointer = T*;
		using element_type = T;
		using difference_type = std::ptrdiff_t;

		template<class U>
		using rebind = U*;

		static pointer pointer_to(conditional_t<is_void<element_type>::value,void*,element_type&> r) noexcept
		{
			return Yupei::addressof(r);
		}
	};
}
