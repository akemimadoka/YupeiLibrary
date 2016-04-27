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
        extern int StrCmp(const char* s1, const char* s2, std::size_t count) noexcept;
        extern int StrCmp(const wchar_t* s1, const wchar_t* s2, std::size_t count) noexcept;
        extern int StrCmp(const char16_t* s1, const char16_t* s2, std::size_t count) noexcept;
        extern int StrCmp(const char32_t* s1, const char32_t* s2, std::size_t count) noexcept;
    }
}