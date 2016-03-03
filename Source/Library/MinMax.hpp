#pragma once

#include "TypeTraits.hpp"
#include "Extensions.hpp"

namespace Yupei
{
    namespace Internal
    {
        template<typename T>
        constexpr auto MinForward(T&& t) noexcept 
            -> std::conditional_t<std::is_lvalue_reference<T>::value, T, std::remove_reference_t<T>>
        {
            return t;
        }
    }

    template<typename T>
    constexpr decltype(auto) min(T&& t)
    {
        return Internal::MinForward(exforward(t));
    }

    template<typename T, typename... Args>
    constexpr decltype(auto) min(T&& lhs, Args&&... args)
    {
        auto&& prevMin = Yupei::min(exforward(args)...);
        return exforward(lhs) < prevMin ? Internal::MinForward(exforward(lhs)) : Internal::MinForward(exforward(prevMin));
    }
}
