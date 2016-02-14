//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers/List.hpp>

// void clear();

#include <Containers/List.hpp>
#include <cassert>


int main()
{
    {
        int a[] = {1, 2, 3};
        Yupei::list<int> c(a, a + 3);
        c.clear();
        assert(c.empty());
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
