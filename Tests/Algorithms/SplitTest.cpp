#include <Config.hpp>

#ifdef HAS_COROUTINE

#include <Algorithm\Split.hpp>
#include <UnitTest.hpp>
#include <algorithm>
#include <vector>

TEST_CASE(SplitStringTest)
{
    using Yupei::u8string_view;
    using Yupei::u8string;
    using Yupei::as_span;
    using namespace std::experimental;
    using namespace Yupei::Literals;

    using std::cbegin;
    using std::cend;
    u8string str = "a,bd,c,d.e.f";
    std::vector<u8string_view> vec { "a","bd","c","d","e","f" };
    auto g = split(str.view(), as_span(u8",."));
    TEST_ASSERT(std::equal(g.begin(), g.end(), vec.begin(), vec.end()));

    u8string str2 = "";
    auto g2 = split(str2.view(), as_span(",."));
    TEST_ASSERT(*g2.begin() == ""_u8v);

    u8string str3 = ",";
    auto g3 = split(str3.view(), as_span(",."));
    TEST_ASSERT(*g3.begin() == ""_u8v);

    u8string str4 = ",";
    auto g4 = split(str4.view(), as_span(",."));
    TEST_ASSERT(*g4.begin() == ""_u8v);
}

#endif