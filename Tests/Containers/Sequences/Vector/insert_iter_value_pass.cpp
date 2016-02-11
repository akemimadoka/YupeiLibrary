#define _DEBUG 1

#include <Containers\Vector.hpp>
#include <Assert.hpp>
#include <iostream>
#include <crtdbg.h>

int main() 
{
    {
        Yupei::vector<int> v(100);
        Yupei::vector<int>::iterator i = v.insert(v.cbegin() + 10, 1);
        assert(v.size() == 101);       
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        assert(v[j] == 1);
        for (++j; j < 101; ++j)
            assert(v[j] == 0);
    }
    {
        Yupei::vector<int> v(100);
        while (v.size() < v.capacity()) v.push_back(0); // force reallocation
        size_t sz = v.size();
        Yupei::vector<int>::iterator i = v.insert(v.cbegin() + 10, 1);
        assert(v.size() == sz + 1);        
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        assert(v[j] == 1);
        for (++j; j < v.size(); ++j)
            assert(v[j] == 0);
    }
    {
        Yupei::vector<int> v(100);
        while (v.size() < v.capacity()) v.push_back(0);
        v.pop_back(); v.pop_back(); // force no reallocation
        size_t sz = v.size();
        Yupei::vector<int>::iterator i = v.insert(v.cbegin() + 10, 1);
        assert(v.size() == sz + 1);       
        assert(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            assert(v[j] == 0);
        assert(v[j] == 1);
        for (++j; j < v.size(); ++j)
            assert(v[j] == 0);

    }
}