#include <Scope.hpp>
#include <catch.hpp>
#include <cstdio>
#include <stdexcept>

TEST_CASE("Scop")
{
	SECTION("ScopeExit")
	{
		bool flag = false;
		{
			SCOPE_EXIT { flag = true; };
		}
		CHECK(flag == true);
	}

	SECTION("ScopeSuccess")
	{
		bool flag = true;
		try
		{
			SCOPE_SUCCESS { flag = false; };
			throw std::runtime_error { "test" };
		}
		catch (std::runtime_error&)
		{
		}
		CHECK(flag == true);

		flag = false;
		{
			SCOPE_SUCCESS { flag = true; };
		}
		CHECK(flag == true);
	}
	
	SECTION("ScopeFail")
	{
		bool flag = false;
		try
		{
			SCOPE_FAIL { flag = true; };
			throw std::runtime_error { "test" };
		}
		catch (std::runtime_error&)
		{
		}
		CHECK(flag == true);

		{
			SCOPE_FAIL { flag = false; };
		}
		CHECK(flag == true);
	}
}