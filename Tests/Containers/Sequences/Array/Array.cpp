#include <Containers/Array.hpp>
#include <catch.hpp>

TEST_CASE("Arrays")
{
	static_assert(std::is_same<Yupei::array<int, 2, 3>, std::array<std::array<int, 3>, 2>>::value, "");
	static_assert(std::is_same<Yupei::array<int, 2>, std::array<int, 2>>::value, "");
}