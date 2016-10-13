#pragma once

#define _HAS_CXX17 1

#include "TypeAlias.hpp"
#include <span.h>
#include <string_view>
#include <stdexcept>

namespace Yupei
{
    enum class EncodingErrorCode
    {
        kInvalidCodePoint,
        kInvalidCodeUnit,
        kSourceExhausted,
        kInsufficientSpace
    };

    static constexpr char32_t kInvalidCodePoint = static_cast<char32_t>(-1);
    static constexpr char32_t kSourceExhausted = static_cast<char32_t>(-2);
    static constexpr char32_t kInvalidCodeUnit = static_cast<char32_t>(-3);
    static constexpr char32_t kInsufficientSpace = static_cast<char32_t>(-4);

    static constexpr char32_t kMaxValidCodePoint = static_cast<char32_t>(0x10FFFF);

    class EncodingError : public std::invalid_argument
    {
    public:
        EncodingErrorCode ErrorCode;

        EncodingError(EncodingErrorCode errorCode)
            : ErrorCode{ errorCode }, std::invalid_argument{""}
        {}

        //const char* what() const override;
    };


    //UTF16 -> UTF8
    void GetUtf8Chars(std::u16string_view inUtf16String, gsl::span<char> outUtf8Chars);
    std::size_t GetUtf8CharsLength(std::u16string_view inUtf16String);
    void GetUtf8String(std::u16string_view inUtf16String, std::string& outUtf8String);

    //UTF8 -> UTF16
    void GetUtf16Chars(std::string_view inUtf8String, gsl::span<char16_t> outUtf16Chars);
    std::size_t GetUtf16CharsLength(std::string_view inUtf8String);
    void GetUtf16String(std::string_view inUtf8String, std::u16string& outUtf16String);

    //UTF8 -> UTF32
    char32_t GetFirstCodePoint(const char*& start, const char* last);
    //bool IsValidUtf8(const char* start, std::uint8_t length) noexcept;
    void GetUtf32Chars(std::string_view inUtf8String, gsl::span<char32_t> outUtf32Chars);
    std::size_t GetUtf32CharsLength(std::string_view inUtf8String);
    void GetUtf32String(std::string_view inUtf8String, std::u32string& outUtf32String);

    //UTF32 -> UTF8
    char32_t GetFirstCodeUnits(char32_t codePoint, char*& desStart, const char* desLast);
    std::uint8_t GetCodeUnitLength(char32_t codePoint) noexcept;
    void GetUtf8Chars(std::u32string_view inUtf32String, gsl::span<char> outUtf8Chars);
    std::size_t GetUtf8CharsLength(std::u32string_view inUtf8String);
    void GetUtf8String(std::u32string_view inUtf16String, std::string& outUtf8String);

    //UTF16 -> UTF32
    void GetUtf32Chars(std::u16string_view inUtf16String, gsl::span<char32_t> outUtf32Chars);
    std::size_t GetUtf32CharsLength(std::u16string_view inUtf8String);
    void GetUtf32Chars(std::u16string_view inUtf16String, std::u32string& outUtf32String);

    //UTF32 -> UTF16
    void GetUtf16Chars(std::u32string_view inUtf8String, gsl::span<char16_t> outUtf16Chars);
    std::size_t GetUtf16CharsLength(std::u32string_view inUtf8String);
    void GetUtf16String(std::u32string_view inUtf8String, std::u16string& outUtf16String);
}