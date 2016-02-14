//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers/List.hpp>

// iterator erase(const_iterator position);

#include <Containers/List.hpp>
#include <cassert>
#include <iterator>

int main()
{
    {
        int a1[] = {1, 2, 3};
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::const_iterator i = l1.begin();
        ++i;
        Yupei::list<int>::iterator j = l1.erase(i);
        assert(l1.size() == 2);
        assert(std::distance(l1.begin(), l1.end()) == 2);
        assert(*j == 3);
        assert(*l1.begin() == 1);
        assert(*std::next(l1.begin()) == 3);
        j = l1.erase(j);
        assert(j == l1.end());
        assert(l1.size() == 1);
        assert(std::distance(l1.begin(), l1.end()) == 1);
        assert(*l1.begin() == 1);
        j = l1.erase(l1.begin());
        assert(j == l1.end());
        assert(l1.size() == 0);
        assert(std::distance(l1.begin(), l1.end()) == 0);
    }
}
