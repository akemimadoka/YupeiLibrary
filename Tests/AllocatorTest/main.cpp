#include "..\..\Source\Stl\memory.h"
#include <memory>
#include <typeinfo>
#include <iostream>

template<typename T>
class SimpleAllocator
{
public:
	using value_type = T;
};


int main()
{
	int* a = new int;
	Yupei::pointer_traits<int*>::element_type{};
	Yupei::pointer_traits<std::shared_ptr<int>>::element_type;
	SimpleAllocator<int> sa{};
	Yupei::allocator_traits<SimpleAllocator<int>>::construct(sa, a, 0);
	std::cout << typeid(Yupei::allocator_traits<SimpleAllocator<int>>::pointer{}).name() << std::endl;;
	std::cout << typeid(Yupei::allocator_traits<SimpleAllocator<int>>::const_pointer{}).name() << std::endl;
	std::system("pause");
	//std::set_new_handler
	return 0;
}