#pragma once

#include "..\TypeTraits.hpp"
#include "..\Extensions.hpp"
#include "..\Iterator.hpp"
#include "fnv32.hpp"
#include "fnv64.hpp"
#include <utility>
#include <cstdint>

namespace Yupei
{   
    using hash_code = std::conditional_t<sizeof(std::uintptr_t) == 32, fnv32, fnv64>;
       
    template<typename T, typename Enable = void>
    struct is_uniquely_represented : std::false_type {};

    template<>
    struct is_uniquely_represented<unsigned char> : std::true_type {};

    template<>
    struct is_uniquely_represented<bool> : std::false_type {};

    template<typename T>
    struct is_uniquely_represented<T, std::enable_if_t<std::is_integral<T>::value || std::is_pointer<T>::value || std::is_array<T>::value>>
        : std::true_type {};

    namespace Internal
    {
        template<typename... Ts>
        struct all_uniquely_represented;

        template<typename T, typename... Ts>
        struct all_uniquely_represented<T, Ts...>
            : bool_constant<is_uniquely_represented<T>::value &&
            all_uniquely_represented<Ts...>::value> {};
    }
  
    namespace Internal
    {
        template<typename InputItT>
        struct CouldBeHashedAsBytes : bool_constant<is_contiguous_iterator<InputItT>::value && is_uniquely_represented<value_type_t<InputItT>>::value> {};
      
        template <typename InputIterator>
        inline auto HashCombineRangeImp(hash_code code, InputIterator first, InputIterator last, std::true_type) noexcept -> hash_code
        {
            const auto hashStart = reinterpret_cast<const byte*>(Yupei::pointer_from(first));
            const auto hashLast = reinterpret_cast<const byte*>(Yupei::pointer_from(last));
            return hash_combine_range(std::move(code), hashStart, hashLast);
        }

        template <typename InputIterator>
        inline auto HashCombineRangeImp(hash_code code, InputIterator first, InputIterator last, std::false_type) noexcept -> hash_code
        {
            for (;first != last; ++first)
                code = hash_value(std::move(code), *first);
            return code;
        }

        template<typename HashCode, typename T>
        inline auto HashBytes(HashCode code, const T& v) -> HashCode
        {            
            const auto hashStart = reinterpret_cast<const unsigned char*>(std::addressof(v));
            const auto hashEnd = hashStart + sizeof(v);
            return hash_combine_range(std::move(code), hashStart, hashEnd);
        }
    }

    template <typename InputIterator>
    inline auto hash_combine_range(hash_code code, InputIterator first, InputIterator last) -> hash_code
    {
        return Internal::HashCombineRangeImp(std::move(code), first, last, Internal::CouldBeHashedAsBytes<InputIterator>{});
    }

    template<typename HashCode, typename T, std::enable_if_t<std::is_integral<T>::value || std::is_enum<T>::value, int> = 0>
    inline auto hash_value(HashCode code, T value) -> hash_code
    {
        return Internal::HashBytes(std::move(code), value);
    }

    template<typename HashCode, typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
    inline auto hash_value(HashCode code, T value) -> hash_code
    {       
        return Internal::HashBytes(std::move(code), value == 0.0 ? 0.0 : value);
    }

    template<typename HashCode, typename T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
    inline auto hash_value(HashCode code, T value) -> hash_code
    {
        return Internal::HashBytes(std::move(code), value);
    }

    template<typename HashCode>
    inline auto hash_value(HashCode code, std::nullptr_t) -> hash_code
    {
        return Internal::HashBytes(std::move(code), 0);
    }

    template <typename HashCode, typename T, typename... Ts>
    inline auto hash_combine(HashCode code,const T& v, const Ts&... ts) noexcept -> hash_code
    {
        return hash_combine(hash_value(std::move(code), v), ts...);
    }

    template<typename HashCode>
    inline auto hash_combine(HashCode code) noexcept -> hash_code
    {
        return code;
    }

    template<typename HashCode = hash_code>
    struct hash
    {
        template<typename T>
        std::size_t operator()(const T& value) const
        {
            HashCode hashCode;
            return hash_value(std::move(hashCode), value).finalize();
        }
    };

}