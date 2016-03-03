#include <Searchers.hpp>
#include <UnitTest.hpp>
#include <string>

TEST_CASE(BMSearcher)
{
    using namespace Yupei;
    std::string pat = "GCAGAGAG";
    std::string str = "GCATCGCAGAGAGTATACAGTACG";
    const auto i = make_boyer_moore_searcher(begin(pat), end(pat))(begin(str), end(str));
    TEST_ASSERT(i == begin(str) + 5);
}