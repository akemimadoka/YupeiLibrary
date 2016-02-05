#include "list.h"
#include "vector.h"
#include "ForwardList.h"
#include <iostream>
#include <string>

using namespace Yupei;

int main()
{
	/*{
		list<int> l;
		list<int> l2;
		
		for (int i = 0;i < 10;++i)
		{
			l.push_back(i);
			l2.push_back(i + 2);
		}
		for (auto x : l)
			std::cout << x << " ";		
		l.merge(l2);
		std::cout << "\n";
		for (auto x : l)
			std::cout << x << " ";
		l.reverse();
		std::cout << "\n";
		for (auto x : l)
			std::cout << x << " ";
		std::cout << "\n";
		l.sort();
		for (auto x : l)
			std::cout << x << " ";
		auto l3 = l;
		std::cout << "\n";
		for (auto x : l3)
			std::cout << x << " ";
	}
	
	{
		vector<std::string> v;
		for (int i = 0;i < 1000;++i)
			v.push_back(std::to_string(i));		
        for (int i = 0;i < 1000;++i)
            v.insert(v.begin(), std::to_string(i));
        v.erase(v.begin());
        for (auto c : v)
            std::cout << c << " ";
        
        std::cout << "\n";
	}*/
    {
        forward_list<int> l1 = {1,2,3,4,5};
        forward_list<int> l2 = {10,11,12};

        l2.splice_after(l2.cbegin(), l1, l1.cbegin(), l1.cend());
        // not equivalent to l2.splice_after(l2.cbegin(), l1);

        for (int n : l1)
            std::cout << n << ' ';
        std::cout << '\n';

        for (int n : l2)
            std::cout << n << ' ';
        std::cout << '\n';

    }

	getchar();
    _CrtDumpMemoryLeaks();
	return 0;
}