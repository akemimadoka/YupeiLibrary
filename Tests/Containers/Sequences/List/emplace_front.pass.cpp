//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers/List.hpp>

// template <class... Args> void emplace_front(Args&&... args);

#include <Containers/List.hpp>
#include <cassert>



class A
{
    int i_;
    double d_;

    A(const A&);
    A& operator=(const A&);
public:
    A(int i, double d)
        : i_(i), d_(d) {}

    int geti() const { return i_; }
    double getd() const { return d_; }
};

int main()
{
    {
        Yupei::list<A> c;
        c.emplace_front(2, 3.5);
        assert(c.size() == 1);
        assert(c.front().geti() == 2);
        assert(c.front().getd() == 3.5);
        c.emplace_front(3, 4.5);
        assert(c.size() == 2);
        assert(c.front().geti() == 3);
        assert(c.front().getd() == 4.5);
        assert(c.back().geti() == 2);
        assert(c.back().getd() == 3.5);
    }
}
