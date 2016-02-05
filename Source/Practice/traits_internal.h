#pragma once

#include "basic.h"
#include "traits_constant.h"

namespace Yupei
{
	
	template<bool TestVal,
		typename Type = void>
	struct enable_if
	{

	};

	template<typename Type>
	struct enable_if <true, Type>
	{
		using type = Type;
	};

	template<bool B, typename Type = void>
	using enable_if_t = typename enable_if<B, Type>::type;

	template<bool TestVal,
		typename Type1,
		typename Type2>
	struct conditional
	{
		using type = Type2;
	};

	template<typename Type1,
		typename Type2>
	struct conditional<true,Type1,Type2>
	{
		using type = Type1;
	};

	template<bool TestVal,
		typename Type1,
		typename Type2>
		using conditional_t = typename conditional<TestVal, Type1, Type2>::type;

	template<typename Type1,
		typename Type2>
	struct is_same : public false_type
	{

	};

	template<typename Type>
	struct is_same<Type, Type> : public true_type
	{

	};

#define TOGETHER(A,B) A##B

#define TOGETHERT(A,B,C) A##B##C

#define REMOVE_TOGETHER(CV) TOGETHER(remove_,CV)

#define REMOVE_HELPER(CV)			\
	template<typename Type>			\
	struct REMOVE_TOGETHER(CV)		\
	{								\
		using type = Type;			\
	};								\
									\
	template<typename Type>			\
	struct REMOVE_TOGETHER(CV)<CV Type>\
	{								\
		using type = Type;			\
	};								\
									\
	template<typename Type>			\
	struct REMOVE_TOGETHER(CV)<CV Type[]>\
	{\
		using type = Type[];		\
	};\
	\
	template<typename Type,size_t Index>	\
	struct REMOVE_TOGETHER(CV)<CV Type[Index]>\
	{								\
		using type = Type[Index];	\
	};\
	template<typename Type>\
	using TOGETHERT(remove_,CV,_t) = typename REMOVE_TOGETHER(CV)<Type>::type;


	REMOVE_HELPER(const)
	REMOVE_HELPER(volatile)


	template<typename Type>
	struct remove_cv
	{
		using type = typename remove_const<typename
			remove_volatile<Type>::type>::type;
	};

	template<typename Type>
	using remove_cv_t = typename remove_cv<Type>::type;

	template<typename T>
	struct remove_reference
	{
		using type = T;
	};

	template<typename T>
	struct remove_reference<T&>
	{
		using type = T;
	};

	template<typename T>
	struct remove_reference<T&&>
	{
		using type = T;
	};

	template<typename T>
	using remove_reference_t = typename remove_reference<T>::type;

	template<typename Type>
	struct add_const
	{
		using type = const Type;
	};

	template<typename Type>
	using add_const_t = typename add_const<Type>::type;

	template<typename Type>
	struct add_volatile
	{
		using type = volatile Type;
	};

	template<typename Type>
	using add_volatile_t = typename add_volatile<Type>::type;

	template<typename Type>
	struct add_cv
	{
		using type = const volatile Type;
	};

	template<typename Type>
	using add_cv_t = typename add_cv<Type>::type;

	template<typename Type>
	struct add_rvalue_reference
	{
		using type = Type&&;
	};

	template<typename Type>
	using add_rvalue_reference_t = typename add_rvalue_reference<Type>::type;

	template<typename Type>
	struct add_lvalue_reference
	{
		using type = Type&;
	};

	template<typename Type>
	using add_lvalue_reference_t = typename add_lvalue_reference<Type>::type;

    template<typename Type, typename... Args>
    struct is_one_of;

    template<typename Type>
    struct is_one_of<Type> : std::false_type {};

    template<typename Type, typename... Args>
    struct is_one_of<Type, Type, Args...> : std::true_type {};

    template<typename Type, typename P, typename... Args>
    struct is_one_of<Type, P, Args...> : is_one_of<Type, Args...> {};
	
	template<typename Type>
	using is_integral = is_one_of<remove_cv_t<Type>,
		bool,
		char,
		signed char,
		unsigned char,
		signed short,
		unsigned short,
		signed int,
		unsigned int,
		signed long,
		unsigned long,
		long long,
		unsigned long long>;

	template<typename Type>
	using is_floating_point = is_one_of<
		remove_cv_t<Type>,
		float,
		double,
		long double>;


	template<typename Type>
	struct is_arithmetic :
		bool_constant<is_integral<Type>::value || is_floating_point<Type>::value>
	{

	};



	template<typename T,
	typename U>
	using is_base_of = bool_constant<
		__is_base_of(T,U)
	>;


};
