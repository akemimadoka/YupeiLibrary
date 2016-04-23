#pragma once

#include <array>

namespace Yupei
{
	namespace Internal
	{
		template<typename T, std::size_t... I>
		struct ArrayImp;

		template<typename T, std::size_t I, std::size_t... Rest>
		struct ArrayImp<T, I, Rest...>
		{
			using type = std::array<typename ArrayImp<T, Rest...>::type, I> ;
		};

		template<typename T, std::size_t I>
		struct ArrayImp<T, I>
		{
			using type = std::array<T, I>;
		};
	}
	
	template<typename T, std::size_t... I>
	using array = typename Internal::ArrayImp<T, I...>::type;
}