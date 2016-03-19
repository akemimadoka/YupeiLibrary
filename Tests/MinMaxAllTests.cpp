#include <MinMax.hpp>
#include <UnitTest.hpp>
#include <string>

TEST_CASE(MinTest)
{
    std::string str2("lalala");
    auto&& str = Yupei::min("haha", str2);    
    TEST_ASSERT(str == "haha");   
}