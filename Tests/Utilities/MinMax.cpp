#include <MinMax.hpp>
#include <catch.hpp>
#include <string>

TEST_CASE("Min")
{
    std::string str2("lalala");
    auto&& str = Yupei::min("haha", str2);    
    CHECK(str == "haha");   
}