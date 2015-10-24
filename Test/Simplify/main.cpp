
#include "..\..\Source\Stl\type_traits.h"
#include <iostream>
#include <typeinfo>
#include <memory>

#pragma warning(disable:4101)
#pragma warning(disable:4091)
#pragma warning(disable:4189)

using namespace Yupei;

struct test
{
	int i = 0;
	int foo(int) { return 0; }
	typedef int value_type;
};
void bar(int) {}

template<typename Type>
void uni(Type&& t)
{
	//static_assert(is_lvalue_reference<Type>::value || is_rvalue_reference<Type>::value, "hahhha");
	//printf("%s", typeid(t).name());
}

struct S {
	double operator()(char, int&);
	float operator()(int) { return 1.0; }
};

int main()
{
	//typedef Yupei::allocator_traits<test>::pointer pointer;
	//auto x = Yupei::remove_const<const int>::type{};
	Yupei::is_arithmetic<float>::value; //true
	//pointer p;
	is_one_of<int, long, char, Yupei::true_type> ::value; //false
	is_array<int>::value; //false
	is_member_function_pointer<decltype(&test::foo)>::value; //true
	//is_function<decltype(std::printf)>::value; //true
	is_enum<int>::value; //false
	is_fundamental<nullptr_t>::value; //true
	is_object<decltype(bar)>::value; //false
	is_pointer<int*>::value; //true
	is_const<const int*>::value; //false
	//is_assignable<std::unique_ptr<int>&, std::unique_ptr<int>&&>::value;
	is_const<const volatile int>::value; // true
	is_const<const int*>::value; // false
	is_const<const int&>::value;// false
	is_const<int[3]>::value; // false
	is_const<const int[3]>::value; // true

	//Yupei::remove_const
	//Yupei::re
	std::cout << typeid(remove_const_t<const volatile int>).name();// volatile int
	Yupei::remove_const<const int* const>::type;// const int*
	remove_const<const int&>::type; // const int&
	remove_const<const int[3]>::type; // int[3]
	rank<int[][1][2][3]>::value;
	extent<const int[][1][2][3], 2>::value;
	make_signed<unsigned int>::type i;
	is_signed<make_signed<unsigned long long>::type>::value;
	sizeof(make_signed<unsigned long long>::type);
	remove_pointer<int *const>::type;
	remove_cv_t<int*const>;
	test t;
	//(*(&t)).foo();
	is_member_function_pointer<decay_t<decltype(&test::foo)>>::value;
	is_member_object_pointer<decay_t<decltype(&test::foo)>>::value;
	is_member_object_pointer<decltype(&test::i)>::value;
	is_member_function_pointer<decltype(&test::i)>::value;
	Yupei::result_of_t<S(int)>{};
	invoke(S(), 1);
	static_assert(sizeof(std::aligned_storage_t<5>) ==
		sizeof(Yupei::aligned_storage_t<5>), "error");
	//constexpr auto m = static_max(2, 3, 4, 5, 6, 7, 9);
	//Yupei::aligned_union<12, int, char, float, double>::alignment_value; //8
	//constexpr auto y = sizeof(Yupei::aligned_union_t<12, int, char, float, double>);
	//static_assert(sizeof(Yupei::aligned_union_t<12, int, char, float, double>)
	//	== sizeof(std::aligned_union_t<12, int, char, float, double>), "error");
	
	auto ptr = std::make_shared<test>();
	auto res = Yupei::invoke(&test::foo,ptr, 1);
	return 0;
}