
#include "..\..\Source\Stl\MemoryReasource\memory_resource.h"
#include "..\..\Source\Custom\StopWatch.h"

#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <cassert>

using namespace Yupei;

template<typename T>
using vector_t = std::vector<T, polymorphic_allocator<T>>;

template<typename T>
using list_t = std::list<T, polymorphic_allocator<T>>;

int main()
{
	{
		//_CrtSetBreakAlloc(146);
		auto res = new monotonic_buffer_resource({});
		auto pool = new unsynchronized_pool_resource({}, memory_resource_ptr{ res });
		{
			vector_t<int> v(memory_resource_ptr{ res });
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
		{


			list_t<int> v(memory_resource_ptr{ pool });
			//naive benchmark
			using namespace Yupei;
			StopWatch sw;
			sw.Start();
			for (int i = 0;i < 999999;++i)
				v.push_back(i);
			std::cout << sw.Stop() << "\n";
			list_t<int> v2;
			StopWatch sw2;
			sw2.Start();
			for (int i = 0;i < 999999;++i)
				v2.push_back(i);
			std::cout << sw2.Stop() << "\n";
		}
		delete pool;
		delete res;
	}
	
	getchar();
	_CrtDumpMemoryLeaks();
}