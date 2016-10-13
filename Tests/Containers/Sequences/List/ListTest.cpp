//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\List.hpp>

#include "..\..\NotConstructible.h"
#include "..\..\Emplaceable.h"
#include "..\..\Copyable.h"
#include <MoveOnly.h>
#include <UnitTest.hpp>
#include <Containers\List.hpp>

//C4822	local class member function does not have a body
#pragma warning(disable : 4822)

TEST_CASE(ListClear)
{
    {
        int a[] = {1, 2, 3};
        Yupei::list<int> c(a, a + 3);
        c.clear();
        CHECK(c.empty());
    }
    
}

TEST_CASE(ListEmplace)
{
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
    {
        Yupei::list<A> c;
        c.emplace(c.cbegin(), 2, 3.5);
        CHECK(c.size() == 1);
        CHECK(c.front().geti() == 2);
        CHECK(c.front().getd() == 3.5);
        c.emplace(c.cend(), 3, 4.5);
        CHECK(c.size() == 2);
        CHECK(c.front().geti() == 2);
        CHECK(c.front().getd() == 3.5);
        CHECK(c.back().geti() == 3);
        CHECK(c.back().getd() == 4.5);
    }
}

TEST_CASE(ListEmplaceBack)
{
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

    {
        Yupei::list<A> c;
        c.emplace_back(2, 3.5);
        CHECK(c.size() == 1);
        CHECK(c.front().geti() == 2);
        CHECK(c.front().getd() == 3.5);
        c.emplace_back(3, 4.5);
        CHECK(c.size() == 2);
        CHECK(c.front().geti() == 2);
        CHECK(c.front().getd() == 3.5);
        CHECK(c.back().geti() == 3);
        CHECK(c.back().getd() == 4.5);
    }
    
}

TEST_CASE(ListEmplaceFront)
{
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

    {
        Yupei::list<A> c;
        c.emplace_front(2, 3.5);
        CHECK(c.size() == 1);
        CHECK(c.front().geti() == 2);
        CHECK(c.front().getd() == 3.5);
        c.emplace_front(3, 4.5);
        CHECK(c.size() == 2);
        CHECK(c.front().geti() == 3);
        CHECK(c.front().getd() == 4.5);
        CHECK(c.back().geti() == 2);
        CHECK(c.back().getd() == 3.5);
    }
}

TEST_CASE(ListEraseIter)
{
    {
        int a1[] = {1, 2, 3};
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::const_iterator i = l1.begin();
        ++i;
        Yupei::list<int>::iterator j = l1.erase(i);
        CHECK(l1.size() == 2);
        CHECK(std::distance(l1.begin(), l1.end()) == 2);
        CHECK(*j == 3);
        CHECK(*l1.begin() == 1);
        CHECK(*std::next(l1.begin()) == 3);
        j = l1.erase(j);
        CHECK(j == l1.end());
        CHECK(l1.size() == 1);
        CHECK(std::distance(l1.begin(), l1.end()) == 1);
        CHECK(*l1.begin() == 1);
        j = l1.erase(l1.begin());
        CHECK(j == l1.end());
        CHECK(l1.size() == 0);
        CHECK(std::distance(l1.begin(), l1.end()) == 0);
    }
}


TEST_CASE(ListEraseIterIter)
{
    int a1[] = {1, 2, 3};
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), l1.cbegin());
        CHECK(l1.size() == 3);
        CHECK(std::distance(l1.cbegin(), l1.cend()) == 3);
        CHECK(i == l1.begin());
    }
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin()));
        CHECK(l1.size() == 2);
        CHECK(std::distance(l1.cbegin(), l1.cend()) == 2);
        CHECK(i == l1.begin());
        CHECK(l1 == Yupei::list<int>(a1 + 1, a1 + 3));
    }
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 2));
        CHECK(l1.size() == 1);
        CHECK(std::distance(l1.cbegin(), l1.cend()) == 1);
        CHECK(i == l1.begin());
        CHECK(l1 == Yupei::list<int>(a1 + 2, a1 + 3));
    }
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 3));
        CHECK(l1.size() == 0);
        CHECK(std::distance(l1.cbegin(), l1.cend()) == 0);
        CHECK(i == l1.begin());
    }
    
}


TEST_CASE(ListInsertIterIl)
{
    {
        Yupei::list<int> d(10, 1);
        Yupei::list<int>::iterator i = d.insert(std::next(d.cbegin(), 2), {3, 4, 5, 6});
        CHECK(d.size() == 14);
        CHECK(i == std::next(d.begin(), 2));
        i = d.begin();
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
        CHECK(*i++ == 3);
        CHECK(*i++ == 4);
        CHECK(*i++ == 5);
        CHECK(*i++ == 6);
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
        CHECK(*i++ == 1);
    }
    
}

TEST_CASE(ListPopBack)
{
    {
        int a[] = {1, 2, 3};
        Yupei::list<int> c(a, a + 3);
        c.pop_back();
        CHECK(c == Yupei::list<int>(a, a + 2));
        c.pop_back();
        CHECK(c == Yupei::list<int>(a, a + 1));
        c.pop_back();
        CHECK(c.empty());

    }
    
}

TEST_CASE(ListPopFront)
{
    {
        int a[] = {1, 2, 3};
        Yupei::list<int> c(a, a + 3);
        c.pop_front();
        CHECK(c == Yupei::list<int>(a + 1, a + 3));
        c.pop_front();
        CHECK(c == Yupei::list<int>(a + 2, a + 3));
        c.pop_front();
        CHECK(c.empty());
    }
}

TEST_CASE(ListPushBack)
{
    {
        Yupei::list<int> c;
        for (int i = 0; i < 5; ++i)
            c.push_back(i);
        int a[] = {0, 1, 2, 3, 4};
        CHECK(c == Yupei::list<int>(a, a + 5));
    }
    
}

TEST_CASE(ListPushFront)
{
    {
        Yupei::list<int> c;
        for (int i = 0; i < 5; ++i)
            c.push_front(i);
        int a[] = {4, 3, 2, 1, 0};
        CHECK(c == Yupei::list<int>(a, a + 5));
    }
    
}