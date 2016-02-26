//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//<Containers\Dictionary.hpp>

#include <Containers\Dictionary.hpp>
#include <UnitTest.hpp>
#include <string>

using namespace Yupei;

TEST_CASE(DictionaryClear)
{
    pair<int, std::string> a[] = {
        {1, "one"},
        {2, "two"},
        {3, "three"},
        {4, "four"},
        {1, "four"},
        {2, "four"}
    };

    dictionary<int, std::string> c(a, a + sizeof(a) / sizeof(a[0]));
    c.clear();
    TEST_ASSERT(c.size() == 0);
}

class Moveable
{
    Moveable(const Moveable&);
    Moveable& operator=(const Moveable&);

    int int_;
    double double_;
public:
    Moveable() : int_(0), double_(0) {}
    Moveable(int i, double d) : int_(i), double_(d) {}
    Moveable(Moveable&& x) noexcept
        : int_(x.int_), double_(x.double_)
    {
        x.int_ = -1; x.double_ = -1;
    }
    Moveable& operator=(Moveable&& x)
    {
        int_ = x.int_; x.int_ = -1;
        double_ = x.double_; x.double_ = -1;
        return *this;
    }

    bool operator==(const Moveable& x) const
    {
        return int_ == x.int_ && double_ == x.double_;
    }
    bool operator<(const Moveable& x) const
    {
        return int_ < x.int_ || (int_ == x.int_ && double_ < x.double_);
    }
    size_t hash() const { return std::hash<int>()(int_) + std::hash<double>()(double_); }

    int get() const { return int_; }
    bool moved() const { return int_ == -1; }

    template<typename HashCode>
    friend HashCode hash_value(HashCode hashCode, const Moveable& m)
    {
        return hash_combine(std::move(hashCode), m.int_, m.double_);
    }
};



TEST_CASE(DictionaryTryEmplace)
{
    { // pair<iterator, bool> try_emplace(const key_type& k, Args&&... args);
        typedef Yupei::dictionary<int, Moveable> M;
        typedef Yupei::pair<M::iterator, bool> R;
        M m;
        R r;
        for (int i = 0; i < 20; i += 2)
            m.insert({i, Moveable(i, (double)i)});
        TEST_ASSERT(m.size() == 10);

        Moveable mv1(3, 3.0);
        for (int i = 0; i < 20; i += 2)
        {
            r = m.try_emplace(i, std::move(mv1));
            TEST_ASSERT(m.size() == 10);
            TEST_ASSERT(!r.second);              // was not inserted
            TEST_ASSERT(!mv1.moved());           // was not moved from
            TEST_ASSERT(r.first->first == i);    // key
        }

        r = m.try_emplace(-1, std::move(mv1));
        TEST_ASSERT(m.size() == 11);
        TEST_ASSERT(r.second);                   // was inserted
        TEST_ASSERT(mv1.moved());                // was moved from
        TEST_ASSERT(r.first->first == -1);       // key
        TEST_ASSERT(r.first->second.get() == 3); // value

        Moveable mv2(5, 3.0);
        r = m.try_emplace(5, std::move(mv2));
        TEST_ASSERT(m.size() == 12);
        TEST_ASSERT(r.second);                   // was inserted
        TEST_ASSERT(mv2.moved());                // was moved from
        TEST_ASSERT(r.first->first == 5);        // key
        TEST_ASSERT(r.first->second.get() == 5); // value

        Moveable mv3(-1, 3.0);
        r = m.try_emplace(117, std::move(mv2));
        TEST_ASSERT(m.size() == 13);
        TEST_ASSERT(r.second);                    // was inserted
        TEST_ASSERT(mv2.moved());                 // was moved from
        TEST_ASSERT(r.first->first == 117);       // key
        TEST_ASSERT(r.first->second.get() == -1); // value
    }

    {  // pair<iterator, bool> try_emplace(key_type&& k, Args&&... args);
        typedef Yupei::dictionary<Moveable, Moveable> M;
        typedef Yupei::pair<M::iterator, bool> R;
        M m;
        R r;
        for (int i = 0; i < 20; i += 2)
            m.insert({Moveable(i, (double)i), Moveable(i + 1, (double)i + 1)});
        TEST_ASSERT(m.size() == 10);

        Moveable mvkey1(2, 2.0);
        Moveable mv1(4, 4.0);
        r = m.try_emplace(std::move(mvkey1), std::move(mv1));
        TEST_ASSERT(m.size() == 10);
        TEST_ASSERT(!r.second);                 // was not inserted
        TEST_ASSERT(!mv1.moved());              // was not moved from
        TEST_ASSERT(!mvkey1.moved());           // was not moved from
        TEST_ASSERT(r.first->first == mvkey1);  // key

        Moveable mvkey2(3, 3.0);
        r = m.try_emplace(std::move(mvkey2), std::move(mv1));
        TEST_ASSERT(m.size() == 11);
        TEST_ASSERT(r.second);                   // was inserted
        TEST_ASSERT(mv1.moved());                // was moved from
        TEST_ASSERT(mvkey2.moved());             // was moved from
        TEST_ASSERT(r.first->first.get() == 3); // key
        TEST_ASSERT(r.first->second.get() == 4); // value
    }
}


