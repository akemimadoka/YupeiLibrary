//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers/List.hpp>

// iterator erase(const_iterator first, const_iterator last);

#include <Containers/List.hpp>
#include <cassert>



int main()
{
    int a1[] = {1, 2, 3};
    {
        Yupei::list<int> l1(a1, a1+3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), l1.cbegin());
        assert(l1.size() == 3);
        assert(std::distance(l1.cbegin(), l1.cend()) == 3);
        assert(i == l1.begin());
    }
    {
        Yupei::list<int> l1(a1, a1+3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin()));
        assert(l1.size() == 2);
        assert(std::distance(l1.cbegin(), l1.cend()) == 2);
        assert(i == l1.begin());
        assert(l1 == Yupei::list<int>(a1+1, a1+3));
    }
    {
        Yupei::list<int> l1(a1, a1+3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 2));
        assert(l1.size() == 1);
        assert(std::distance(l1.cbegin(), l1.cend()) == 1);
        assert(i == l1.begin());
        assert(l1 == Yupei::list<int>(a1+2, a1+3));
    }
    {
        Yupei::list<int> l1(a1, a1+3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 3));
        assert(l1.size() == 0);
        assert(std::distance(l1.cbegin(), l1.cend()) == 0);
        assert(i == l1.begin());
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
