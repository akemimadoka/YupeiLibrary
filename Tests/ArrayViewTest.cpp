#include <ArrayView.hpp>
#include <UnitTest.hpp>
#include <memory>

TEST_CASE(ArrayViewCtor)
{
    using namespace Yupei;
    char a[3][1][4] { { { 'H', 'i' } } };
    auto av = array_view<char, 3> { a };
    TEST_ASSERT((av.bounds() == bounds<3>{3, 1, 4}));
    TEST_ASSERT((av[{0, 0, 0}] == 'H'));

    const auto parr = std::make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i)
        parr[i] = i;
    auto av2 = array_view<int, 1>(parr.get(), { 5 });
    for (int i = 0; i < 5; ++i)
        TEST_ASSERT(av2[{i}] == i);

    int i = 0;
    for (auto x : av2)
        TEST_ASSERT(x == i++);

	int a3[] = { 1, 2, 3, 4, 5 };
	array_view<int, 1> av3 { a3 };
	auto sav3 = av3.section({ 1 }, { 3 });
}

TEST_CASE(ArrayViewSection)
{
    using namespace Yupei;
    char a[3][1][4] { { { 'H', 'i' } } };

    auto av = array_view<char, 3> { a };
    auto sav = av.section({ 0, 0, 0 }, { 1, 1, 2 });
    TEST_ASSERT(sav.size() == 2);
    TEST_ASSERT((sav[{0, 0, 0}] == 'H'));
    TEST_ASSERT((sav[{0, 0, 1}] == 'i'));

    char a2[] = { 'H', 'i' };
    int i = 0;
    for (auto x : sav)
        TEST_ASSERT(x == a2[i++]);
}