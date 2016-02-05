#pragma once

#include "basic.h"
#include "type_traits.h"
#include "utility_internal.h"
#include "container_traits.h"

namespace Yupei
{
	struct allocator_arg_t {};
	constexpr allocator_arg_t allocator_arg{};

	//Remarks:
	//
	//automatically detects whether T has a nested allocator_type that is convertible from Alloc.
	//Meets the BinaryTypeTrait requirements(C++14 ′20.10.1).The implementation shall 
	//provide a definition that is derived from true_type if a type 
	//T::allocator_type exists and either is_convertible_v<Alloc, T::allocator_type> != false or 
	//T::allocator_type is an alias for std::experimental::erased_type(3.1.2), 
	//otherwise it shall be derived from false_type.A program may specialize this template to 
	//derive from true_type for a user - defined type T that does not have a nested allocator_type 
	//but nonetheless can be constructed with an allocator where either :
	//！the first argument of a constructor has type allocator_arg_t and the second_obj argument has type Alloc or
	//！the last argument of a constructor has type Alloc.
	//


	namespace Internal
	{
		template<typename Type,
			typename Alloc,typename = void>
		struct UsesAllocatorHelper : false_type
		{

		};

		template<typename Type,
			typename Alloc
		>
		struct UsesAllocatorHelper<Type,Alloc,
			Yupei::void_t<container_allocator_type<Type>>>
			: bool_constant<
			is_convertible<container_allocator_type<Type>,Alloc>::value ||
			is_same<container_allocator_type<Type>,Experimental::erased_type>::value>
		{

		};
	}

	template<typename Type,
		typename Alloc>
	struct uses_allocator : Internal::UsesAllocatorHelper<Type, Alloc>
	{

	};

	/*(1.1) ！ if uses_allocator<T, Alloc>::value is false and is_constructible<T, V1, V2, ..., VN>::value
		is true, then obj is initialized as obj(v1, v2, ..., vN);
	(1.2) ！ otherwise, if uses_allocator<T, Alloc>::value is true and is_constructible<T, allocator_ -
		arg_t, Alloc, V1, V2, ..., VN>::value is true, then obj is initialized as obj(allocator_arg,
			alloc, v1, v2, ..., vN);
	(1.3) ！ otherwise, if uses_allocator<T, Alloc>::value is true and is_constructible<T, V1, V2, ...,
		VN, Alloc>::value is true, then obj is initialized as obj(v1, v2, ..., vN, alloc);
	(1.4) ！ otherwise, the request for uses - allocator construction is ill - formed.[Note:An error will result if
		uses_allocator<T, Alloc>::value is true but the specific constructor does not take an allocator.
		This definition prevents a silent failure to pass the allocator to an element.！end note]*/


	template<typename Type,typename Alloc,typename... Args>
	struct can_be_constructed_by_allocator
	{
		static constexpr bool is_uses_allocator = uses_allocator<Type, Alloc>::value;
		static constexpr bool is_con1 = is_constructible<Type, allocator_arg_t, Args...>::value;
		static constexpr bool is_con2 = is_constructible<Type, Args..., Alloc>::value;
		static_assert(!is_uses_allocator ||
			(is_uses_allocator && (is_con1 || is_con2)), "prevents a silent failure to pass the allocator to an element.");
		using type = integral_constant< int,(is_uses_allocator ? (is_con1 ? 1 : 2) : 0)>;
	};

	template<typename Type, typename Alloc, typename... Args>
	using can_be_constructed_by_allocator_t = typename can_be_constructed_by_allocator<Type, Alloc, Args...>::type;
}
