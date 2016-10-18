#include "Encoding.hpp"
#include "Assert.hpp"
#include "Scope.hpp"
#include <climits>

namespace Yupei
{
    std::uint32_t ReadUtf8CodeUnit(const char* p) noexcept
    {
        assert(p != nullptr);
        return static_cast<std::uint32_t>(*reinterpret_cast<const std::uint8_t*>(p));
    }

    std::uint32_t ReadUtf16CodeUnit(const char16_t* p) noexcept
    {
        assert(p != nullptr);
        return static_cast<std::uint32_t>(*p);
    }

    char RestoreUtf8CodeUnit(std::uint32_t codeUnit) noexcept
    {
        assert((codeUnit >> 8) == 0);
        const auto narrow = static_cast<std::uint8_t>(codeUnit);
        return reinterpret_cast<const char&>(narrow);
    }

    char16_t RestoreUtf16CodeUnit(std::uint32_t codeUnit) noexcept
    {
        assert((codeUnit >> 16) == 0);
        return static_cast<char16_t>(codeUnit);
    }

    //constexpr ?
    std::uint32_t ReadCodePoint(char32_t codePoint) noexcept
    {
        return static_cast<std::uint32_t>(codePoint);
    }

    char32_t RestoreCodePoint(std::uint32_t codePoint) noexcept
    {
        assert(codePoint <= 0x10FFFF);
        return static_cast<char32_t>(codePoint);
    }

    void ProcessEncodingResult(char32_t result)
    {
        switch (result)
        {
        case kInvalidCodePoint:
            throw EncodingError(EncodingErrorCode::kInvalidCodePoint);
        case kSourceExhausted:
            throw EncodingError(EncodingErrorCode::kSourceExhausted);
        default:
            break;
        }
    }

    void ProcessDecodingResult(char32_t result)
    {
        switch (result)
        {
        case kInvalidCodeUnit:
            throw EncodingError{ EncodingErrorCode::kInvalidCodeUnit };
        case kInsufficientSpace:
            throw EncodingError{ EncodingErrorCode::kInsufficientSpace };
        default:
            break;
        }
    }

    //http://www.unicode.org/versions/corrigendum1.html
    //http://clang.llvm.org/doxygen/ConvertUTF_8c_source.html

#pragma region UTF16 -> UTF-8

    void GetUtf8Chars(std::u16string_view inUtf16String, gsl::span<char> outUtf8Chars)
    {
        auto pSrcCur = inUtf16String.data();
        const auto pSrcEnd = pSrcCur + inUtf16String.size();
        auto pDesCur = outUtf8Chars.data();
        const auto pDesEnd = pDesCur + outUtf8Chars.size();
        while (pSrcCur < pSrcEnd)
        {
            const auto result = GetFirstUtf16CodePoint(pSrcCur, pSrcEnd);
            ProcessDecodingResult(result);
            ProcessEncodingResult(GetFirstUtf8CodeUnits(result, pDesCur, pDesEnd));
        }
    }

    std::size_t GetUtf8CharsLength(std::u16string_view inUtf16String);

    void GetUtf8String(std::u16string_view inUtf16String, std::string& outUtf8String)
    {
        char codeUnits[4] = {};
        auto pSrcCur = inUtf16String.data();
        const auto pSrcEnd = pSrcCur + inUtf16String.size();
        while (pSrcCur < pSrcEnd)
        {
            const auto pDesStart = std::begin(codeUnits);
            auto pDesCur = pDesStart;
            const auto pDesEnd = std::end(codeUnits);
            const auto result = GetFirstUtf16CodePoint(pSrcCur, pSrcEnd);
            ProcessDecodingResult(result);
            ProcessEncodingResult(GetFirstUtf8CodeUnits(result, pDesCur, pDesEnd));
            outUtf8String.append(pDesStart, pDesCur - pDesStart);
        }
    }

#pragma endregion UTF16 -> UTF-8

#pragma region UTF8 -> UTF16

    void GetUtf16Chars(std::string_view inUtf8String, gsl::span<char16_t> outUtf16Chars)
    {
        auto pSrcCur = inUtf8String.data();
        const auto pSrcEnd = pSrcCur + inUtf8String.size();
        auto pDesCur = outUtf16Chars.data();
        const auto pDesEnd = pDesCur + outUtf16Chars.size();
        while (pSrcCur < pSrcEnd)
        {
            const auto result = GetFirstUtf8CodePoint(pSrcCur, pSrcEnd);
            ProcessDecodingResult(result);
            ProcessEncodingResult(GetFirstUtf16CodeUnits(result, pDesCur, pDesEnd));
        }
    }

