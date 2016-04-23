#include <TypeTraits.hpp>
#include <UnitTest.hpp>

TEST_CASE(ConjunctionTest)
{
	using namespace Yupei;
	static_assert(conjunction<std::true_type, std::true_type, std::false_type>::value == false, "");
	static_assert(conjunction<std::true_type, std::true_type, std::true_type>::value == true, "");
}

TEST_CASE(DisjunctionTest)
{
	using namespace Yupei;
	static_assert(disjunction<std::true_type, std::true_type, std::false_type>::value == true, "");
	static_assert(disjunction<std::true_type, std::true_type, std::true_type>::value == true, "");
	static_assert(disjunction<std::false_type, std::false_type, std::false_type>::value == false, "");
}
