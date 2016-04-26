#include <ArrayView.hpp>
#include <UnitTest.hpp>

TEST_CASE(ArrayViewCtor)
{
	using namespace Yupei;
	char a[3][1][4] { { { 'H', 'i' } } };
	auto av = array_view<char, 3> { a };
	TEST_ASSERT((av.bounds() == bounds<3>{3, 1, 4}));
	TEST_ASSERT((av[{0, 0, 0}] == 'H'));
}