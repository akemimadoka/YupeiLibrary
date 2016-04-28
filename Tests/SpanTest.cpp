#include <Span.hpp>
#include <UnitTest.hpp>
#include <algorithm>
#include <numeric>

TEST_CASE(Span)
{
    using namespace Yupei;
    int a[20];
    std::iota(std::begin(a), std::end(a), 0);
    auto sp = as_span(a);
    const auto x = sp.subspan(0, 3);
    const auto x2 = sp.subspan<3>(0);
    TEST_ASSERT(x[0] == 0);
    TEST_ASSERT(x[1] == 1);
    TEST_ASSERT(x[2] == 2);
    TEST_ASSERT(x2[0] == 0);
    TEST_ASSERT(x2[1] == 1);
    TEST_ASSERT(x2[2] == 2);
}