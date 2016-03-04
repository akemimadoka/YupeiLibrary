#pragma once

#include "Assert.hpp"
#include "MinMax.hpp"
#include "Searchers.hpp"
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace Yupei
{
    enum class string_type
    {
        narrow,
        wide,
        utf8,
        utf32,
        ansi
    };

    template<typename CharT>
    class basic_string_view
    {
    public:
        using value_type = CharT;
        using size_type = std::size_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = CharT&;
        using const_reference = const CharT&;
        using iterator = const_pointer;
        using const_iterator = iterator;

        static constexpr size_type npos = static_cast<size_type>(-1);

        constexpr basic_string_view(const CharT* str, size_type len) noexcept
            :data_{str}, size_{len}
        {}

        constexpr basic_string_view(const CharT* str) noexcept
            :basic_string_view{str, std::strlen(str)}
        {}

        constexpr basic_string_view() noexcept
            :basic_string_view{{}, {}}
        {}

        constexpr basic_string_view(const basic_string_view&) noexcept = default;
        constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

        constexpr const_iterator begin() const noexcept
        {
            return data_;
        }

        constexpr const_iterator end() const noexcept
        {
            return data_ + size_;
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return begin();
        }

        constexpr const_iterator cend() const noexcept
        {
            return end();
        }

        constexpr size_type size() const noexcept
        {
            return size_;
        }

        constexpr bool empty() const noexcept
        {
            return size() == 0;
        }

        constexpr explicit operator bool() const noexcept
        {
            return size() != 0;
        }

        constexpr const_reference operator[](size_type pos) const
        {
            return data_[pos];
        }

        constexpr const_reference at(size_type pos) const
        {
            if (pos >= size()) throw std::out_of_range {"pos >= size()!"};
            return data_[pos];
        }

        constexpr const_pointer data() const noexcept
        {
            return data_;
        }

        constexpr void clear() noexcept
        {
            *this = basic_string_view {};
        }

        constexpr void remove_prefix(size_type n)
        {
            assert(n <= size());
            data_ += n;
            size_ -= n;
        }

        constexpr void remove_suffix(size_type n)
        {
            assert(n <= size());
            size_ -= n;
        }

        size_type copy(CharT* s, size_type n, size_type pos = {}) const
        {
            const auto sizeToCopy = min(size() - pos, n);
            (void)std::copy_n(data(), static_cast<std::ptrdiff_t>(sizeToCopy), s);
            return sizeToCopy;
        }

        //constexpr ?
        void swap(basic_string_view& s) noexcept
        {
            using std::swap;
            swap(data_, s.data_);
            swap(size_, s.size_);
        }

        constexpr basic_string_view substr(size_type pos = {}, size_type n = npos) noexcept
        {
            if (pos > size()) throw std::out_of_range("pos > size()!");
            return {data_ + pos, min(n, size_ - pos};
        }

        constexpr int compare(basic_string_view str) const noexcept
        {
            const auto c = InternalCompare(size(), str.size());
            if (c != 0)
            {
                for (auto cur1 = begin(), cur2 = str.begin(); cur1 != end() && cur2 != end(); ++cur1, ++cur2)
                {
                    const auto c1 = *cur1;
                    const auto c2 = *cur2;
                    if (c1 != c2)
                        return InternalCompare(c1, c2);
                }
            }
            else
                return c;
            return 0;
        }

        //sorry, not constexpr.
        size_type find(const basic_string_view& pattern, size_type pos = {}) const
        {
            const auto it = make_boyer_moore_searcher(pattern.begin(), pattern.end())(begin() + pos, end());
            if (it == end()) return npos;
            return static_cast<size_type>(it - begin());
        }

        size_type find(const CharT* s, size_type pos) const
        {
            return find(basic_string_view(s), pos);
        }

        size_type find(const CharT* s, size_type pos, size_type n) const
        {
            return find(basic_string_view(s, n), pos);
        }

        size_type find(CharT c, size_type pos) const noexcept
        {
            const auto searchStart = begin() + pos;
            const auto it = std::find(searchStart, end(), c);
            if (it == end()) return npos;
            return static_cast<size_type>(it - begin() - pos);
        }

        constexpr size_type find_first_of(const basic_string_view& v, size_type pos = {}) const noexcept
        {
            size_type res {};
            for (const auto& c : *this)
            {
                ++res;
                for (const auto& p : v)
                    if (p == c)
                        return res;
            }
        }

        constexpr size_type find_first_of(const CharT* s, size_type pos) const noexcept
        {
            return find_first_of(basic_string_view(s), pos);
        }

        constexpr size_type find_first_of(const CharT* s, size_type pos, size_type n) const noexcept
        {
            return find_first_of(basic_string_view(s, n), pos);
        }

    private:
        const_pointer data_;
        size_type size_;

        template<typename T, typename U>
        constexpr int InternalCompare(T&& lhs, U&& rhs)
        {
            auto&& bigger = Yupei::max(exforward(lhs), exforward(rhs));
            if (exforward(bigger) == exforward(lhs)) return 1;
            else if (exforward(bigger) == exforward(rhs)) return -1;
            return 0;
        }
    };
}