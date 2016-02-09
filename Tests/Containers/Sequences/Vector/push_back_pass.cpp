#include <Containers\Vector.hpp>
#include <cassert>
#include <crtdbg.h>

int main()
{
    {
        using namespace Yupei;
        vector<int> c;
        c.push_back(0);
        assert(c.size() == 1);
        for (std::size_t i{}; i < c.size(); ++i)
            assert(c[i] == i);
        c.push_back(1);
        assert(c.size() == 2);      
        for (std::size_t j = 0; j < c.size(); ++j)
            assert(c[j] == j);
        c.push_back(2);
        assert(c.size() == 3);       
        for (std::size_t j = 0; j < c.size(); ++j)
            assert(c[j] == j);
        c.push_back(3);
        assert(c.size() == 4);       
        for (std::size_t j = 0; j < c.size(); ++j)
            assert(c[j] == j);
        c.push_back(4);
        assert(c.size() == 5);      
        for (std::size_t j = 0; j < c.size(); ++j)
            assert(c[j] == j);       
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}

