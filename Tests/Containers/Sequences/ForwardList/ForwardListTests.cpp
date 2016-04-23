//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\ForwardList.hpp>


#include "..\..\NotConstructible.h"
#include "..\..\Emplaceable.h"
#include "..\..\Copyable.h"
#include <DefaultOnly.h>
#include <MoveOnly.h>
#include <UnitTest.hpp>
#include <Containers\ForwardList.hpp>

TEST_CASE(ForwardListClear)
{
    {
        typedef NotConstructible T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.clear();
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);
    }
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        const T t[] = {0, 1, 2, 3, 4};
        C c(std::begin(t), std::end(t));

        c.clear();
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);

        c.clear();
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);
    }
    
}

TEST_CASE(ForwardListEmplaceAfter)
{
    {
        typedef Emplaceable T;
        typedef Yupei::forward_list<T> C;
        typedef C::iterator I;
        C c;
        I i = c.emplace_after(c.cbefore_begin());
        TEST_ASSERT(i == c.begin());
        TEST_ASSERT(c.front() == Emplaceable());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);

        i = c.emplace_after(c.cbegin(), 1, 2.5);
        TEST_ASSERT(i == std::next(c.begin()));
        TEST_ASSERT(c.front() == Emplaceable());
        TEST_ASSERT(*std::next(c.begin()) == Emplaceable(1, 2.5));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 2);

        i = c.emplace_after(std::next(c.cbegin()), 2, 3.5);
        TEST_ASSERT(i == std::next(c.begin(), 2));
        TEST_ASSERT(c.front() == Emplaceable());
        TEST_ASSERT(*std::next(c.begin()) == Emplaceable(1, 2.5));
        TEST_ASSERT(*std::next(c.begin(), 2) == Emplaceable(2, 3.5));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 3);

        i = c.emplace_after(c.cbegin(), 3, 4.5);
        TEST_ASSERT(i == std::next(c.begin()));
        TEST_ASSERT(c.front() == Emplaceable());
        TEST_ASSERT(*std::next(c.begin(), 1) == Emplaceable(3, 4.5));
        TEST_ASSERT(*std::next(c.begin(), 2) == Emplaceable(1, 2.5));
        TEST_ASSERT(*std::next(c.begin(), 3) == Emplaceable(2, 3.5));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 4);
    }
    
}

TEST_CASE(ForwardListEmplaceFront)
{
    {
        typedef Emplaceable T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.emplace_front();
        TEST_ASSERT(c.front() == Emplaceable());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);
        c.emplace_front(1, 2.5);
        TEST_ASSERT(c.front() == Emplaceable(1, 2.5));
        TEST_ASSERT(*std::next(c.begin()) == Emplaceable());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 2);
    }
    
}

