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
    using hash_code = std::conditional_t<sizeof(std::uintptr_t) == 4, fnv32, fnv64>;

    extern void hash_combine_range(hash_code& h, const byte* start, const byte* last) noexcept;

    template<typename T, typename Enable = void>
    struct is_uniquely_represented : std::false_type {};

    template<>
    struct is_uniquely_represented<unsigned char> : std::true_type {};

    template<>
    struct is_uniquely_represented<bool> : std::false_type {};

    template<typename T>
    struct is_uniquely_represented<T, std::enable_if_t<disjunction<std::is_integral<T>, std::is_pointer<T>, std::is_array<T>>::value>>
        : std::true_type {};

    namespace Internal
    {
        template<typename... Ts>
        struct all_uniquely_represented : conjunction<is_uniquely_represented<Ts>...> {};
    }

    namespace Internal
    {
        template<typename InputItT>
        struct CouldBeHashedAsBytes : bool_constant<is_contiguous_iterator<InputItT>::value && is_uniquely_represented<value_type_t<InputItT>>::value> {};

        template <typename InputIterator>
        inline void HashCombineRangeImp(hash_code& code, InputIterator start, InputIterator last, std::true_type) noexcept
        {
            const auto hashStart = reinterpret_cast<const byte*>(Yupei::pointer_from(start));
            const auto hashLast = reinterpret_cast<const byte*>(Yupei::pointer_from(last));
            hash_combine_range(code, hashStart, hashLast);
        }

        template <typename InputIterator>
        inline void HashCombineRangeImp(hash_code& hashCode, InputIterator start, InputIterator last, std::false_type) noexcept
        {
            std::for_each(start, last, [](const auto& v)
            {
                hash_combine(hashCode, v);
            });
        }

        template<typename HashCode, typename T>
        inline void HashBytes(HashCode& hashCode, const T& v) noexcept
        {
            const auto hashStart = reinterpret_cast<const byte*>(std::addressof(v));
            const auto hashLast = hashStart + sizeof(v);
            hash_combine_range(hashCode, hashStart, hashLast);
        }
    }

    template <typename InputIterator>
    inline void hash_combine_range(hash_code& hashCode, InputIterator start, InputIterator last)
    {
        Internal::HashCombineRangeImp(hashCode, start, last, Internal::CouldBeHashedAsBytes<InputIterator>{});
    }

    template<typename HashCode, typename T, std::enable_if_t<std::is_integral<T>::value || std::is_enum<T>::value, int> = 0>
    inline void hash_value(HashCode& hashCode, T value)
    {
        Internal::HashBytes(hashCode, value);
    }

    template<typename HashCode, typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
    inline void hash_value(HashCode& hashCode, T value)
    {
        return Internal::HashBytes(hashCode, value == 0.0 ? 0.0 : value);
    }

    template<typename HashCode, typename T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
    inline void hash_value(HashCode& hashCode, T value)
    {
        Internal::HashBytes(hashCode, value);
    }

    template<typename HashCode>
    inline void hash_value(HashCode& hashCode, std::nullptr_t)
    {
        Internal::HashBytes(hashCode, 0);
    }

    template <typename HashCode, typename T, typename... Ts>
    inline void hash_combine(HashCode& hashCode, const T& v, const Ts&... ts) noexcept
    {
        hash_value(hashCode, v);
        hash_combine(hashCode, ts...);
    }

    template<typename HashCode>
    inline void hash_combine(HashCode&) noexcept
    {}

    template<typename HashCode = hash_code>
    struct hash
    {
        template<typename T>
        std::size_t operator()(const T& value) const
        {
            HashCode hashCode;
            hash_combine(hashCode, value);
            return hashCode.finalize();
        }
    };

}