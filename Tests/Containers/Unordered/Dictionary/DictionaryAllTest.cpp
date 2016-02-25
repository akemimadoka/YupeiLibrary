#include <Containers\Dictionary.hpp>
#include <UnitTest.hpp>
#include <string>

using namespace Yupei;

TEST_CASE(DictionaryClear)
{
    pair<int, std::string> a[] = {
        {1, "one"},
        {2, "two"},
        {3, "three"},
        {4, "four"},
        {1, "four"},
        {2, "four"}
    };

    dictionary<int, std::string> c(a, a + sizeof(a) / sizeof(a[0]));
    c.clear();
    TEST_ASSERT(c.size() == 0);
}