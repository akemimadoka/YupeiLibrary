#include <OS/Windows/Win32Exception.hpp>
#include <catch.hpp>
#include <winerror.h>
#include <iostream>

TEST_CASE("Win32Exception")
{
	using namespace Yupei;
	Win32Exception ex { ERROR_INVALID_OPERATION };
	std::cout << ex.what();
}