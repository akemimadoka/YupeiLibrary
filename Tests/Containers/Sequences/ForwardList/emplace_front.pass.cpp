//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// template <class... Args> void emplace_front(Args&&... args);

#include <Containers\ForwardList.hpp>
#include <cassert>

#include "..\..\Emplaceable.h"


int main()
{
    {
        typedef Emplaceable T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.emplace_front();
        assert(c.front() == Emplaceable());
        assert(std::distance(c.begin(), c.end()) == 1);
        c.emplace_front(1, 2.5);
        assert(c.front() == Emplaceable(1, 2.5));
        assert(*std::next(c.begin()) == Emplaceable());
        assert(std::distance(c.begin(), c.end()) == 2);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