TEST_CASE(ForwardListEraseAfterMany)
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        const T t[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        C c(std::begin(t), std::end(t));

        C::iterator i = c.erase_after(std::next(c.cbefore_begin(), 4), std::next(c.cbefore_begin(), 4));
        TEST_ASSERT(i == std::next(c.cbefore_begin(), 4));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 10);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(*std::next(c.begin(), 3) == 3);
        TEST_ASSERT(*std::next(c.begin(), 4) == 4);
        TEST_ASSERT(*std::next(c.begin(), 5) == 5);
        TEST_ASSERT(*std::next(c.begin(), 6) == 6);
        TEST_ASSERT(*std::next(c.begin(), 7) == 7);
        TEST_ASSERT(*std::next(c.begin(), 8) == 8);
        TEST_ASSERT(*std::next(c.begin(), 9) == 9);

        i = c.erase_after(std::next(c.cbefore_begin(), 2), std::next(c.cbefore_begin(), 5));
        TEST_ASSERT(i == std::next(c.begin(), 2));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 8);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 4);
        TEST_ASSERT(*std::next(c.begin(), 3) == 5);
        TEST_ASSERT(*std::next(c.begin(), 4) == 6);
        TEST_ASSERT(*std::next(c.begin(), 5) == 7);
        TEST_ASSERT(*std::next(c.begin(), 6) == 8);
        TEST_ASSERT(*std::next(c.begin(), 7) == 9);

        i = c.erase_after(std::next(c.cbefore_begin(), 2), std::next(c.cbefore_begin(), 3));
        TEST_ASSERT(i == std::next(c.begin(), 2));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 8);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 4);
        TEST_ASSERT(*std::next(c.begin(), 3) == 5);
        TEST_ASSERT(*std::next(c.begin(), 4) == 6);
        TEST_ASSERT(*std::next(c.begin(), 5) == 7);
        TEST_ASSERT(*std::next(c.begin(), 6) == 8);
        TEST_ASSERT(*std::next(c.begin(), 7) == 9);

        i = c.erase_after(std::next(c.cbefore_begin(), 5), std::next(c.cbefore_begin(), 9));
        TEST_ASSERT(i == c.end());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 5);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 4);
        TEST_ASSERT(*std::next(c.begin(), 3) == 5);
        TEST_ASSERT(*std::next(c.begin(), 4) == 6);

        i = c.erase_after(std::next(c.cbefore_begin(), 0), std::next(c.cbefore_begin(), 2));
        TEST_ASSERT(i == c.begin());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 4);
        TEST_ASSERT(*std::next(c.begin(), 0) == 1);
        TEST_ASSERT(*std::next(c.begin(), 1) == 4);
        TEST_ASSERT(*std::next(c.begin(), 2) == 5);
        TEST_ASSERT(*std::next(c.begin(), 3) == 6);

        i = c.erase_after(std::next(c.cbefore_begin(), 0), std::next(c.cbefore_begin(), 5));
        TEST_ASSERT(i == c.begin());
        TEST_ASSERT(i == c.end());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);
    }
    
}

TEST_CASE(ForwardListInsertAfterConst)
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        typedef C::iterator I;
        C c;
        I i = c.insert_after(c.cbefore_begin(), 0);
        TEST_ASSERT(i == c.begin());
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);

        i = c.insert_after(c.cbegin(), 1);
        TEST_ASSERT(i == std::next(c.begin()));
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(*std::next(c.begin()) == 1);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 2);

        i = c.insert_after(std::next(c.cbegin()), 2);
        TEST_ASSERT(i == std::next(c.begin(), 2));
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(*std::next(c.begin()) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 3);

        i = c.insert_after(c.cbegin(), 3);
        TEST_ASSERT(i == std::next(c.begin()));
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 3);
        TEST_ASSERT(*std::next(c.begin(), 2) == 1);
        TEST_ASSERT(*std::next(c.begin(), 3) == 2);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 4);
    }
}

TEST_CASE(ForwardListInsertAfterIl)
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        typedef C::iterator I;
        C c;
        I i = c.insert_after(c.cbefore_begin(), {});
        TEST_ASSERT(i == c.before_begin());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);

        i = c.insert_after(c.cbefore_begin(), {0, 1, 2});
        TEST_ASSERT(i == std::next(c.before_begin(), 3));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 3);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);

        i = c.insert_after(c.begin(), {3, 4});
        TEST_ASSERT(i == std::next(c.begin(), 2));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 5);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 3);
        TEST_ASSERT(*std::next(c.begin(), 2) == 4);
        TEST_ASSERT(*std::next(c.begin(), 3) == 1);
        TEST_ASSERT(*std::next(c.begin(), 4) == 2);
    }
    
}

TEST_CASE(ForwardListInsertAfterRv)
{
    {
        typedef MoveOnly T;
        typedef Yupei::forward_list<T> C;
        typedef C::iterator I;
        C c;
        I i = c.insert_after(c.cbefore_begin(), 0);
        TEST_ASSERT(i == c.begin());
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);

        i = c.insert_after(c.cbegin(), 1);
        TEST_ASSERT(i == std::next(c.begin()));
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(*std::next(c.begin()) == 1);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 2);

        i = c.insert_after(std::next(c.cbegin()), 2);
        TEST_ASSERT(i == std::next(c.begin(), 2));
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(*std::next(c.begin()) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 3);

        i = c.insert_after(c.cbegin(), 3);
        TEST_ASSERT(i == std::next(c.begin()));
        TEST_ASSERT(c.front() == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 3);
        TEST_ASSERT(*std::next(c.begin(), 2) == 1);
        TEST_ASSERT(*std::next(c.begin(), 3) == 2);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 4);
    }
    
}

