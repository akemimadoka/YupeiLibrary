//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// iterator insert_after(const_iterator p, value_type&& v);

#include <Containers\ForwardList.hpp>
#include <cassert>

#include "MoveOnly.h"


int main()
{
    {
        typedef MoveOnly T;
        typedef Yupei::forward_list<T> C;
        typedef C::iterator I;
        C c;
        I i = c.insert_after(c.cbefore_begin(), 0);
        assert(i == c.begin());
        assert(c.front() == 0);
        assert(c.front() == 0);
        assert(std::distance(c.begin(), c.end()) == 1);

        i = c.insert_after(c.cbegin(), 1);
        assert(i == std::next(c.begin()));
        assert(c.front() == 0);
        assert(*std::next(c.begin()) == 1);
        assert(std::distance(c.begin(), c.end()) == 2);

        i = c.insert_after(std::next(c.cbegin()), 2);
        assert(i == std::next(c.begin(), 2));
        assert(c.front() == 0);
        assert(*std::next(c.begin()) == 1);
        assert(*std::next(c.begin(), 2) == 2);
        assert(std::distance(c.begin(), c.end()) == 3);

        i = c.insert_after(c.cbegin(), 3);
        assert(i == std::next(c.begin()));
        assert(c.front() == 0);
        assert(*std::next(c.begin(), 1) == 3);
        assert(*std::next(c.begin(), 2) == 1);
        assert(*std::next(c.begin(), 3) == 2);
        assert(std::distance(c.begin(), c.end()) == 4);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
