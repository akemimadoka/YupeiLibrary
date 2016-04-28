#pragma once

#include "Assert.hpp"
#include "MinMax.hpp"
#include "Searchers.hpp"
#include "CharTraitsHelpers.hpp"
#include "Config.hpp"
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Yupei
{
    enum class string_type
    {
        utf8,
        utf16,
        utf32,
        wide
    };

    namespace Internal
    {
        template<string_type StringT>
        struct CharTTraits;

        template<>
        struct CharTTraits<string_type::wide>
        {
            using Type = wchar_t;
        };

        template<>
        struct CharTTraits<string_type::utf8>
        {
            using Type = char;
        };

        template<>
        struct CharTTraits<string_type::utf16>
        {
            using Type = char16_t;
        };

        template<>
        struct CharTTraits<string_type::utf32>
        {
            using Type = char32_t;
        };
    }

    template<string_type StringT>
    using char_type_t = typename Internal::CharTTraits<StringT>::Type;

    template<string_type>
    class basic_string;

    template<string_type StringT>
    class basic_string_view
    {
    public:
        using value_type = typename Internal::CharTTraits<StringT>::Type;
        using size_type = std::size_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = const_pointer;
        using const_iterator = iterator;

        static constexpr size_type npos = static_cast<size_type>(-1);

        CXX14_CONSTEXPR basic_string_view(const_pointer str, size_type len) noexcept
            :data_ { str }, size_ { len }
        {}

        CXX14_CONSTEXPR basic_string_view(const_iterator start, const_iterator last) noexcept
            : data_ { start }, size_ { static_cast<size_type>(last - start) }
        {}

        basic_string_view(const basic_string<StringT>& str);

        basic_string_view(const_pointer str) noexcept
            : basic_string_view { str, Internal::StrLen(str) }
        {}

        CXX14_CONSTEXPR basic_string_view() noexcept
            : basic_string_view { {}, {} }
        {}

        CXX14_CONSTEXPR basic_string_view(const basic_string_view&) noexcept = default;
        CXX14_CONSTEXPR basic_string_view& operator=(const basic_string_view&) noexcept = default;

        CXX14_CONSTEXPR const_iterator begin() const noexcept
        {
            return data_;
        }

        CXX14_CONSTEXPR const_iterator end() const noexcept
        {
            return data_ + size_;
        }

        CXX14_CONSTEXPR const_iterator cbegin() const noexcept
        {
            return begin();
        }

        CXX14_CONSTEXPR const_iterator cend() const noexcept
        {
            return end();
        }

        CXX14_CONSTEXPR size_type size() const noexcept
        {
            return size_;
        }

        CXX14_CONSTEXPR bool empty() const noexcept
        {
            return size() == 0;
        }

        CXX14_CONSTEXPR explicit operator bool() const noexcept
        {
            return size() != 0;
        }

        CXX14_CONSTEXPR const_reference operator[](size_type pos) const
        {
            return data_[pos];
        }

        CXX14_CONSTEXPR const_reference at(size_type pos) const
        {
            if (pos >= size()) throw std::out_of_range { "pos >= size()!" };
            return data_[pos];
        }

        CXX14_CONSTEXPR const_pointer data() const noexcept
        {
            return data_;
        }

        CXX14_CONSTEXPR void clear() noexcept
        {
            *this = basic_string_view {};
        }

        CXX14_CONSTEXPR void remove_prefix(size_type n)
        {
            assert(n <= size());
            data_ += n;
            size_ -= n;
        }

        CXX14_CONSTEXPR void remove_suffix(size_type n)
        {
            assert(n <= size());
            size_ -= n;
        }

        size_type copy(pointer s, size_type n, size_type pos = {}) const
        {
            const auto sizeToCopy = min(size() - pos, n);
            (void)std::copy_n(data(), static_cast<std::ptrdiff_t>(sizeToCopy), s);
            return sizeToCopy;
        }

        void swap(basic_string_view& s) noexcept
        {
            using std::swap;
            swap(data_, s.data_);
            swap(size_, s.size_);
        }

        basic_string_view substr(size_type pos = {}, size_type n = npos) noexcept
        {
            if (pos > size()) throw std::out_of_range("pos > size()!");
            return { data_ + pos, min(n, size_ - pos) };
        }

        int compare(basic_string_view str) const noexcept
        {
            const auto lSize = size();
            const auto rSize = str.size();
            const auto rlen = std::min(lSize, rSize);
            const auto res = Internal::StrCmp(cbegin(), str.cbegin(), rlen);
            if (res != 0) return res;
            else
            {
                if (lSize < rSize) return -1;
                else if (lSize == rSize) return 0;
                else return 1;
            }
        }

        size_type find(const basic_string_view& pattern, size_type pos = {}) const
        {
            const auto p = make_boyer_moore_searcher(pattern.begin(), pattern.end())(begin() + pos, end());
            if (p.first == end()) return npos;
            return static_cast<size_type>(p.first - begin());
        }

        size_type find(const_pointer s, size_type pos) const
        {
            return find(basic_string_view(s), pos);
        }

        size_type find(const_pointer s, size_type pos, size_type n) const
        {
            return find(basic_string_view(s, n), pos);
        }

        size_type find(value_type c, size_type pos) const noexcept
        {
            const auto searchStart = begin() + pos;
            const auto it = std::find(searchStart, end(), c);
            if (it == end()) return npos;
            return static_cast<size_type>(it - begin() - pos);
        }

        CXX14_CONSTEXPR size_type find_first_of(const basic_string_view& v, size_type pos = {}) const noexcept
        {
            size_type res {};
            for (const auto& c : *this)
            {
                ++res;
                for (const auto& p : v)
                    if (p == c)
                        return res;
            }
            return npos;
        }

        CXX14_CONSTEXPR size_type find_first_of(const_pointer s, size_type pos) const noexcept
        {
            return find_first_of(basic_string_view(s), pos);
        }

        CXX14_CONSTEXPR size_type find_first_of(const_pointer s, size_type pos, size_type n) const noexcept
        {
            return find_first_of(basic_string_view(s, n), pos);
        }

    private:
        const_pointer data_;
        size_type size_;
    };

    template<string_type StringT>
    decltype(auto) begin(basic_string_view<StringT>& view) noexcept
    {
        return view.begin();
    }

    template<string_type StringT>
    decltype(auto) begin(const basic_string_view<StringT>& view) noexcept
    {
        return view.begin();
    }

    template<string_type StringT>
    decltype(auto) cbegin(const basic_string_view<StringT>& view) noexcept
    {
        return begin(view);
    }

    template<string_type StringT>
    decltype(auto) end(basic_string_view<StringT>& view) noexcept
    {
        return view.end();
    }

    template<string_type StringT>
    decltype(auto) end(const basic_string_view<StringT>& view) noexcept
    {
        return view.end();
    }

    template<string_type StringT>
    decltype(auto) cend(const basic_string_view<StringT>& view) noexcept
    {
        return end(view);
    }

    template<string_type StringT>
    decltype(auto) size(const basic_string_view<StringT>& view) noexcept
    {
        return view.size();
    }

    template<string_type StringT>
    constexpr bool operator==(basic_string_view<StringT> lhs, basic_string_view<StringT> rhs) noexcept
    {
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<string_type StringT>
    constexpr bool operator!=(basic_string_view<StringT> lhs, basic_string_view<StringT> rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template<string_type StringT>
    constexpr bool operator<(basic_string_view<StringT> lhs, basic_string_view<StringT> rhs) noexcept
    {
        return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<string_type StringT>
    constexpr bool operator<=(basic_string_view<StringT> lhs, basic_string_view<StringT> rhs) noexcept
    {
        return !(rhs < lhs);
    }

    template<string_type StringT>
    constexpr bool operator>(basic_string_view<StringT> lhs, basic_string_view<StringT> rhs) noexcept
    {
        return rhs < lhs;
    }

    template<string_type StringT>
    constexpr bool operator>=(basic_string_view<StringT> lhs, basic_string_view<StringT> rhs) noexcept
    {
        return !(lhs < rhs);
    }

    using u8string_view = basic_string_view<string_type::utf8>;
    using wstring_view = basic_string_view<string_type::wide>;
    using u16string_view = basic_string_view<string_type::utf16>;
    using u32string_view = basic_string_view<string_type::utf32>;

    namespace Literals
    {
        inline namespace StringViewLiterals
        {
            inline /*constexpr */u8string_view operator"" _u8v(const char* str, std::size_t len) noexcept
            {
                return { str, len };
            }

            inline /*constexpr */wstring_view operator"" _wv(const wchar_t* str, std::size_t len) noexcept
            {
                return { str, len };
            }

            inline /*constexpr */u16string_view operator"" _u16v(const char16_t* str, std::size_t len) noexcept
            {
                return { str, len };
            }

            inline /*constexpr */u32string_view operator"" _u16v(const char32_t* str, std::size_t len) noexcept
            {
                return { str, len };
            }
        }
    }
}