TEST_CASE(ForwardListInsertAfterSizeValue)
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        typedef C::iterator I;
        C c;
        I i = c.insert_after(c.cbefore_begin(), 0, 0);
        TEST_ASSERT(i == c.before_begin());
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);

        i = c.insert_after(c.cbefore_begin(), 3, 3);
        TEST_ASSERT(i == std::next(c.before_begin(), 3));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 3);
        TEST_ASSERT(*std::next(c.begin(), 0) == 3);
        TEST_ASSERT(*std::next(c.begin(), 1) == 3);
        TEST_ASSERT(*std::next(c.begin(), 2) == 3);

        i = c.insert_after(c.begin(), 2, 2);
        TEST_ASSERT(i == std::next(c.begin(), 2));
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 5);
        TEST_ASSERT(*std::next(c.begin(), 0) == 3);
        TEST_ASSERT(*std::next(c.begin(), 1) == 2);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(*std::next(c.begin(), 3) == 3);
        TEST_ASSERT(*std::next(c.begin(), 4) == 3);
    }
    
}

TEST_CASE(ForwardListPopFront)
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        typedef Yupei::forward_list<T> C;
        C c;
        c.push_front(1);
        c.push_front(3);
        c.pop_front();
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);
        TEST_ASSERT(c.front() == 1);
        c.pop_front();
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);
    }

    {
        typedef MoveOnly T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.push_front(1);
        c.push_front(3);
        c.pop_front();
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);
        TEST_ASSERT(c.front() == 1);
        c.pop_front();
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);
    }
    
}

TEST_CASE(ForwardListPushFrontConst)
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.push_front(1);
        TEST_ASSERT(c.front() == 1);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);
        c.push_front(3);
        TEST_ASSERT(c.front() == 3);
        TEST_ASSERT(*std::next(c.begin()) == 1);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 2);
    }
    
}

TEST_CASE(ForwardListPushFrontRv)
{
    {
        typedef MoveOnly T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.push_front(1);
        TEST_ASSERT(c.front() == 1);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 1);
        c.push_front(3);
        TEST_ASSERT(c.front() == 3);
        TEST_ASSERT(*std::next(c.begin()) == 1);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 2);
    }
    
}

TEST_CASE(ForwardListResizeSize)
{
    {
        typedef DefaultOnly T;
        typedef Yupei::forward_list<T> C;
        C c;
        c.resize(0);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);
        c.resize(10);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 10);
        c.resize(20);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 20);
        c.resize(5);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 5);
        c.resize(0);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 0);
    }
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        const T t[] = {0, 1, 2, 3, 4};
        C c(std::begin(t), std::end(t));

        c.resize(3);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 3);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);

        c.resize(6);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 6);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(*std::next(c.begin(), 3) == 0);
        TEST_ASSERT(*std::next(c.begin(), 4) == 0);
        TEST_ASSERT(*std::next(c.begin(), 5) == 0);

        c.resize(6);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 6);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(*std::next(c.begin(), 3) == 0);
        TEST_ASSERT(*std::next(c.begin(), 4) == 0);
        TEST_ASSERT(*std::next(c.begin(), 5) == 0);
    }
    
}

TEST_CASE(ForwardListResizeValue)
{
    {
        typedef int T;
        typedef Yupei::forward_list<T> C;
        const T t[] = {0, 1, 2, 3, 4};
        C c(std::begin(t), std::end(t));

        c.resize(3, 10);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 3);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);

        c.resize(6, 10);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 6);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(*std::next(c.begin(), 3) == 10);
        TEST_ASSERT(*std::next(c.begin(), 4) == 10);
        TEST_ASSERT(*std::next(c.begin(), 5) == 10);

        c.resize(6, 12);
        TEST_ASSERT(std::distance(c.begin(), c.end()) == 6);
        TEST_ASSERT(*std::next(c.begin(), 0) == 0);
        TEST_ASSERT(*std::next(c.begin(), 1) == 1);
        TEST_ASSERT(*std::next(c.begin(), 2) == 2);
        TEST_ASSERT(*std::next(c.begin(), 3) == 10);
        TEST_ASSERT(*std::next(c.begin(), 4) == 10);
        TEST_ASSERT(*std::next(c.begin(), 5) == 10);
    }
    
}