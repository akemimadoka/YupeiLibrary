#pragma once

#include "TypeTraits.hpp"
#include "__addressof.h"
#include "basic.h"
#include "iterator.h"
#include "..\Custom\Extensions.h"

namespace Yupei
{
    class hash_code;

    auto hash_combine_range(hash_code h,const unsigned char* start, const unsigned char* hashEnd) noexcept -> hash_code;

    class hash_code
    {
    public:
        DISABLECOPY(hash_code)
               
        hash_code(hash_code&& other) noexcept = default;
        hash_code& operator = (hash_code&& other) noexcept = default;
        
    private:
        friend auto hash_combine_range(hash_code h, const unsigned char* start, const unsigned char* hashEnd) noexcept -> hash_code;

        bool isInited = false;
        uint32 u32Reg_;
    };

    template<typename T, typename Enable = void>
    struct is_uniquely_represented : false_type {};

    template<>
    struct is_uniquely_represented<unsigned char> : true_type {};

    template<>
    struct is_uniquely_represented<bool> : false_type {};

    template<typename T>
    struct is_uniquely_represented<T, enable_if_t<is_integral<T>::value || is_pointer<T>::value || is_array<T>::value >>
        : true_type {};

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
        inline auto HashCombineRangeImp(hash_code code, InputIterator first, InputIterator last, true_type) noexcept ->hash_code
        {
            const auto hashStart = reinterpret_cast<const unsigned char*>(Yupei::pointer_from(first));
            const auto hashLast = reinterpret_cast<const unsigned char*>(Yupei::pointer_from(last));
            return hash_combine_range(Yupei::move(code), hashStart, hashLast);
        }

        template <typename InputIterator>
        inline auto HashCombineRangeImp(hash_code code, InputIterator first, InputIterator last, false_type) noexcept ->hash_code
        {
            for (;first != last; ++first)
                code = hash_value(Yupei::move(code), *first);
            return code;
        }

        template<typename HashCode, typename T>
        inline auto HashBytes(HashCode code, const T& v) -> HashCode
        {            
            const auto hashStart = reinterpret_cast<const unsigned char*>(Yupei::addressof(v));
            const auto hashEnd = hashStart + sizeof(v);
            return hash_combine_range(Yupei::move(code), hashStart, hashEnd);
        }
    }

    inline auto hash_combine_range(hash_code h, const unsigned char* hashStart, const unsigned char* hashEnd) noexcept -> hash_code
    {
        if (!h.isInited)
        {
            h.isInited = true;
            h.u32Reg_ = 2166136261u;
        }

        const auto DoFnv32Byte = [&](unsigned char byteData)
        {
            h.u32Reg_ ^= byteData;
            h.u32Reg_ *= 16777619u;
        };

        const auto hashSize = static_cast<std::size_t>(hashEnd - hashStart);
        constexpr auto wordSize = sizeof(std::uintptr_t);

        if (hashSize >= wordSize * 2)
        {
            while ((reinterpret_cast<std::uintptr_t>(hashStart) & wordSize) != 0)
            {
                DoFnv32Byte(*hashStart);
                ++hashStart;
            }
            auto i = static_cast<std::size_t>(hashEnd - hashStart) / wordSize;

            while (i)
            {
                auto readWord = *reinterpret_cast<const std::uintptr_t*>(hashStart);
                hashStart += wordSize;
                for (std::size_t j = 0; j < wordSize; ++j)
                {
                    DoFnv32Byte(readWord & 0xFFu);
                    readWord >>= 8;
                }
                --i;
            }

            while (hashStart != hashEnd)
            {
                DoFnv32Byte(*hashStart);
                ++hashStart;
            }
        }

        return h;
    }

    template <typename InputIterator>
    inline auto hash_combine_range(hash_code code, InputIterator first, InputIterator last) -> hash_code
    {
        return Internal::HashCombineRangeImp(Yupei::move(code), first, last, Internal::CouldBeHashedAsBytes<InputIterator>{});
    }

    template<typename HashCode, typename T, typename = enable_if_t<is_integral<T>::value || is_enum<T>::value>>
    inline auto hash_value(HashCode code, T value) -> hash_code
    {
        return Internal::HashBytes(Yupei::move(code), value);
    }

    template<typename HashCode, typename T, typename = enable_if_t<is_floating_point<T>::value>>
    inline auto hash_value(HashCode code, T value) -> hash_code
    {       
        return Internal::HashBytes(Yupei::move(code), value == 0.0 ? 0.0 : value);
    }

    template<typename HashCode, typename T, typename = enable_if_t<is_pointer<T>::value>>
    inline auto hash_value(HashCode code, T value) -> hash_code
    {
        return Internal::HashBytes(Yupei::move(code), value);
    }

    template<typename HashCode>
    inline auto hash_value(HashCode code, std::nullptr_t) -> hash_code
    {
        return Internal::HashBytes(Yupei::move(code), 0);
    }

    template <typename HashCode, typename T, typename... Ts>
    inline auto hash_combine(HashCode code,const T& v, const Ts&... ts) noexcept -> hash_code
    {
        return hash_combine(hash_value(Yupei::move(code), v), ts...);
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
        decltype(auto) operator()(const T& value) const
        {
            HashCode hashCode;
            return hash_value(Yupei::move(hashCode), value);
        }
    };

}