
#include "..\..\Source\Stl\MemoryReasource\memory_resource.h"
#include "..\..\Source\Custom\StopWatch.h"

#include <vector>
#include <string>
#include <iostream>
#include <cassert>

using namespace Yupei;

template<typename T>
using vector_t = std::vector<T, polymorphic_allocator<T>>;

int main()
{
	auto res = new monotonic_buffer_resource({});
	{		
		vector_t<int> v( memory_resource_ptr{ res });
		for (int i = 0;i < 999999;++i)
			v.push_back(i);
		for (int i = 0;i < 999999;++i) assert(v[i] == i);
	}
	{
		vector_t<int> v(memory_resource_ptr{ res });
		//naive benchmark
		using namespace Yupei;
		StopWatch sw;
		sw.Start();
		for (int i = 0;i < 999999;++i)
			v.push_back(i);
		std::cout << sw.Stop() << "\n";
		std::vector<int> v2;
		StopWatch sw2;
		sw2.Start();
		for (int i = 0;i < 999999;++i)
			v2.push_back(i);
		std::cout << sw2.Stop() << "\n";
	}
	delete res;
	getchar();
	//_CrtDumpMemoryLeaks();
}