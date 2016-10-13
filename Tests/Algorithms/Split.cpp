//#include <Config.hpp>
//
//#ifdef HAS_COROUTINE
//#define _HAS_CXX17 1
//
//#include <Algorithm/Split.hpp>
//#include <catch.hpp>
//#include <algorithm>
//#include <vector>
//
//TEST_CASE("Split")
//{
//    std::string str = "a,bd,c,d.e.f";
//    std::vector<std::string_view> vec { "a","bd","c","d","e","f" };
//    auto g = split(str, u8",.");
//    CHECK(std::equal(g.begin(), g.end(), vec.begin(), vec.end()));
//
//    u8string str2 = "";
//    auto g2 = split(str2.view(), as_span(",."));
//    CHECK(*g2.begin() == ""_u8v);
//
//    u8string str3 = ",";
//    auto g3 = split(str3.view(), as_span(",."));
//    CHECK(*g3.begin() == ""_u8v);
//
//    u8string str4 = ",";
//    auto g4 = split(str4.view(), as_span(",."));
//    CHECK(*g4.begin() == ""_u8v);
//}
//
//#endif