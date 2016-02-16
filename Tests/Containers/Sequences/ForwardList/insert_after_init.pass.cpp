//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// iterator insert_after(const_iterator p, initializer_list<value_type> il);

#include <Containers\ForwardList.hpp>
#include <cassert>


int main()
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        typedef C::iterator I;
        C c;
        I i = c.insert_after(c.cbefore_begin(), {});
        assert(i == c.before_begin());
        assert(std::distance(c.begin(), c.end()) == 0);

        i = c.insert_after(c.cbefore_begin(), {0, 1, 2});
        assert(i == std::next(c.before_begin(), 3));
        assert(std::distance(c.begin(), c.end()) == 3);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 2);

        i = c.insert_after(c.begin(), {3, 4});
        assert(i == std::next(c.begin(), 2));
        assert(std::distance(c.begin(), c.end()) == 5);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 3);
        assert(*std::next(c.begin(), 2) == 4);
        assert(*std::next(c.begin(), 3) == 1);
        assert(*std::next(c.begin(), 4) == 2);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
