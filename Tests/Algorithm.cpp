#include <catch.hpp>
#include <Algorithm/ForEach.hpp>
#include <vector>
#include <iostream>

TEST_CASE("For")
{
    For(i, const auto& e, std::vector<int>{ 1, 2, 3, 4 })
    {
        std::cout << i << e;
    };
}