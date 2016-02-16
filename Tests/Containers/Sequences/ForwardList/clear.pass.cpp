//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// void clear();

#include <Containers\ForwardList.hpp>
#include <cassert>

#include "..\..\NotConstructible.h"

int main()
{
    {
        typedef NotConstructible T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.clear();
        assert(std::distance(c.begin(), c.end()) == 0);
    }
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        const T t[] = {0, 1, 2, 3, 4};
        C c(std::begin(t), std::end(t));

        c.clear();
        assert(std::distance(c.begin(), c.end()) == 0);

        c.clear();
        assert(std::distance(c.begin(), c.end()) == 0);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
