//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// void push_front(value_type&& v);

#include <Containers\ForwardList.hpp>
#include <cassert>

#include "MoveOnly.h"


int main()
{
    {
        typedef MoveOnly T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.push_front(1);
        assert(c.front() == 1);
        assert(std::distance(c.begin(), c.end()) == 1);
        c.push_front(3);
        assert(c.front() == 3);
        assert(*std::next(c.begin()) == 1);
        assert(std::distance(c.begin(), c.end()) == 2);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
