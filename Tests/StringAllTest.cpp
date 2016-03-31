#include <UnitTest.hpp>
#include <String.hpp>
#include <locale>
#include <sstream>
#include <string>
#include <iostream>

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
    TEST_ASSERT(str.size() == 31);
    
    const auto cStr = str.c_str();
    TEST_ASSERT(cStr[31] == '\0');
    TEST_ASSERT(str.size() == 31);
}

TEST_CASE(StringAppendAndReplace)
{
    using namespace Yupei;
    utf8_string str;
    for (int i = 0; i < 100; ++i)
        str.append("haha");
    
    TEST_ASSERT(str.size() == 400);

    str.append("Cool!");

    for (int i = 0;i < 400; i += 2)
    {
        TEST_ASSERT(str[i] == 'h');
        TEST_ASSERT(str[i + 1] == 'a');
    }

    //强制重分配。
    str.replace({"ha"}, {"haha"});

    TEST_ASSERT(str.size() == 805);

    str.replace("haha", "ha");

    TEST_ASSERT(str.size() == 405);
}

TEST_CASE(SmallStringReplace)
{
    using namespace Yupei;

    //测试均在小内存的情况。
    utf8_string str {"abcba"};

    str.replace("ab", "abc");

    TEST_ASSERT(str[0] == 'a');
    TEST_ASSERT(str[1] == 'b');
    TEST_ASSERT(str[2] == 'c');
    TEST_ASSERT(str[3] == 'c');
    TEST_ASSERT(str[4] == 'b');
    TEST_ASSERT(str[5] == 'a');

    TEST_ASSERT(str.size() == 6);

    auto cStr = str.c_str();

    TEST_ASSERT(cStr[6] == '\0');

    TEST_ASSERT(str.size() == 6);
}

TEST_CASE(SmallStringReplaceToBig)
{
    using namespace Yupei;

    //测试在小内存，replace 完毕后到大内存的情况。
    utf8_string str {"abcba"};

    str.replace("ab", "abcdefghi");

    TEST_ASSERT(str[0] == 'a');
    TEST_ASSERT(str[1] == 'b');
    TEST_ASSERT(str[2] == 'c');
    TEST_ASSERT(str[3] == 'd');
    TEST_ASSERT(str[4] == 'e');
    TEST_ASSERT(str[5] == 'f');
    TEST_ASSERT(str[6] == 'g');
    TEST_ASSERT(str[7] == 'h');
    TEST_ASSERT(str[8] == 'i');
    TEST_ASSERT(str[9] == 'c');
    TEST_ASSERT(str[10] == 'b');
    TEST_ASSERT(str[11] == 'a');

    TEST_ASSERT(str.size() == 12);

    auto cStr = str.c_str();

    TEST_ASSERT(cStr[12] == '\0');

    TEST_ASSERT(str.size() == 12);
}

TEST_CASE(StringSmallInsert)
{
    using namespace Yupei;

    utf8_string str {"abefg"};

    str.insert(2, {"cd"});

    TEST_ASSERT(str.size() == 7);

    for (std::size_t i = 0; i < str.size(); ++i)
        TEST_ASSERT(str[i] == static_cast<char>('a' + i));
}

TEST_CASE(StringPushBackTest)
{
	using namespace Yupei;
	utf8_string str;
	for (int i = 0;i < 1000;++i)
	{
		str.push_back('v');
	}
	TEST_ASSERT(str.size() == 1000);
	for (int i = 0;i < 1000;++i)
	{
		TEST_ASSERT(str[i] == 'v');
	}
}

TEST_CASE(StringBigInsert)
{
    using namespace Yupei;

    utf8_string str;

    for (int i = 0;i < 100;++i)
        str.append("abcd");

    str.insert(0, "abcd");

    for (int i = 0; i < 404; i += 4)
    {
        TEST_ASSERT(str[i] == 'a');
        TEST_ASSERT(str[i + 1] == 'b');
        TEST_ASSERT(str[i + 2] == 'c');
        TEST_ASSERT(str[i + 3] == 'd');
    }
}

TEST_CASE(StringRemove)
{
    using namespace Yupei;

    utf8_string str {"abfcd"};

    str.remove(2, 1);

    TEST_ASSERT(str.size() == 4);

    for (std::size_t i = 0; i < str.size(); ++i)
        TEST_ASSERT(str[i] == static_cast<char>('a' + i));
}

TEST_CASE(Utf32StringTest)
{
	using namespace Yupei;
	const auto& u32Str = U"你好啊，哈哈哈。";	
	const auto yStr = utf32_string { u32Str };
	TEST_ASSERT(std::equal(std::begin(u32Str), std::end(u32Str) - 1, begin(yStr), end(yStr)));
}


TEST_CASE(Utf16DecoderTest)
{
	using namespace Yupei;
	const utf16_string u16Str = u"你好啊，哈哈哈。";
	std::u32string u32Str = U"你好啊，哈哈哈。";
	const auto u32Str2 = to_utf32(u16Str.to_string_view());
	TEST_ASSERT(std::equal(begin(u32Str), end(u32Str), begin(u32Str2), end(u32Str2)));
}

TEST_CASE(Utf8EncoderTest)
{
	using namespace Yupei;
	const utf16_string u16Str = u"你好啊，哈哈哈。";
	const auto& u8Str = u8"你好啊，哈哈哈。";
	const auto u8Str2 = to_utf8(u16Str.to_string_view());
	TEST_ASSERT(std::equal(std::begin(u8Str), std::end(u8Str) - 1, begin(u8Str2), end(u8Str2)));
}