TEST_CASE(DictionaryErase)
{
    typedef Yupei::dictionary<int, std::string> C;
    typedef Yupei::pair<int, std::string> P;
    P a[] =
    {
        P(1, "one"),
        P(2, "two"),
        P(3, "three"),
        P(4, "four"),
        P(1, "four"),
        P(2, "four"),
    };
    C c(a, a + sizeof(a) / sizeof(a[0]));
    C::const_iterator i = c.find(2);
    c.erase(i);
    TEST_ASSERT(c.size() == 3);
    TEST_ASSERT(c.at(1) == "one");
    TEST_ASSERT(c.at(3) == "three");
    TEST_ASSERT(c.at(4) == "four");
}

TEST_CASE(DictionaryEraseKey)
{
    typedef Yupei::dictionary<int, std::string> C;
    typedef Yupei::pair<int, std::string> P;
    P a[] =
    {
        P(1, "one"),
        P(2, "two"),
        P(3, "three"),
        P(4, "four"),
        P(1, "four"),
        P(2, "four"),
    };
    C c(a, a + sizeof(a) / sizeof(a[0]));
    TEST_ASSERT(c.erase(5) == 0);
    TEST_ASSERT(c.size() == 4);
    TEST_ASSERT(c.at(1) == "one");
    TEST_ASSERT(c.at(2) == "two");
    TEST_ASSERT(c.at(3) == "three");
    TEST_ASSERT(c.at(4) == "four");

    TEST_ASSERT(c.erase(2) == 1);
    TEST_ASSERT(c.size() == 3);
    TEST_ASSERT(c.at(1) == "one");
    TEST_ASSERT(c.at(3) == "three");
    TEST_ASSERT(c.at(4) == "four");

    TEST_ASSERT(c.erase(2) == 0);
    TEST_ASSERT(c.size() == 3);
    TEST_ASSERT(c.at(1) == "one");
    TEST_ASSERT(c.at(3) == "three");
    TEST_ASSERT(c.at(4) == "four");

    TEST_ASSERT(c.erase(4) == 1);
    TEST_ASSERT(c.size() == 2);
    TEST_ASSERT(c.at(1) == "one");
    TEST_ASSERT(c.at(3) == "three");

    TEST_ASSERT(c.erase(4) == 0);
    TEST_ASSERT(c.size() == 2);
    TEST_ASSERT(c.at(1) == "one");
    TEST_ASSERT(c.at(3) == "three");

    TEST_ASSERT(c.erase(1) == 1);
    TEST_ASSERT(c.size() == 1);
    TEST_ASSERT(c.at(3) == "three");

    TEST_ASSERT(c.erase(1) == 0);
    TEST_ASSERT(c.size() == 1);
    TEST_ASSERT(c.at(3) == "three");

    TEST_ASSERT(c.erase(3) == 1);
    TEST_ASSERT(c.size() == 0);

    TEST_ASSERT(c.erase(3) == 0);
    TEST_ASSERT(c.size() == 0);
}

