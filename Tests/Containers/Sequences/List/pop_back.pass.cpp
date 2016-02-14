//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers/List.hpp>

// void pop_back();

#include <Containers/List.hpp>
#include <cassert>



int main()
{
    {
        int a[] = {1, 2, 3};
        Yupei::list<int> c(a, a + 3);
        c.pop_back();
        assert(c == Yupei::list<int>(a, a + 2));
        c.pop_back();
        assert(c == Yupei::list<int>(a, a + 1));
        c.pop_back();
        assert(c.empty());

    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
