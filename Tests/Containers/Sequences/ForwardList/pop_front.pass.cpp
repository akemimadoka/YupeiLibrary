//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// void pop_front();

#include <Containers\ForwardList.hpp>
#include <cassert>

#include "MoveOnly.h"


int main()
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        typedef Yupei::forward_list<T> C;
        C c;
        c.push_front(1);
        c.push_front(3);
        c.pop_front();
        assert(std::distance(c.begin(), c.end()) == 1);
        assert(c.front() == 1);
        c.pop_front();
        assert(std::distance(c.begin(), c.end()) == 0);
    }

    {
        typedef MoveOnly T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.push_front(1);
        c.push_front(3);
        c.pop_front();
        assert(std::distance(c.begin(), c.end()) == 1);
        assert(c.front() == 1);
        c.pop_front();
        assert(std::distance(c.begin(), c.end()) == 0);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
