#include "..\..\Source\Stl\tuple.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>

using namespace std::string_literals;

#pragma warning(disable:4101)
#pragma warning(disable:4091)
#pragma warning(disable:4189)

template <class F, class Tuple, std::size_t... I>
constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, Yupei::index_sequence<I...>)
{
	return Yupei::invoke(Yupei::forward<F>(f), Yupei::get<I>(Yupei::forward<Tuple>(t))...);
	// Note: std::invoke is a C++17 feature
}
template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t)
{
	return apply_impl(Yupei::forward<F>(f), Yupei::forward<Tuple>(t),
		Yupei::make_index_sequence < Yupei::tuple_size<Yupei::decay_t<Tuple>>::value > {});
}
void foo(std::unique_ptr<int> p )
{
	std::cout << *p << std::endl;
	std::system("pause");
	//static_assert(Yupei::is_lvalue_reference<decltype(p.first)>::value, "haha");
}

using namespace Yupei;

int main()
{
	is_copy_constructible<int>::value;
	is_convertible<const int&, int>::value;
	tuple<int,int,int> a(1,2,3);
	a.tuple_size;
	//auto t2 = t1;
	//Yupei::tuple<int, char, float, int, int> t4{ 2,1,3,4,5 };
	//swap(t1, t4);
	//int a;
	//char b;
	//Yupei::tuple<std::unique_ptr<int>> tu;
	//Yupei::tuple<std::unique_ptr<int>> tu2;
	//tu = Yupei::move(tu);
	//Yupei::tuple<int, char,std::string> t5{ 2,3,"hahah" };
	//Yupei::tie(a, b, Yupei::ignore)= Yupei::move(t5);
	//std::cout << (t1 <= t4) << std::endl;
	//auto x = Yupei::get<3>(t1);
	//Yupei::tuple<std::vector<int>, std::vector<int>> t3{ Yupei::allocator_arg,std::allocator<int>{} };
	//Yupei::get<0>(t3).push_back(3);
	//Yupei::reference_wrapper<int> li(a);
	//auto t6 = Yupei::make_tuple(li, b);
	//Yupei::is_trivially_copyable<Yupei::reference_wrapper<int>>::value;
	//auto f = Yupei::get<char>(t1);
	//Yupei::pair<int, std::string> p;
	//Yupei::tuple<int, std::string> t8(p);
	//t8 = p;
	//auto py = Yupei::get<int>(p);
	//auto px = Yupei::get<0>(p);
	//p = Yupei::make_pair(2, 3);
	////apply(foo, Yupei::make_tuple(2, "hahaha"s));
	//auto ptr = std::make_unique<int>();
	//auto footup = Yupei::make_tuple(std::move(ptr));
	//apply(foo, std::move(footup));

	//Yupei::pair<std::string, int> sip{ 
	//	Yupei::piecewise_construct, 
	//	Yupei::make_tuple(3,'c'),
	//	Yupei::make_tuple(1) };

	return 0;
}