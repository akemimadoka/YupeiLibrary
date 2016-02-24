#include <Ranges\Range.hpp>
#include <Containers\Vector.hpp>
#include <iostream>

using namespace Yupei;

int main()
{
    for (auto i : range(5))
        std::cout << i << '\n'; 
    vector<int> vec{1,2,3,4};
    for (auto it : range(begin(vec), end(vec)))
        std::cout << *it << '\n';
    return 0;
}