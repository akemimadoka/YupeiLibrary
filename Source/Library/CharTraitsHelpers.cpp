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
    }   
}