#pragma once

#include <cstddef>

namespace Yupei
{
    namespace Internal
    {
        extern std::size_t StrLen(const char* str) noexcept;
        extern std::size_t StrLen(const wchar_t* str) noexcept;
        extern std::size_t StrLen(const char16_t* str) noexcept;
        extern std::size_t StrLen(const char32_t* str) noexcept;
    }
    
}
