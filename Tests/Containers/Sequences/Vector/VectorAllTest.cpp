//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <Containers\Vector.hpp>

#include "..\..\NotConstructible.h"
#include "..\..\Emplaceable.h"
#include "..\..\Copyable.h"
#include <DefaultOnly.h>
#include <MoveOnly.h>
#include <UnitTest.hpp>
#include <Containers\Vector.hpp>

TEST_CASE(VectorInsertIterValue)
{
    {
        Yupei::vector<int> v(100);
        Yupei::vector<int>::iterator i = v.insert(v.cbegin() + 10, 1);
        TEST_ASSERT(v.size() == 101);
        TEST_ASSERT(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            TEST_ASSERT(v[j] == 0);
        TEST_ASSERT(v[j] == 1);
        for (++j; j < 101; ++j)
            TEST_ASSERT(v[j] == 0);
    }
    {
        Yupei::vector<int> v(100);
        while (v.size() < v.capacity()) v.push_back(0); // force reallocation
        size_t sz = v.size();
        Yupei::vector<int>::iterator i = v.insert(v.cbegin() + 10, 1);
        TEST_ASSERT(v.size() == sz + 1);
        TEST_ASSERT(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            TEST_ASSERT(v[j] == 0);
        TEST_ASSERT(v[j] == 1);
        for (++j; j < v.size(); ++j)
            TEST_ASSERT(v[j] == 0);
    }
    {
        Yupei::vector<int> v(100);
        while (v.size() < v.capacity()) v.push_back(0);
        v.pop_back(); v.pop_back(); // force no reallocation
        size_t sz = v.size();
        Yupei::vector<int>::iterator i = v.insert(v.cbegin() + 10, 1);
        TEST_ASSERT(v.size() == sz + 1);
        TEST_ASSERT(i == v.begin() + 10);
        int j;
        for (j = 0; j < 10; ++j)
            TEST_ASSERT(v[j] == 0);
        TEST_ASSERT(v[j] == 1);
        for (++j; j < v.size(); ++j)
            TEST_ASSERT(v[j] == 0);

    }
    
}

TEST_CASE(VectorPushBack)
{
    {
        using namespace Yupei;
        vector<int> c;
        c.push_back(0);
        TEST_ASSERT(c.size() == 1);
        for (std::size_t i {}; i < c.size(); ++i)
            TEST_ASSERT(c[i] == i);
        c.push_back(1);
        TEST_ASSERT(c.size() == 2);
        for (std::size_t j = 0; j < c.size(); ++j)
            TEST_ASSERT(c[j] == j);
        c.push_back(2);
        TEST_ASSERT(c.size() == 3);
        for (std::size_t j = 0; j < c.size(); ++j)
            TEST_ASSERT(c[j] == j);
        c.push_back(3);
        TEST_ASSERT(c.size() == 4);
        for (std::size_t j = 0; j < c.size(); ++j)
            TEST_ASSERT(c[j] == j);
        c.push_back(4);
        TEST_ASSERT(c.size() == 5);
        for (std::size_t j = 0; j < c.size(); ++j)
            TEST_ASSERT(c[j] == j);
    }
    
}

