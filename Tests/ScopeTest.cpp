#include <Scope.hpp>
#include <UnitTest.hpp>
#include <cstdio>


TEST_CASE(ScopeExitTest)
{
	SCOPE_EXIT { std::printf("%s", "hahah\n"); };
	SCOPE_FAIL { std::printf("%s", "failed!\n"); };
	SCOPE_SUCCESS { std::printf("%s", "success!\n"); };
}