    //TODO
    std::size_t GetUtf16CharsLength(std::string_view inUtf8String)
    {

    }

    void GetUtf16String(std::string_view inUtf8String, std::u16string& outUtf16String)
    {
        char16_t codeUnits[2] = {};
        auto pSrcCur = inUtf8String.data();
        const auto pSrcEnd = pSrcCur + inUtf8String.size();
        while (pSrcCur < pSrcEnd)
        {
            const auto pDesStart = std::begin(codeUnits);
            auto pDesCur = pDesStart;
            const auto pDesEnd = std::end(codeUnits);
            const auto result = GetFirstUtf8CodePoint(pSrcCur, pSrcEnd);
            ProcessDecodingResult(result);
            ProcessEncodingResult(GetFirstUtf16CodeUnits(result, pDesCur, pDesEnd));
            outUtf16String.append(pDesStart, pDesCur - pDesStart);
        }
    }

#pragma endregion UTF8 -> UTF16

#pragma region UTF8 -> UTF32

    bool IsValidUtf8(const char* start, std::uint8_t length) noexcept
    {
        assert(start != nullptr);

        auto pLast = start + length + 1;
        const auto firstCodeUnit = ReadUtf8CodeUnit(start);
        std::uint32_t codeUnit;

#define UNROLL \
        codeUnit = ReadUtf8CodeUnit(--pLast);\
        if (codeUnit < 0x80 || codeUnit > 0xBF)\
            return false;

        switch (length)
        {
        default:
            return false;
        case 3: UNROLL
            [[fallthrough]];
        case 2: UNROLL
            [[fallthrough]];
        case 1: UNROLL
            switch (firstCodeUnit)
            {
            case 0xE0u: if (codeUnit < 0xA0u) return false; break;
            case 0xEDu: if (codeUnit > 0x9Fu) return false; break;
            case 0xF0u: if (codeUnit < 0x90u) return false; break;
            case 0xF4u: if (codeUnit > 0x8Fu) return false; break;
            default: break;
            }
                [[fallthrough]];
        case 0:
            if (firstCodeUnit >= 0x80u && firstCodeUnit < 0xC2u) return false;
        }
        //第一个 code unit 的合法范围为 0 - 0x7F, 0xC2 - 0xF4
        return firstCodeUnit <= 0xF4u;
#undef UNROLL
    }

    char32_t GetFirstUtf8CodePoint(const char*& start, const char* last)
    {
        constexpr std::uint8_t kTrailingBytesForUtf8[256] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
        };

        constexpr std::uint32_t kOffsetsFromUTF8[6] = {
            0x00000000u, 0x00003080u, 0x000E2080u, 0x03C82080u
        };

        YPASSERT(start != nullptr, "Input pointers should not be null");

        if (start == last) return kSourceExhausted;

        auto cur = start;
        const auto firstUnit = ReadUtf8CodeUnit(start);
        std::uint32_t resultCodePoint = {};

        SCOPE_EXIT{ start = cur; };

        const auto bytesLeft = kTrailingBytesForUtf8[firstUnit];

        if (last - start < bytesLeft + 1)
        {
            return kSourceExhausted;
        }

        const auto EncodingUtf8 = [&] {
            resultCodePoint += ReadUtf8CodeUnit(cur++);
            resultCodePoint <<= 6;
        };

        if (!IsValidUtf8(start, bytesLeft))
            goto HandleInvalid;

        switch (bytesLeft)
        {
        default:
            goto HandleInvalid;
            break;
        case 3:
            EncodingUtf8();
            [[fallthrough]];
        case 2:
            EncodingUtf8();
            [[fallthrough]];
        case 1:
            EncodingUtf8();
            [[fallthrough]];
        case 0:
            resultCodePoint += ReadUtf8CodeUnit(cur++);
        }
        resultCodePoint -= kOffsetsFromUTF8[bytesLeft];
        return RestoreCodePoint(resultCodePoint);

