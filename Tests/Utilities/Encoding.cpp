#include <Encoding.hpp>
#include <catch.hpp>
#include <iterator>
#include <algorithm>

TEST_CASE("UTF8 -> UTF32")
{
    using namespace Yupei;
    {
        const char sequence[] = { '\xF0', '\xA3', '\x8E', '\xB4' };
        auto p = std::begin(sequence);
        const auto codePoint = GetFirstCodePoint(p, std::end(sequence));
        CHECK(codePoint == 0x233B4u);
        CHECK(p == std::end(sequence));
    }
    
    {
        const char sequence[] = { '\xC0', '\x80' };
        auto p = std::begin(sequence);
        const auto codePoint = GetFirstCodePoint(p, std::end(sequence));
        CHECK(codePoint == kInvalidCodePoint);
    }

    {
        const char32_t codePoint = 0x233B4u;
        CHECK(GetCodeUnitLength(codePoint) == 4);
        char codeUnits[4] = {};
        char* pCur = codeUnits;
        GetFirstCodeUnits(codePoint, pCur, std::end(codeUnits));
        CHECK(pCur == std::end(codeUnits));
        const char sequence[] = { '\xF0', '\xA3', '\x8E', '\xB4' };
        CHECK(std::equal(std::begin(codeUnits), std::end(codeUnits), sequence));
    }
}