#include "..\..\Source\Stl\algorithm.h"
#include <iostream>
#include <vector>
#include <random>
using namespace Yupei;

template <class Container, class Size, class T>
bool consecutive_values(const Container& c, Size count, const T& v)
{
	return search_n(std::begin(c), std::end(c), count, v) != std::end(c);
}

#define RANGE(container) std::begin(container),std::end(container)

auto PrintLine = [](){
	std::cout << std::endl;
};

std::random_device rd;
std::mt19937 mt{ rd() };
std::uniform_int_distribution<int> dist{ 0,10000 };

auto get_random_number = []()
{
	return dist(mt);
};

auto get_random_vector = [](std::size_t count = 1000)
{
	std::vector<int> v;
	v.reserve(count);
	for (std::size_t i = 0; i < count;++i)
	{
		v.push_back(get_random_number());
	}
	return v;
};


int main()
{
	{
		auto lst = { 2,4,1,2,4,100,2,4,3 };
		auto pr = minmax(lst);
		auto mv = min(lst);
		auto maxv = max(lst);
		constexpr auto x = static_max(1, 3.0, 2, 1, 3, 2, 9, 5, 3);
		std::cout << pr.first << " " << pr.second << '\n';
	}

	{
		std::vector<int> v{ 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4 };
		std::vector<int>::iterator result;

		std::vector<int> t1{ 1, 2, 3 };

		result = find_end(v.begin(), v.end(), t1.begin(), t1.end());
		if (result == v.end()) {
			std::cout << "subsequence not found\n";
		}
		else {
			std::cout << "last subsequence is at: "
				<< std::distance(v.begin(), result) << "\n";
		}

		std::vector<int> t2{ 4, 5, 6 };
		result = find_end(v.begin(), v.end(), t2.begin(), t2.end());
		if (result == v.end()) {
			std::cout << "subsequence not found\n";
		}
		else {
			std::cout << "last subsequence is at: "
				<< std::distance(v.begin(), result) << "\n";
		}

	}
	{
			const char sequence[] = "1001010100010101001010101";

			std::cout << std::boolalpha;
			std::cout << "Has 4 consecutive zeros: "
				<< consecutive_values(sequence, 4, '0') << '\n';
			std::cout << "Has 3 consecutive zeros: "
				<< consecutive_values(sequence, 3, '0') << '\n';
	}
	{
		auto v = get_random_vector();
		auto v2 = v;
		Yupei::rotate(v.begin(),v.begin() + v.size()/2,v.end());
		//std::cout << std::boolalpha << Yupei::is_permutation(RANGE(v), v2.begin());
		assert(Yupei::is_permutation(RANGE(v), v2.begin()));
	}
	{
		auto v = get_random_vector();
		insertion_sort(RANGE(v));
		assert(is_sorted(RANGE(v)));
	}
	{
		//heap test
		auto v = get_random_vector();
		make_heap(RANGE(v));
		assert(is_heap(RANGE(v)));
		v.push_back(get_random_number());
		push_heap(RANGE(v));
		assert(is_heap(RANGE(v)));
		pop_heap(RANGE(v));
		v.pop_back();
		assert(is_heap(RANGE(v)));
		sort_heap(RANGE(v));
		assert(is_sorted(RANGE(v)));
	}
	{
		auto v = get_random_vector();
		Yupei::stable_sort(RANGE(v));
		assert(is_sorted(RANGE(v)));
		Yupei::sort(RANGE(v), greater<>());
		assert(is_sorted(RANGE(v), greater<>()));
		auto is_even = [](const auto& v)
		{
			return v % 2 == 0;
		};
		Yupei::stable_partition(RANGE(v), is_even);
		assert(Yupei::is_partitioned(RANGE(v), is_even));

	}
	{
		auto v = get_random_vector();
		sort(RANGE(v));
		auto en = Yupei::unique(RANGE(v));
		for (auto it = v.begin(); it != en;++it)
			assert(Yupei::count(v.begin(), en, *it) == 1);
	}
	{
		std::vector<int> v{ 5, 6, 4, 3, 2, 6, 7, 9, 3 };
		insertion_sort(RANGE(v));
		Yupei::sort(RANGE(v));
		nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
		std::cout << "The median is " << v[v.size() / 2] << '\n';

		nth_element(v.begin(), v.begin() + 1, v.end(), greater<int>());
		std::cout << "The second largest element is " << v[1] << '\n';

	}
	{
		auto v = get_random_vector();
		auto nth = v.begin() + 10;
		nth_element(v.begin(),nth,v.end());
		auto n = *nth;
		sort(RANGE(v));
		assert(n == v[10]);
	}
	{
		char s[] = { 'a','b','c','\0' };
		//reverse(RANGE(s)-1);
		while (next_permutation(RANGE(s)-1))
		{
			std::cout << s << '\n';
		}
        PrintLine();
		reverse(RANGE(s) - 1);
		while (prev_permutation(RANGE(s)-1))
		{
			std::cout << s << '\n';
		}
	}
	{
		auto v = get_random_vector();
		sort(RANGE(v));
		auto p = equal_range(RANGE(v), 500);
		assert(any_of(v.begin(), p.first, [](const auto& x) {
			return x >= 500;
		}) == false);
	}
	std::getchar();
	return 0;
}