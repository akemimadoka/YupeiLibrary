#include "CharTraitsHelpers.hpp"
#include <string> //for std::char_traits

namespace Yupei
{
    namespace Internal
    {
        std::size_t StrLen(const char* str) noexcept
        {
            return std::char_traits<char>::length(str);
        }

        std::size_t StrLen(const wchar_t* str) noexcept
        {
            return std::char_traits<wchar_t>::length(str);
        }

        std::size_t StrLen(const char16_t* str) noexcept
        {
            return std::char_traits<char16_t>::length(str);
        }

        std::size_t StrLen(const char32_t* str) noexcept
        {
            return std::char_traits<char32_t>::length(str);
        }

        int StrCmp(const char* s1, const char* s2, std::size_t count) noexcept
        {
            return std::char_traits<char>::compare(s1, s2, count);
        }

        int StrCmp(const wchar_t* s1, const wchar_t* s2, std::size_t count) noexcept
        {
            return std::char_traits<wchar_t>::compare(s1, s2, count);
        }

        int StrCmp(const char16_t* s1, const char16_t* s2, std::size_t count) noexcept
        {
            return std::char_traits<char16_t>::compare(s1, s2, count);
        }

        int StrCmp(const char32_t* s1, const char32_t* s2, std::size_t count) noexcept
        {
            return std::char_traits<char32_t>::compare(s1, s2, count);
        }
    }
}