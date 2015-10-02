#include "..\..\Source\Stl\iterator.h"
#include <vector>
#include <memory>

struct ContiIterator
{
	using iterator_category = Yupei::contiguous_iterator_tag;
	using value_type = int;
	using pointer = value_type*;
	int operator*() { return 0; }
	int* operator->() { return nullptr; }
	ContiIterator operator++(int) { return *this; }
	ContiIterator& operator++() { return *this; }
	ContiIterator operator+(std::ptrdiff_t) { return *this; }
	ContiIterator operator-(std::ptrdiff_t) { return *this; }

};

int main()
{
	int i;
	Yupei::IteratorCategory<int*>{};
	Yupei::ValueType<int*>{};
	Yupei::iterator_traits<int*>::difference_type{};
	Yupei::iterator_traits<int*>::pointer x{};
	Yupei::iterator_traits<std::vector<int>::iterator>::reference ri = i;
	//Yupei::iterator_traits<Yupei::ostream_iterator<int>>::pointer i;
	ri = 4;
	Yupei::is_contiguous_iterator<ContiIterator>::value;
	Yupei::is_pod_iterator<ContiIterator>::value;
	//Yupei::po
	//Yupei::declval<std::unique_ptr<int>>().operator->();
	return 0;
}