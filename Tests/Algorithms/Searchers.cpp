#include <Searchers.hpp>
#include <catch.hpp>
#include <string>

TEST_CASE("BMSearcher")
{
    using namespace Yupei;
    std::string pat = "GCAGAGAG";
    std::string str = "GCATCGCAGAGAGTATACAGTACG";
    auto i = make_boyer_moore_searcher(begin(pat), end(pat))(begin(str), end(str));
    CHECK(i.first == begin(str) + 5);

    pat = "AT_THAT";
    str = "WHICH_FINALLY_HALTS.__AT_THAT_POINT";
    i = make_boyer_moore_searcher(begin(pat), end(pat))(begin(str), end(str));
    CHECK(i.first == begin(str) + 22);
}