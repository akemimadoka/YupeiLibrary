#pragma once

#include "Config.hpp"
#include "HelperMacros.hpp"
#include "TypeTraits.hpp"
#include "Extensions.hpp"
#include <cstddef>
#include <cassert>

namespace Yupei
{
    constexpr std::ptrdiff_t dynamic_extent = -1;

    template<typename T, std::ptrdiff_t Extent = dynamic_extent>
    class span
    {
    public:
        using index_type = std::ptrdiff_t;
        CONTAINER_DEFINE(T)
        using iterator = pointer;
        using const_iterator = const_pointer;

        static constexpr index_type extent = Extent;

        template<std::ptrdiff_t E = extent, std::enable_if_t<E == dynamic_extent || E == 0, int> = 0>
        CXX14_CONSTEXPR span() noexcept
            :data_ {},
            size_ {}
        {}

        CXX14_CONSTEXPR span(pointer ptr, index_type count) noexcept
            :data_{ptr},
            size_{count}
        {
            assert((ptr == nullptr && count == 0) || (extent == dynamic_extent && count >= 0) || count == extent);
        }

        CXX14_CONSTEXPR span(pointer first, pointer last) noexcept
            :span{first, last - first}
        {}

        template<std::size_t N, index_type E = extent>
        CXX14_CONSTEXPR span(value_type(&arr)[N]) noexcept
            : span { static_cast<pointer>(arr), static_cast<index_type>(N) }
        {
            static_assert(E == dynamic_extent || E == N, "arr's size is invalid.");
        }

        DEFAULTCOPY(span)
        DEFAULTMOVE(span)

        template<std::ptrdiff_t Count = dynamic_extent, std::enable_if_t<Count != dynamic_extent, int> = 0>
        CXX14_CONSTEXPR span<value_type, Count> subspan(std::ptrdiff_t offset) noexcept
        {
            assert(offset + Count <= static_cast<std::ptrdiff_t>(size()));
            return { data() + offset, Count };
        }

        template<std::ptrdiff_t Count = dynamic_extent, std::enable_if_t<Count == dynamic_extent, int> = 0>
        CXX14_CONSTEXPR span<value_type, Count> subspan(std::ptrdiff_t offset, std::ptrdiff_t count) noexcept
        {
            assert(offset + count <= static_cast<std::ptrdiff_t>(size()));
            return { data() + offset, Count };
        }

        size_type size() const noexcept
        {
            return size_;
        }

        bool empty() const noexcept
        {
            return size() == 0;
        }

        constexpr pointer data() const noexcept
        {
            return data_;
        }

        constexpr reference operator[](index_type idx) const noexcept
        {
            return data()[idx];
        }

        iterator begin() noexcept
        {
            return data();
        }

        const_iterator begin() const noexcept
        {
            return data();
        }

        iterator end() noexcept
        {
            return data() + size();
        }

        const_iterator end() const noexcept
        {
            return data() + size();
        }

    private:
        pointer data_;
        index_type size_;
    };

    using std::begin;
    using std::end;
    using std::cbegin;
    using std::cend;
    using std::rbegin;
    using std::rend;
    using std::size;
    using std::data;
    using std::empty;

    template<typename T, std::ptrdiff_t Extent>
    CXX14_CONSTEXPR bool operator==(const span<T, Extent>& lhs, const span<T, Extent>& rhs) noexcept
    {
        return std::equal(cbegin(lhs), cend(lhs), cbegin(rhs), cend(rhs));
    }

    template<typename T, std::ptrdiff_t Extent>
    constexpr bool operator!=(const span<T, Extent>& lhs, const span<T, Extent>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template<typename T, std::ptrdiff_t Extent>
    CXX14_CONSTEXPR bool operator<(const span<T, Extent>& lhs, const span<T, Extent>& rhs) noexcept
    {
        return std::lexicographical_compare(cbegin(lhs), cend(lhs), cbegin(rhs), cend(rhs));
    }

    template<typename T, std::ptrdiff_t Extent>
    CXX14_CONSTEXPR bool operator>(const span<T, Extent>& lhs, const span<T, Extent>& rhs) noexcept
    {
        return rhs < lhs;
    }

    template<typename T, std::ptrdiff_t Extent>
    CXX14_CONSTEXPR bool operator<=(const span<T, Extent>& lhs, const span<T, Extent>& rhs) noexcept
    {
        return !(rhs < lhs);
    }

    template<typename T, std::ptrdiff_t Extent>
    CXX14_CONSTEXPR bool operator>=(const span<T, Extent>& lhs, const span<T, Extent>& rhs) noexcept
    {
        return !(lhs < rhs);
    }

    template<typename T, std::size_t N>
    constexpr span<T, N> as_span(T(&arr)[N]) noexcept
    {
        return { arr };
    }

    template<typename T, std::ptrdiff_t Extent = dynamic_extent>
    constexpr span<T, Extent> as_span(T* first, T* last) noexcept
    {
        return { first, last };
    }

    template<typename T, std::ptrdiff_t Extent = dynamic_extent>
    using cspan = span<const T, Extent>;
}