TEST_CASE(DictionaryEraseOrAssign)
{
    { // pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);
        typedef Yupei::dictionary<int, Moveable> M;
        typedef Yupei::pair<M::iterator, bool> R;
        M m;
        R r;
        for (int i = 0; i < 20; i += 2)
            m.insert({i, Moveable(i, (double)i)});
        TEST_ASSERT(m.size() == 10);

        for (int i = 0; i < 20; i += 2)
        {
            Moveable mv(i + 1, i + 1);
            r = m.insert_or_assign(i, std::move(mv));
            TEST_ASSERT(m.size() == 10);
            TEST_ASSERT(!r.second);                    // was not inserted
            TEST_ASSERT(mv.moved());                   // was moved from
            TEST_ASSERT(r.first->first == i);          // key
            TEST_ASSERT(r.first->second.get() == i + 1); // value
        }

        Moveable mv1(5, 5.0);
        r = m.insert_or_assign(-1, std::move(mv1));
        TEST_ASSERT(m.size() == 11);
        TEST_ASSERT(r.second);                    // was inserted
        TEST_ASSERT(mv1.moved());                 // was moved from
        TEST_ASSERT(r.first->first == -1); // key
        TEST_ASSERT(r.first->second.get() == 5);  // value

        Moveable mv2(9, 9.0);
        r = m.insert_or_assign(3, std::move(mv2));
        TEST_ASSERT(m.size() == 12);
        TEST_ASSERT(r.second);                   // was inserted
        TEST_ASSERT(mv2.moved());                // was moved from
        TEST_ASSERT(r.first->first == 3); // key
        TEST_ASSERT(r.first->second.get() == 9); // value

        Moveable mv3(-1, 5.0);
        r = m.insert_or_assign(117, std::move(mv3));
        TEST_ASSERT(m.size() == 13);
        TEST_ASSERT(r.second);                     // was inserted
        TEST_ASSERT(mv3.moved());                  // was moved from
        TEST_ASSERT(r.first->first == 117); // key
        TEST_ASSERT(r.first->second.get() == -1);  // value
    }
    { // pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj);
        typedef Yupei::dictionary<Moveable, Moveable> M;
        typedef Yupei::pair<M::iterator, bool> R;
        M m;
        R r;
        for (int i = 0; i < 20; i += 2)
            m.insert({Moveable(i, (double)i), Moveable(i + 1, (double)i + 1)});
        TEST_ASSERT(m.size() == 10);

        Moveable mvkey1(2, 2.0);
        Moveable mv1(4, 4.0);
        r = m.insert_or_assign(std::move(mvkey1), std::move(mv1));
        TEST_ASSERT(m.size() == 10);
        TEST_ASSERT(!r.second);                  // was not inserted
        TEST_ASSERT(!mvkey1.moved());            // was not moved from
        TEST_ASSERT(mv1.moved());                // was moved from
        TEST_ASSERT(r.first->first == mvkey1);   // key
        TEST_ASSERT(r.first->second.get() == 4); // value

        Moveable mvkey2(3, 3.0);
        Moveable mv2(5, 5.0);
        r = m.try_emplace(std::move(mvkey2), std::move(mv2));
        TEST_ASSERT(m.size() == 11);
        TEST_ASSERT(r.second);                   // was inserted
        TEST_ASSERT(mv2.moved());                // was moved from
        TEST_ASSERT(mvkey2.moved());             // was moved from
        TEST_ASSERT(r.first->first.get() == 3); // key
        TEST_ASSERT(r.first->second.get() == 5); // value
    }
}