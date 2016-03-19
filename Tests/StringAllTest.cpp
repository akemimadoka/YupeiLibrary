#include <UnitTest.hpp>
#include <String.hpp>

TEST_CASE(StringConstruct)
{
    using Yupei::utf8_string;

    utf8_string str {"hahahahahahahahahahahahahahahah"};
    TEST_ASSERT(str[0] == 'h');
    TEST_ASSERT(str[1] == 'a');
    TEST_ASSERT(str[2] == 'h');
    TEST_ASSERT(str[3] == 'a');
    TEST_ASSERT(str[4] == 'h');
    TEST_ASSERT(str[5] == 'a');
    TEST_ASSERT(str[6] == 'h');
    TEST_ASSERT(str[7] == 'a');
    TEST_ASSERT(str[8] == 'h');
    TEST_ASSERT(str[9] == 'a');
    TEST_ASSERT(str[10] == 'h');
    TEST_ASSERT(str[11] == 'a');
    TEST_ASSERT(str[12] == 'h');
    TEST_ASSERT(str[13] == 'a');
    TEST_ASSERT(str[14] == 'h');
    TEST_ASSERT(str[15] == 'a');
    TEST_ASSERT(str[16] == 'h');
    TEST_ASSERT(str[17] == 'a');
    TEST_ASSERT(str[18] == 'h');
    TEST_ASSERT(str[19] == 'a');
    TEST_ASSERT(str[20] == 'h');
    TEST_ASSERT(str[21] == 'a');
    TEST_ASSERT(str[22] == 'h');
    TEST_ASSERT(str[23] == 'a');
    TEST_ASSERT(str[24] == 'h');
    TEST_ASSERT(str[25] == 'a');
    TEST_ASSERT(str[26] == 'h');
    TEST_ASSERT(str[27] == 'a');
    TEST_ASSERT(str[28] == 'h');
    TEST_ASSERT(str[29] == 'a');
    TEST_ASSERT(str[30] == 'h');
    
    const auto cStr = str.c_str();
    TEST_ASSERT(cStr[31] == '\0');
}