//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>

// iterator erase_after(const_iterator first, const_iterator last);

#include <Containers\ForwardList.hpp>
#include <cassert>



int main()
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        const T t[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        C c(std::begin(t), std::end(t));

        C::iterator i = c.erase_after(std::next(c.cbefore_begin(), 4), std::next(c.cbefore_begin(), 4));
        assert(i == std::next(c.cbefore_begin(), 4));
        assert(std::distance(c.begin(), c.end()) == 10);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 2);
        assert(*std::next(c.begin(), 3) == 3);
        assert(*std::next(c.begin(), 4) == 4);
        assert(*std::next(c.begin(), 5) == 5);
        assert(*std::next(c.begin(), 6) == 6);
        assert(*std::next(c.begin(), 7) == 7);
        assert(*std::next(c.begin(), 8) == 8);
        assert(*std::next(c.begin(), 9) == 9);

        i = c.erase_after(std::next(c.cbefore_begin(), 2), std::next(c.cbefore_begin(), 5));
        assert(i == std::next(c.begin(), 2));
        assert(std::distance(c.begin(), c.end()) == 8);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 4);
        assert(*std::next(c.begin(), 3) == 5);
        assert(*std::next(c.begin(), 4) == 6);
        assert(*std::next(c.begin(), 5) == 7);
        assert(*std::next(c.begin(), 6) == 8);
        assert(*std::next(c.begin(), 7) == 9);

        i = c.erase_after(std::next(c.cbefore_begin(), 2), std::next(c.cbefore_begin(), 3));
        assert(i == std::next(c.begin(), 2));
        assert(std::distance(c.begin(), c.end()) == 8);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 4);
        assert(*std::next(c.begin(), 3) == 5);
        assert(*std::next(c.begin(), 4) == 6);
        assert(*std::next(c.begin(), 5) == 7);
        assert(*std::next(c.begin(), 6) == 8);
        assert(*std::next(c.begin(), 7) == 9);

        i = c.erase_after(std::next(c.cbefore_begin(), 5), std::next(c.cbefore_begin(), 9));
        assert(i == c.end());
        assert(std::distance(c.begin(), c.end()) == 5);
        assert(*std::next(c.begin(), 0) == 0);
        assert(*std::next(c.begin(), 1) == 1);
        assert(*std::next(c.begin(), 2) == 4);
        assert(*std::next(c.begin(), 3) == 5);
        assert(*std::next(c.begin(), 4) == 6);

        i = c.erase_after(std::next(c.cbefore_begin(), 0), std::next(c.cbefore_begin(), 2));
        assert(i == c.begin());
        assert(std::distance(c.begin(), c.end()) == 4);
        assert(*std::next(c.begin(), 0) == 1);
        assert(*std::next(c.begin(), 1) == 4);
        assert(*std::next(c.begin(), 2) == 5);
        assert(*std::next(c.begin(), 3) == 6);

        i = c.erase_after(std::next(c.cbefore_begin(), 0), std::next(c.cbefore_begin(), 5));
        assert(i == c.begin());
        assert(i == c.end());
        assert(std::distance(c.begin(), c.end()) == 0);
    }
    assert(_CrtDumpMemoryLeaks() == 0);
}
