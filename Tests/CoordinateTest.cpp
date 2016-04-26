#include <Coordinate.hpp>
#include <UnitTest.hpp>

TEST_CASE(IndexRankOne)
{
	using namespace Yupei;
	index<1> i { 2 };

	TEST_ASSERT(i[0] == 2);
	++i;
	TEST_ASSERT(i[0] == 3);
	i += 2;
	TEST_ASSERT(i[0] == 5);
	i -= 2;
	TEST_ASSERT(i[0] == 3);
	i *= 3;
	TEST_ASSERT(i[0] == 9);
	i /= 3;
	TEST_ASSERT(i[0] == 3);

	index<1> i2 { 3 };
	TEST_ASSERT(i == i2);
}

TEST_CASE(IndexRankMore)
{
	using namespace Yupei;
	index<3> i { 1,2,3 };

	TEST_ASSERT(i[0] == 1);
	TEST_ASSERT(i[1] == 2);
	TEST_ASSERT(i[2] == 3);

	i *= 2;
	TEST_ASSERT(i[0] == 2);
	TEST_ASSERT(i[1] == 4);
	TEST_ASSERT(i[2] == 6);

	i /= 2;
	TEST_ASSERT(i[0] == 1);
	TEST_ASSERT(i[1] == 2);
	TEST_ASSERT(i[2] == 3);
}

TEST_CASE(BoundRankOne)
{
	using namespace Yupei;
	bounds<1> b { 1 };

	TEST_ASSERT(b[0] == 1);
	TEST_ASSERT(b.size() == 1);
	TEST_ASSERT(*++b.begin() == index<1>{1});

	index<1> i { 2 };
	b += i;
	TEST_ASSERT(b[0] == 3);

	b -= i;
	TEST_ASSERT(b[0] == 1);

	b *= 2;
	TEST_ASSERT(b[0] == 2);

	b /= 2;
	TEST_ASSERT(b[0] == 1);

	auto b2 = 2 * b;
	TEST_ASSERT(b2.size() == 2);
	TEST_ASSERT(b2[0] == 2);

	b2 /= 2;
	TEST_ASSERT(b2 == b);
}

TEST_CASE(BoundRankMore)
{
	using namespace Yupei;

	bounds<3> b { 1, 2, 3 };
	TEST_ASSERT(b[0] == 1);
	TEST_ASSERT(b[1] == 2);
	TEST_ASSERT(b[2] == 3);
	TEST_ASSERT(b.size() == 6);

	auto it = b.begin();
	TEST_ASSERT((*it == index<3>{0, 0, 0}));
	++it;
	TEST_ASSERT(it - b.begin() == 1);
	TEST_ASSERT((*it == index<3>{0, 0, 1}));
	++it;
	TEST_ASSERT((*it == index<3>{0, 0, 2}));
	++it;
	TEST_ASSERT((*it == index<3>{0, 1, 0}));
	++it;
	TEST_ASSERT((*it == index<3>{0, 1, 1}));
	++it;
	TEST_ASSERT((*it == index<3>{0, 1, 2}));
	auto e = b.end();
	TEST_ASSERT(++it == e);
	--it;
	TEST_ASSERT((*it == index<3>{0, 1, 2}));
	TEST_ASSERT(it != e);
	--it;
	TEST_ASSERT((*it == index<3>{0, 1, 1}));
	TEST_ASSERT(it != e);
	--it;
	TEST_ASSERT((*it == index<3>{0, 1, 0}));
	TEST_ASSERT(it != e);
	--it;
	TEST_ASSERT((*it == index<3>{0, 0, 2}));
	TEST_ASSERT(it != e);
	--it;
	TEST_ASSERT((*it == index<3>{0, 0, 1}));
	--it;
	TEST_ASSERT((*it == index<3>{0, 0, 0}));
	TEST_ASSERT(it != e);
	TEST_ASSERT(it == b.begin());
}