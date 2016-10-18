#pragma once

//To define _HAS_CXX17 for EDG (Intellisense).
#include "Config.hpp"

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

    static constexpr char32_t kMaxValidCodePoint = 0x10FFFFu;

    //UTF16
    static constexpr char16_t kHighSurrogateStart = 0xD800u;
    static constexpr char16_t kHighSurrogateEnd = 0xDBFFu;
    static constexpr char16_t kLowSurrogateStart = 0xDC00u;
    static constexpr char16_t kLowSurrogateEnd = 0xDFFFu;
    static constexpr char32_t kMaxBmp = 0xFFFFu;

    class EncodingError : public std::invalid_argument
    {
    public:
        EncodingErrorCode ErrorCode;

        EncodingError(EncodingErrorCode errorCode)
            : ErrorCode{ errorCode }, std::invalid_argument{""}
        {}
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
    char32_t GetFirstUtf8CodePoint(const char*& start, const char* last);
    void GetUtf32Chars(std::string_view inUtf8String, gsl::span<char32_t> outUtf32Chars);
    std::size_t GetUtf32CharsLength(std::string_view inUtf8String);
    void GetUtf32String(std::string_view inUtf8String, std::u32string& outUtf32String);

    //UTF32 -> UTF8
    char32_t GetFirstUtf8CodeUnits(char32_t codePoint, char*& desStart, const char* desLast);
    std::uint8_t GetUtf8CodeUnitLength(char32_t codePoint) noexcept;
    void GetUtf8Chars(std::u32string_view inUtf32String, gsl::span<char> outUtf8Chars);
    std::size_t GetUtf8CharsLength(std::u32string_view inUtf8String);
    void GetUtf8String(std::u32string_view inUtf32String, std::string& outUtf8String);

    //UTF16 -> UTF32
    bool IsBmp(char16_t c) noexcept;
    bool IsHighSurrogate(char16_t c) noexcept;
    bool IsLowSurrogate(char16_t c) noexcept;
    char32_t GetFirstUtf16CodePoint(const char16_t*& start, const char16_t* last);
    void GetUtf32Chars(std::u16string_view inUtf16String, gsl::span<char32_t> outUtf32Chars);
    std::size_t GetUtf32CharsLength(std::u16string_view inUtf168String);
    void GetUtf32String(std::u16string_view inUtf16String, std::u32string& outUtf32String);

    //UTF32 -> UTF16
    char32_t GetFirstUtf16CodeUnits(char32_t codePoint, char16_t*& desStart, const char16_t* desLast);
    std::uint8_t GetUtf16CodeUnitLength(char32_t codePoint) noexcept;
    void GetUtf16Chars(std::u32string_view inUtf32String, gsl::span<char16_t> outUtf16Chars);
    std::size_t GetUtf16CharsLength(std::u32string_view inUtf32String);
    void GetUtf16String(std::u32string_view inUtf32String, std::u16string& outUtf16String);
}