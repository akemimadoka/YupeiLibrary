#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#include <UnitTest.hpp>

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    UNIT_TEST_NS::GetUnitTest().RunTest();    
    std::getchar();
}