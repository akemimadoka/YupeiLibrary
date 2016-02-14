//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers/List.hpp>

// iterator insert(const_iterator p, initializer_list<value_type> il);

#include <Containers/List.hpp>
#include <cassert>


int main()
{
    {
        Yupei::list<int> d(10, 1);
        Yupei::list<int>::iterator i = d.insert(std::next(d.cbegin(), 2), {3, 4, 5, 6});
        assert(d.size() == 14);
        assert(i == std::next(d.begin(), 2));
        i = d.begin();
        assert(*i++ == 1);
        assert(*i++ == 1);
        assert(*i++ == 3);
        assert(*i++ == 4);
        assert(*i++ == 5);
        assert(*i++ == 6);
        assert(*i++ == 1);
        assert(*i++ == 1);
        assert(*i++ == 1);
        assert(*i++ == 1);
        assert(*i++ == 1);
        assert(*i++ == 1);
        assert(*i++ == 1);
        assert(*i++ == 1);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
