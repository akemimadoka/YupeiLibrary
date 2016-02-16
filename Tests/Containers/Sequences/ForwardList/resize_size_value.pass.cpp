//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// void resize(size_type n, const value_type& v);

#include <Containers\ForwardList.hpp>
#include <cassert>

#include "DefaultOnly.h"


int main()
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        const T t[] = {0, 1, 2, 3, 4};
        C c(std::begin(t), std::end(t));

        c.resize(3, 10);
        assert(std::distance(c.begin(), c.end()) == 3);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 2);

        c.resize(6, 10);
        assert(std::distance(c.begin(), c.end()) == 6);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 2);
        assert(*std::next(c.begin(), 3) == 10);
        assert(*std::next(c.begin(), 4) == 10);
        assert(*std::next(c.begin(), 5) == 10);

        c.resize(6, 12);
        assert(std::distance(c.begin(), c.end()) == 6);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 2);
        assert(*std::next(c.begin(), 3) == 10);
        assert(*std::next(c.begin(), 4) == 10);
        assert(*std::next(c.begin(), 5) == 10);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
