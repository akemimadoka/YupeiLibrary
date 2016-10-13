//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//<Containers/Dictionary.hpp>

#include <Containers/Dictionary.hpp>
#include <catch.hpp>
#include <string>
#include <utility>

namespace 
{
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
		friend void hash_value(HashCode& hashCode, const Moveable& m)
		{
			hash_combine(hashCode, m.int_, m.double_);
		}
	};

}

TEST_CASE("Dictionary")
{
	using namespace Yupei;
	
	SECTION("operator[] and insert")
	{	
		std::pair<int, std::string> elements[] {
			{ 1, "one" },
			{ 2, "two" },
			{ 3, "three" },
			{ 4, "four" } };

		dictionary<int, std::string> dict;
		for (const auto& v : elements)
			dict.insert(v);

		CHECK(dict[1] == "one");
		CHECK(dict[2] == "two");
		CHECK(dict[3] == "three");
		CHECK(dict[4] == "four");
	}

	SECTION("try_emplace(const key_type& k, Args&&... args)")
	{
		dictionary<int, Moveable> m;

		for (int i = 0; i < 20; i += 2)
			m.insert({ i, Moveable(i, (double)i) });
		CHECK(m.size() == 10);

		Moveable mv1(3, 3.0);
		for (int i = 0; i < 20; i += 2)
		{
			const auto r = m.try_emplace(i, std::move(mv1));
			CHECK(m.size() == 10);
			CHECK(!r.second);              // was not inserted
			CHECK(!mv1.moved());           // was not moved from
			CHECK(r.first->first == i);    // key
		}

		auto r = m.try_emplace(-1, std::move(mv1));
		CHECK(m.size() == 11);
		CHECK(r.second);                   // was inserted
		CHECK(mv1.moved());                // was moved from
		CHECK(r.first->first == -1);       // key
		CHECK(r.first->second.get() == 3); // value

		Moveable mv2(5, 3.0);
		r = m.try_emplace(5, std::move(mv2));
		CHECK(m.size() == 12);
		CHECK(r.second);                   // was inserted
		CHECK(mv2.moved());                // was moved from
		CHECK(r.first->first == 5);        // key
		CHECK(r.first->second.get() == 5); // value

		Moveable mv3(-1, 3.0);
		r = m.try_emplace(117, std::move(mv2));
		CHECK(m.size() == 13);
		CHECK(r.second);                    // was inserted
		CHECK(mv2.moved());                 // was moved from
		CHECK(r.first->first == 117);       // key
		CHECK(r.first->second.get() == -1); // value
	}

	SECTION("try_emplace(key_type&& k, Args&&... args)")
	{
		dictionary<Moveable, Moveable> m;
		for (int i = 0; i < 20; i += 2)
			m.insert({ Moveable(i, (double)i), Moveable(i + 1, (double)i + 1) });
		CHECK(m.size() == 10);

		Moveable mvkey1(2, 2.0);
		Moveable mv1(4, 4.0);
		auto r = m.try_emplace(std::move(mvkey1), std::move(mv1));
		CHECK(m.size() == 10);
		CHECK(!r.second);                 // was not inserted
		CHECK(!mv1.moved());              // was not moved from
		CHECK(!mvkey1.moved());           // was not moved from
		CHECK(r.first->first == mvkey1);  // key

		Moveable mvkey2(3, 3.0);
		r = m.try_emplace(std::move(mvkey2), std::move(mv1));
		CHECK(m.size() == 11);
		CHECK(r.second);                   // was inserted
		CHECK(mv1.moved());                // was moved from
		CHECK(mvkey2.moved());             // was moved from
		CHECK(r.first->first.get() == 3); // key
		CHECK(r.first->second.get() == 4); // value
	}

	SECTION("iterator erase(const_iterator pos)")
	{
		std::pair<int, std::string> a[] =
		{
			{1, "one"},
			{2, "two"},
			{3, "three"},
			{4, "four"}
		};

		dictionary<int, std::string> c(a, a + sizeof(a) / sizeof(a[0]));
		const auto i = c.find(2);
		c.erase(i);
		CHECK(c.size() == 3);
		CHECK(c.at(1) == "one");
		CHECK(c.at(3) == "three");
		CHECK(c.at(4) == "four");
	}

	SECTION("bool erase(const key_type& key)")
	{
		std::pair<int, std::string> a[] =
		{
			{ 1, "one" },
			{ 2, "two" },
			{ 3, "three" },
			{ 4, "four" }
		};

		dictionary<int, std::string> c(a, a + sizeof(a) / sizeof(a[0]));

		CHECK(c.erase(5) == false);
		CHECK(c.size() == 4);
		CHECK(c.at(1) == "one");
		CHECK(c.at(2) == "two");
		CHECK(c.at(3) == "three");
		CHECK(c.at(4) == "four");

		CHECK(c.erase(2) == 1);
		CHECK(c.size() == 3);
		CHECK(c.at(1) == "one");
		CHECK(c.at(3) == "three");
		CHECK(c.at(4) == "four");

		CHECK(c.erase(2) == 0);
		CHECK(c.size() == 3);
		CHECK(c.at(1) == "one");
		CHECK(c.at(3) == "three");
		CHECK(c.at(4) == "four");

		CHECK(c.erase(4) == 1);
		CHECK(c.size() == 2);
		CHECK(c.at(1) == "one");
		CHECK(c.at(3) == "three");

		CHECK(c.erase(4) == 0);
		CHECK(c.size() == 2);
		CHECK(c.at(1) == "one");
		CHECK(c.at(3) == "three");

		CHECK(c.erase(1) == 1);
		CHECK(c.size() == 1);
		CHECK(c.at(3) == "three");

		CHECK(c.erase(1) == 0);
		CHECK(c.size() == 1);
		CHECK(c.at(3) == "three");

		CHECK(c.erase(3) == 1);
		CHECK(c.size() == 0);

		CHECK(c.erase(3) == 0);
		CHECK(c.size() == 0);
	}

	SECTION(R"(template<typename M> std::pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj))")
	{	

		dictionary<int, Moveable> m;

		for (int i = 0; i < 20; i += 2)
			m.insert({ i, Moveable(i, (double)i) });
		CHECK(m.size() == 10);

		for (int i = 0; i < 20; i += 2)
		{
			Moveable mv(i + 1, i + 1);
			const auto r = m.insert_or_assign(i, std::move(mv));
			CHECK(m.size() == 10);
			CHECK(!r.second);                    // was not inserted
			CHECK(mv.moved());                   // was moved from
			CHECK(r.first->first == i);          // key
			CHECK(r.first->second.get() == i + 1); // value
		}

		Moveable mv1(5, 5.0);
		auto r = m.insert_or_assign(-1, std::move(mv1));
		CHECK(m.size() == 11);
		CHECK(r.second);                    // was inserted
		CHECK(mv1.moved());                 // was moved from
		CHECK(r.first->first == -1); // key
		CHECK(r.first->second.get() == 5);  // value

		Moveable mv2(9, 9.0);
		r = m.insert_or_assign(3, std::move(mv2));
		CHECK(m.size() == 12);
		CHECK(r.second);                   // was inserted
		CHECK(mv2.moved());                // was moved from
		CHECK(r.first->first == 3); // key
		CHECK(r.first->second.get() == 9); // value

		Moveable mv3(-1, 5.0);
		r = m.insert_or_assign(117, std::move(mv3));
		CHECK(m.size() == 13);
		CHECK(r.second);                     // was inserted
		CHECK(mv3.moved());                  // was moved from
		CHECK(r.first->first == 117); // key
		CHECK(r.first->second.get() == -1);  // value
	}

	SECTION(R"(template<typename M> std::pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj))")
	{
		dictionary<Moveable, Moveable> m;
		for (int i = 0; i < 20; i += 2)
			m.insert({ Moveable(i, (double)i), Moveable(i + 1, (double)i + 1) });
		CHECK(m.size() == 10);

		Moveable mvkey1(2, 2.0);
		Moveable mv1(4, 4.0);
		auto r = m.insert_or_assign(std::move(mvkey1), std::move(mv1));
		CHECK(m.size() == 10);
		CHECK(!r.second);                  // was not inserted
		CHECK(!mvkey1.moved());            // was not moved from
		CHECK(mv1.moved());                // was moved from
		CHECK(r.first->first == mvkey1);   // key
		CHECK(r.first->second.get() == 4); // value

		Moveable mvkey2(3, 3.0);
		Moveable mv2(5, 5.0);
		r = m.try_emplace(std::move(mvkey2), std::move(mv2));
		CHECK(m.size() == 11);
		CHECK(r.second);                   // was inserted
		CHECK(mv2.moved());                // was moved from
		CHECK(mvkey2.moved());             // was moved from
		CHECK(r.first->first.get() == 3); // key
		CHECK(r.first->second.get() == 5); // value
	}
}