    HandleInvalid:
        return kInvalidCodePoint;
    }

    namespace Internal
    {
        template<typename OutputIteratorT>
        void OutputUtf32(std::string_view inUtf8String, OutputIteratorT iter)
        {
            auto pSrcStart = inUtf8String.data();
            const auto pSrcEnd = pSrcStart + inUtf8String.size();
            while (pSrcStart != pSrcEnd)
            {
                assert(pSrcStart <= pSrcEnd);
                const auto result = GetFirstUtf8CodePoint(pSrcStart, pSrcEnd);
                ProcessEncodingResult(result);
                *iter++ = result;
            }
        }
    }

    void GetUtf32Chars(std::string_view inUtf8String, gsl::span<char32_t> outUtf32Chars)
    {
        Internal::OutputUtf32(inUtf8String, outUtf32Chars.begin());
    }

    //TODO
    std::size_t GetUtf32CharsLength(std::string_view inUtf8String);

    void GetUtf32String(std::string_view inUtf8String, std::u32string& outUtf32String)
    {
        Internal::OutputUtf32(inUtf8String, std::back_inserter(outUtf32String));
    }

#pragma endregion UTF8 -> UTF32

#pragma region UTF32 -> UTF8

    void GetUtf8String(std::u32string_view inUtf32String, std::string& outUtf8String)
    {
        const auto totalLength = GetUtf16CharsLength(inUtf32String);
        outUtf8String.resize(totalLength);
        GetUtf8Chars(inUtf32String, outUtf8String);
    }

    char32_t GetFirstUtf8CodeUnits(char32_t codePoint, char*& desStart, const char* desLast)
    {
        const auto bytesToWrite = GetUtf8CodeUnitLength(codePoint);
        auto codePointRead = ReadCodePoint(codePoint);

        if (desLast - desStart < bytesToWrite)
            return kInsufficientSpace;

        desStart += bytesToWrite;
        auto curPos = desStart;

        constexpr std::uint32_t ByteMark = 0x80;
        constexpr std::uint32_t ByteMask = 0x3F;
        constexpr std::uint32_t kFirstByteMark[] = { 0x00, 0xC0, 0xE0, 0xF0 };

        const auto DecodingUtf8 = [&] {
            *--curPos = RestoreUtf8CodeUnit((codePointRead & ByteMask) | ByteMark);
            codePointRead >>= 6;
        };

        switch (bytesToWrite)
        {
        case 4:
            DecodingUtf8();
            [[fallthrough]];
        case 3:
            DecodingUtf8();
            [[fallthrough]];
        case 2:
            DecodingUtf8();
            [[fallthrough]];
        case 1:
            *--curPos = RestoreUtf8CodeUnit(codePointRead | kFirstByteMark[bytesToWrite - 1]);
            break;
        default:
            return kInvalidCodePoint;
        }
        return codePoint;
    }

    std::uint8_t GetUtf8CodeUnitLength(char32_t codePoint) noexcept
    {
        const auto codePointRead = ReadCodePoint(codePoint);
        if (codePointRead <= 0x7F)
        {
            return 1;
        }
        else if (codePointRead <= 0x7FF)
        {
            return 2;
        }
        else if (codePointRead <= 0xFFFF)
        {
            return 3;
        }
        else if (codePointRead <= 0x10FFFF)
        {
            return 4;
        }
        else
            return UINT8_MAX;
    }

    void GetUtf8Chars(std::u32string_view inUtf32String, gsl::span<char> outUtf8Chars)
    {
        auto cur = outUtf8Chars.data();
        const auto pLast = cur + outUtf8Chars.size();
        for (auto codePoint : inUtf32String)
        {
            ProcessDecodingResult(GetFirstUtf8CodeUnits(codePoint, cur, pLast));
        }
    }

    std::size_t GetUtf8CharsLength(std::u32string_view inUtf32String)
    {
        return std::accumulate(inUtf32String.cbegin(), inUtf32String.cend(), std::size_t{},
            [](std::size_t now, char32_t codeUnit)
        {
            return now + GetUtf8CodeUnitLength(codeUnit);
        });
    }

    //TODO
    void GetUtf8String(std::u32string_view inUtf32String, std::string& outUtf8String);

#pragma endregion UTF32 -> UTF8

