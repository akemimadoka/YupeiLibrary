#include <OS\Windows\Win32Exception.hpp>
#include <UnitTest.hpp>
#include <winerror.h>
#include <iostream>

TEST_CASE(Win32ExceptionTest)
{
	using namespace Yupei;
	Win32Exception ex { ERROR_INVALID_OPERATION };
	std::cout << ex.what();
}