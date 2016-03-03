#pragma once

#include <type_traits>

namespace Yupei
{
#ifdef __cpp_lib_void_t 
    using std::void_t;
#else
    template<typename... Args>
    struct make_void
    {
        using type = void;
    };

    template<typename... Args>
    using void_t = typename make_void<Args...>::type;
#endif

    template<bool B>
    using bool_constant = std::integral_constant<bool, B>;

    template<std::size_t N>
    using size_constant = std::integral_constant<std::size_t, N>;

    template<typename Type, typename... Args>
    struct is_one_of;

    template<typename Type>
    struct is_one_of<Type> : std::false_type {};

    template<typename Type, typename... Args>
    struct is_one_of<Type, Type, Args...> : std::true_type {};

    template<typename Type, typename P, typename... Args>
    struct is_one_of<Type, P, Args...> : is_one_of<Type, Args...> {};

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
    struct detector<DefaultT, void_t<Op<Args...>>, Op, Args...>
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
    using deteced_or_t = typename detected_or<DefaultT, Op, Args...>::type;

    template<typename Expected, template<typename...>class Op, typename... Args>
    using is_deteced_exact = std::is_same<detected_t<Op, Args...>, Expected>;

    template<template<typename...>class Op, typename... Args>
    constexpr bool is_detected_v = is_detected<Op, Args...>::value;

    template<typename Expected, template<typename...>class Op, typename... Args>
    constexpr bool is_deteced_exact_v = is_deteced_exact<Expected, Op, Args...>::value;

    template<bool...B>
    struct static_and;

    template<bool B, bool... Args>
    struct static_and<B, Args...> : bool_constant<B && static_and<Args...>::value> {};

    template<>
    struct static_and<> : std::true_type {};

    template<typename... B>
    struct conjunction : static_and<B::value...> {};

    template<typename... B>
    constexpr bool conjunction_v = conjunction<B...>::value;
}