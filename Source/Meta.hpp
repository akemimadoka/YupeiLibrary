#pragma once

#include <type_traits>

namespace Yupei
{
	template<typename Type, typename... Args>
	struct is_one_of;

	template<typename Type>
	struct is_one_of<Type> : std::false_type {};

	template<typename Type, typename... Args>
	struct is_one_of<Type, Type, Args...> : std::true_type {};

	template<typename Type, typename P, typename... Args>
	struct is_one_of<Type, P, Args...> : is_one_of<Type, Args...> {};

	template<std::size_t N>
	using size_ = std::integral_constant<std::size_t, N>;

	struct nonesuch
	{
		nonesuch() = delete;
		~nonesuch() = delete;
		nonesuch(const nonesuch&) = delete;
		void operator=(const nonesuch&) = delete;
	};

	template<typename DefaultT, typename, template<typename...>class Op, typename... Args>
	struct detector
	{
		using value_t = std::false_type;
		using type = DefaultT;
	};

	template<typename DefaultT, template<typename...>class Op, typename... Args>
	struct detector<DefaultT, std::void_t<Op<Args...>>, Op, Args...>
	{
		using value_t = std::true_type;
		using type = Op<Args...>;
	};

	template<template<typename...>class Op, typename... Args>
	using is_detected = typename detector<void, void, Op, Args...>::value_t;

	template<template<typename...>class Op, typename... Args>
	using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

	template<typename DefaultT, template<typename...>class Op, typename... Args>
	using detected_or = detector<DefaultT, void, Op, Args...>;

	template<typename DefaultT, template<typename...>class Op, typename... Args>
	using detected_or_t = typename detected_or<DefaultT, Op, Args...>::type;

	template<typename Expected, template<typename...>class Op, typename... Args>
	using is_deteced_exact = std::is_same<detected_t<Op, Args...>, Expected>;

	template<template<typename...>class Op, typename... Args>
	constexpr bool is_detected_v = is_detected<Op, Args...>::value;

	template<typename Expected, template<typename...>class Op, typename... Args>
	constexpr bool is_deteced_exact_v = is_deteced_exact<Expected, Op, Args...>::value;

	template<typename T>
	struct identity
	{
		using type = T;
	};

	template<typename T>
	using unwrap_t = typename T::type;

	//MSVC ÓÐ bug¡­¡­
	template<template<typename... Args> class Func, typename... Params>
	struct lazy
	{
		using type = Func<unwrap_t<Params>...>;
	};

	template<bool B, typename LazyFunc>
	struct lazy_and;

	template<typename LazyFunc>
	struct lazy_and<true, LazyFunc> : unwrap_t<LazyFunc> {};

	template<typename LazyFunc>
	struct lazy_and<false, LazyFunc> : std::false_type {};
}