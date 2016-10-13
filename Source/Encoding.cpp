#include "Encoding.hpp"
#include "Assert.hpp"
#include "Scope.hpp"
#include <climits>

namespace Yupei
{
    std::uint32_t ReadCodeUnit(const char* p) noexcept
    {
        assert(p != nullptr);
        return static_cast<std::uint32_t>(*reinterpret_cast<const std::uint8_t*>(p));
    }

    char RestoreUtf8CodeUnit(std::uint32_t codeUnit) noexcept
    {
        assert((codeUnit >> 8) == 0);
        const auto narrow = static_cast<std::uint8_t>(codeUnit);
        return reinterpret_cast<const char&>(narrow);
    }

    //最大的 code point 是 0x10FFFF，不会涉及到符号位的问题。
    //constexpr ?
    std::uint32_t ReadCodePoint(char32_t codePoint) noexcept
    {
        return codePoint; //reinterpret_cast<std::uint32_t&>(codePoint);
    }

    char32_t RestoreCodePoint(std::uint32_t codePoint) noexcept
    {
        assert(codePoint <= 0x10FFFF);
        return codePoint; //reinterpret_cast<char32_t&>(codePoint);
    }

    //http://www.unicode.org/versions/corrigendum1.html
    //http://clang.llvm.org/doxygen/ConvertUTF_8c_source.html

    bool IsValidUtf8(const char* start, std::uint8_t length) noexcept
    {
        assert(start != nullptr);

        auto pLast = start + length + 1;
        const auto firstCodeUnit = ReadCodeUnit(start);
        std::uint32_t codeUnit;

#define UNROLL \
        codeUnit = ReadCodeUnit(--pLast);\
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
  
    char32_t GetFirstCodePoint(const char*& start, const char* last)
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
        const auto firstUnit = ReadCodeUnit(start);
        std::uint32_t resultCodePoint = {};

        SCOPE_EXIT{ start = cur; };
       
        const auto bytesLeft = kTrailingBytesForUtf8[firstUnit];

        if (last - start < bytesLeft + 1)
        {
            return kSourceExhausted;
        }
     
        const auto EncodingUtf8 = [&] {
            resultCodePoint += ReadCodeUnit(cur++); 
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
            resultCodePoint += ReadCodeUnit(cur++);        
        }
        resultCodePoint -= kOffsetsFromUTF8[bytesLeft];
        return RestoreCodePoint(resultCodePoint);

    HandleInvalid:
        return kInvalidCodePoint;
    }

    void GetUtf32Chars(std::string_view inUtf8String, gsl::span<char32_t> outUtf32Chars)
    {
        if (inUtf8String.empty() || outUtf32Chars.empty())
            return;

        auto pSrcStart = inUtf8String.data();
        const auto pSrcEnd = pSrcStart + inUtf8String.size();
        auto desStart = outUtf32Chars.begin();

        while (pSrcStart != pSrcEnd)
        {
            assert(pSrcStart <= pSrcEnd);           
            const auto codePoint = GetFirstCodePoint(pSrcStart, pSrcEnd);
            switch (codePoint)
            {
            case kInvalidCodePoint:
                throw EncodingError(EncodingErrorCode::kInvalidCodePoint);
            case kSourceExhausted:
                throw EncodingError(EncodingErrorCode::kSourceExhausted);
            default:
                break;
            }
            *desStart++ = codePoint;
        }
    }

    std::uint8_t GetCodeUnitLength(char32_t codePoint) noexcept
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

    char32_t GetFirstCodeUnits(char32_t codePoint, char*& desStart, const char* desLast)
    {
        const auto bytesToWrite = GetCodeUnitLength(codePoint);
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

    void GetUtf8Chars(std::u32string_view inUtf32String, gsl::span<char> outUtf8Chars)
    {
        auto cur = outUtf8Chars.data();
        const auto pLast = cur + outUtf8Chars.size();
        for (auto codePoint : inUtf32String)
        {
            const auto result = GetFirstCodeUnits(codePoint, cur, pLast);
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
    }
}