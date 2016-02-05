#include "variant.h"
#include "shapes.h"

#include <iostream>
#include <string>

int main()
{
	using Yupei::variant;
	using Yupei::match;
	using Yupei::make_overload;

	variant<int, double, std::string> v ;

	auto overloads = make_overload(
		[](int& x) {std::cout << "int\n";},
		[](double& x) {std::cout << "double\n";},
		[](std::string& x) {std::cout << "string " << x << "\n";});

	v = 2.0;
	v.apply<void>(overloads);
	v = "Hello!";
	v.apply<void>(overloads);
	v = 2;
	v.apply<void>(overloads);



	{
		Shape s;
		s = Circle{ 2.0 };
		std::cout << GetPerimeter(s);
	}
	getchar();
}