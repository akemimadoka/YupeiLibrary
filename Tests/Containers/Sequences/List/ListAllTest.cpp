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
        TEST_ASSERT(c.empty());
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
        TEST_ASSERT(c.size() == 1);
        TEST_ASSERT(c.front().geti() == 2);
        TEST_ASSERT(c.front().getd() == 3.5);
        c.emplace(c.cend(), 3, 4.5);
        TEST_ASSERT(c.size() == 2);
        TEST_ASSERT(c.front().geti() == 2);
        TEST_ASSERT(c.front().getd() == 3.5);
        TEST_ASSERT(c.back().geti() == 3);
        TEST_ASSERT(c.back().getd() == 4.5);
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
        TEST_ASSERT(c.size() == 1);
        TEST_ASSERT(c.front().geti() == 2);
        TEST_ASSERT(c.front().getd() == 3.5);
        c.emplace_back(3, 4.5);
        TEST_ASSERT(c.size() == 2);
        TEST_ASSERT(c.front().geti() == 2);
        TEST_ASSERT(c.front().getd() == 3.5);
        TEST_ASSERT(c.back().geti() == 3);
        TEST_ASSERT(c.back().getd() == 4.5);
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
        TEST_ASSERT(c.size() == 1);
        TEST_ASSERT(c.front().geti() == 2);
        TEST_ASSERT(c.front().getd() == 3.5);
        c.emplace_front(3, 4.5);
        TEST_ASSERT(c.size() == 2);
        TEST_ASSERT(c.front().geti() == 3);
        TEST_ASSERT(c.front().getd() == 4.5);
        TEST_ASSERT(c.back().geti() == 2);
        TEST_ASSERT(c.back().getd() == 3.5);
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
        TEST_ASSERT(l1.size() == 2);
        TEST_ASSERT(std::distance(l1.begin(), l1.end()) == 2);
        TEST_ASSERT(*j == 3);
        TEST_ASSERT(*l1.begin() == 1);
        TEST_ASSERT(*std::next(l1.begin()) == 3);
        j = l1.erase(j);
        TEST_ASSERT(j == l1.end());
        TEST_ASSERT(l1.size() == 1);
        TEST_ASSERT(std::distance(l1.begin(), l1.end()) == 1);
        TEST_ASSERT(*l1.begin() == 1);
        j = l1.erase(l1.begin());
        TEST_ASSERT(j == l1.end());
        TEST_ASSERT(l1.size() == 0);
        TEST_ASSERT(std::distance(l1.begin(), l1.end()) == 0);
    }
}


TEST_CASE(ListEraseIterIter)
{
    int a1[] = {1, 2, 3};
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), l1.cbegin());
        TEST_ASSERT(l1.size() == 3);
        TEST_ASSERT(std::distance(l1.cbegin(), l1.cend()) == 3);
        TEST_ASSERT(i == l1.begin());
    }
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin()));
        TEST_ASSERT(l1.size() == 2);
        TEST_ASSERT(std::distance(l1.cbegin(), l1.cend()) == 2);
        TEST_ASSERT(i == l1.begin());
        TEST_ASSERT(l1 == Yupei::list<int>(a1 + 1, a1 + 3));
    }
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 2));
        TEST_ASSERT(l1.size() == 1);
        TEST_ASSERT(std::distance(l1.cbegin(), l1.cend()) == 1);
        TEST_ASSERT(i == l1.begin());
        TEST_ASSERT(l1 == Yupei::list<int>(a1 + 2, a1 + 3));
    }
    {
        Yupei::list<int> l1(a1, a1 + 3);
        Yupei::list<int>::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 3));
        TEST_ASSERT(l1.size() == 0);
        TEST_ASSERT(std::distance(l1.cbegin(), l1.cend()) == 0);
        TEST_ASSERT(i == l1.begin());
    }
    
}


TEST_CASE(ListInsertIterIl)
{
    {
        Yupei::list<int> d(10, 1);
        Yupei::list<int>::iterator i = d.insert(std::next(d.cbegin(), 2), {3, 4, 5, 6});
        TEST_ASSERT(d.size() == 14);
        TEST_ASSERT(i == std::next(d.begin(), 2));
        i = d.begin();
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 3);
        TEST_ASSERT(*i++ == 4);
        TEST_ASSERT(*i++ == 5);
        TEST_ASSERT(*i++ == 6);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
        TEST_ASSERT(*i++ == 1);
    }
    
}

TEST_CASE(ListPopBack)
{
    {
        int a[] = {1, 2, 3};
        Yupei::list<int> c(a, a + 3);
        c.pop_back();
        TEST_ASSERT(c == Yupei::list<int>(a, a + 2));
        c.pop_back();
        TEST_ASSERT(c == Yupei::list<int>(a, a + 1));
        c.pop_back();
        TEST_ASSERT(c.empty());

    }
    
}

TEST_CASE(ListPopFront)
{
    {
        int a[] = {1, 2, 3};
        Yupei::list<int> c(a, a + 3);
        c.pop_front();
        TEST_ASSERT(c == Yupei::list<int>(a + 1, a + 3));
        c.pop_front();
        TEST_ASSERT(c == Yupei::list<int>(a + 2, a + 3));
        c.pop_front();
        TEST_ASSERT(c.empty());
    }
}

TEST_CASE(ListPushBack)
{
    {
        Yupei::list<int> c;
        for (int i = 0; i < 5; ++i)
            c.push_back(i);
        int a[] = {0, 1, 2, 3, 4};
        TEST_ASSERT(c == Yupei::list<int>(a, a + 5));
    }
    
}

TEST_CASE(ListPushFront)
{
    {
        Yupei::list<int> c;
        for (int i = 0; i < 5; ++i)
            c.push_front(i);
        int a[] = {4, 3, 2, 1, 0};
        TEST_ASSERT(c == Yupei::list<int>(a, a + 5));
    }
    
}