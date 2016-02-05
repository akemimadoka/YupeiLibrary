#include "UniquePtr.h"
#include <iostream>
#include <limits>

int main()
{
    using namespace Yupei;
    {
        unique_ptr<int> ptr{new int()};
    }  
    _CrtDumpMemoryLeaks();
}