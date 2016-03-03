#include <MinMax.hpp>
#include <UnitTest.hpp>
#include <string>
#include <iostream>

TEST_CASE(MinTest)
{
    std::string str2("lalala");
    auto&& str = Yupei::min("haha", str2);
    std::cout << str;
    TEST_ASSERT(str == "haha");   
}