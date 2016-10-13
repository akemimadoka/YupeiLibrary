//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers/Vector.hpp>

#include "../../NotConstructible.h"
#include "../../Emplaceable.h"
#include "../../Copyable.h"
#include <MoveOnly.h>
#include <catch.hpp>
#include <Containers/Vector.hpp>

TEST_CASE("vector")
{
	using Yupei::vector;
	SECTION("insert without reallocation")
	{
		vector<int> v(100);
		auto i = v.insert(v.cbegin() + 10, 1);
		CHECK(v.size() == 101);
		CHECK(i == v.begin() + 10);
		int j;
		for (j = 0; j < 10; ++j)
			CHECK(v[j] == 0);
		CHECK(v[j] == 1);
		for (++j; j < 101; ++j)
			CHECK(v[j] == 0);
	}

	SECTION("insert with reallocation")
	{
		vector<int> v(100);
		while (v.size() < v.capacity()) v.push_back(0); // force reallocation
		auto sz = v.size();
		auto i = v.insert(v.cbegin() + 10, 1);
		CHECK(v.size() == sz + 1);
		CHECK(i == v.begin() + 10);
		std::size_t j;
		for (j = 0; j < 10; ++j)
			CHECK(v[j] == 0);
		CHECK(v[j] == 1);
		for (++j; j < v.size(); ++j)
			CHECK(v[j] == 0);
	}

	SECTION("push_back")
	{
		vector<int> c;
		c.push_back(0);
		CHECK(c.size() == 1);
		for (std::size_t i {}; i < c.size(); ++i)
			CHECK(c[i] == i);
		c.push_back(1);
		CHECK(c.size() == 2);
		for (std::size_t j = 0; j < c.size(); ++j)
			CHECK(c[j] == j);
		c.push_back(2);
		CHECK(c.size() == 3);
		for (std::size_t j = 0; j < c.size(); ++j)
			CHECK(c[j] == j);
		c.push_back(3);
		CHECK(c.size() == 4);
		for (std::size_t j = 0; j < c.size(); ++j)
			CHECK(c[j] == j);
		c.push_back(4);
		CHECK(c.size() == 5);
		for (std::size_t j = 0; j < c.size(); ++j)
			CHECK(c[j] == j);
	}
}