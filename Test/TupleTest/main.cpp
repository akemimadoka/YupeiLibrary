#include "..\..\Source\Stl\tuple.h"
#include "..\..\Source\Stl\utility.h"

#include <cassert>
#include <iostream>
#include <string>


using namespace Yupei;

int main()
{
	{
		tuple<int, int, int> a(1, 2, 3);
		tuple<int, int, int> b(1, 2, 3);

		assert(a == b);
		get<0>(b) = 2;
		assert(a < b);

		//get<int>(a); compile error

		auto c = tuple<std::string, int>{ "hahaha",0 };

		assert(get<int>(c) == 0);

		auto c2 = c;

		assert(c2 == c);
	}
	{
		auto foo = []()->tuple<int, int>
		{
			return{ 1,-1 };
		};
	}
	{
		struct D { D(int) {}; D(const D&) = delete; };

		tuple<D> td(12);
	}
	{
		struct Y { explicit Y(int) {}; };

		tuple<Y> ty(12); 
	}
	{
		auto a = make_tuple(1, 2, 3);
		assert(get<0>(a) == 1);
	}

	{
		pair<int, int> a(1, 2);
		pair<int, int> b(1, 2);

		assert(a == b);
		get<0>(b) = 2;
		assert(a < b);

		//get<int>(a); //compile error

		auto c = pair<std::string, int>{ "hahaha",0 };

		assert(get<int>(c) == 0);
	}
	{
		auto foo = []()->pair<int, int>
		{
			return{ 1,-1 };
		};
	}
	{
		struct D { D(int) {}; D(const D&) = delete; };

		pair<D,D> td(12,12);
	}
	{
		struct Y { explicit Y(int) {}; };

		pair<Y,Y> ty(12,12);
	}
	{
		auto a = make_pair(1, 2);
		assert(get<0>(a) == 1);
	}
	{
		class Foo
		{
		public:
			Foo(int,float) {}
		};
		tuple<int, float> t = { 1,2.2f };
		pair<int, Foo> p(piecewise_construct, make_tuple(42), t);

		auto t2 = t;
		assert(t == t2);
	}

	return 0;
}