#pragma region UTF16 -> UTF32

    bool IsBmp(char16_t c) noexcept
    {
        return !IsHighSurrogate(c) && !IsLowSurrogate(c);
    }

    bool IsHighSurrogate(char16_t c) noexcept
    {
        return c <= kHighSurrogateEnd && c >= kHighSurrogateStart;
    }

    bool IsLowSurrogate(char16_t c) noexcept
    {
        return c <= kLowSurrogateEnd && c >= kLowSurrogateStart;
    }

    constexpr std::uint32_t kHalfBase = 0x10000u;

    char32_t GetFirstUtf16CodePoint(const char16_t*& start, const char16_t* last)
    {
        char16_t high, low;
        auto cur = start;
        high = *cur;

        const auto PackAsCodePoint = [](char16_t low, char16_t high)
        {
            return ((static_cast<std::uint32_t>(high) - kHighSurrogateStart) << 10) +
                (static_cast<std::uint32_t>(low) - kLowSurrogateStart) + kHalfBase;
        };

        SCOPE_EXIT{ start = cur; };

        if (IsHighSurrogate(high))
        {
            ++cur;
            if (cur < last)
            {
                low = *cur;
                if (IsLowSurrogate(low))
                {
                    ++cur;
                    return PackAsCodePoint(low, high);
                }
                else return kInvalidCodeUnit;
            }
            else return kSourceExhausted;
        }
        //BMP
        else if (!IsLowSurrogate(high)) return high;
        else return kInvalidCodeUnit;
    }

    namespace Internal
    {
        template<typename OutputIteratorT>
        void OutputUtf32(std::u16string_view inUtf16String, OutputIteratorT iter)
        {
            auto pCur = inUtf16String.data();
            const auto pSrcEnd = pCur + inUtf16String.size();

            while (pCur < pSrcEnd)
            {
                const auto result = GetFirstUtf16CodePoint(pCur, pSrcEnd);
                ProcessDecodingResult(result);
                *iter++ = result;
            }
        }
    }

    void GetUtf32Chars(std::u16string_view inUtf16String, gsl::span<char32_t> outUtf32Chars)
    {
        Internal::OutputUtf32(inUtf16String, outUtf32Chars.begin());
    }

    //TODO
    std::size_t GetUtf32CharsLength(std::u16string_view inUtf168String);

    void GetUtf32String(std::u16string_view inUtf16String, std::u32string& outUtf32String)
    {
        Internal::OutputUtf32(inUtf16String, std::back_inserter(outUtf32String));
    }

#pragma endregion UTF16 -> UTF32
  
#pragma region UTF32 -> UTF16

    char32_t GetFirstUtf16CodeUnits(char32_t codePoint, char16_t*& desStart, const char16_t* desLast)
    {
        const auto codeUnitLength = GetUtf16CodeUnitLength(codePoint);
        switch (codeUnitLength)
        {
        case 1:
            *desStart++ = static_cast<char16_t>(codePoint);
            return codePoint;
        case 2:
            if (desLast - desStart < 1)
                return kInsufficientSpace;
            *desStart++ = RestoreUtf16CodeUnit((codePoint >> 10) + static_cast<std::uint32_t>(kHighSurrogateStart));
            *desStart++ = RestoreUtf16CodeUnit((codePoint & 0x3FFu) + static_cast<std::uint32_t>(kLowSurrogateStart));
            return codePoint;
        default:
            return kInvalidCodePoint;
        }            
    }

    std::uint8_t GetUtf16CodeUnitLength(char32_t codePoint) noexcept
    {
        if (codePoint < kMaxBmp)
        {
            //此时合法的 Code Point 只能是 BMP。
            if (codePoint <= kLowSurrogateEnd && codePoint >= kHighSurrogateStart)
                return UINT8_MAX;
            else
                return 1;
        }
        else if (codePoint < kMaxValidCodePoint)
            return 2;
        else
            return UINT8_MAX;
    }


    void GetUtf16Chars(std::u32string_view inUtf32String, gsl::span<char16_t> outUtf16Chars)
    {
        auto pCur = outUtf16Chars.data();
        const auto pLast = pCur + outUtf16Chars.size();
        for (auto codePoint : inUtf32String)
        {
            ProcessEncodingResult(GetFirstUtf16CodeUnits(codePoint, pCur, pLast));
        }
    }

    std::size_t GetUtf16CharsLength(std::u32string_view inUtf32String)
    {
        return std::accumulate(inUtf32String.cbegin(), inUtf32String.cend(), std::size_t{},
            [](std::size_t now, char32_t codeUnit)
        {
            return now + GetUtf16CodeUnitLength(codeUnit);
        });
    }

    void GetUtf16String(std::u32string_view inUtf32String, std::u16string& outUtf16String)
    {
        const auto length = GetUtf16CharsLength(inUtf32String);
        outUtf16String.resize(length);
        GetUtf16Chars(inUtf32String, outUtf16String);
    }

#pragma endregion UTF32 -> UTF16
}