#pragma once

#include <cstddef>
#include <utility>

namespace Yupei
{
	template<typename Func, typename... ParamsT>
	void repeat_n(std::size_t n, Func func, ParamsT&&... params)
	{
		for (std::size_t i = 0; i < n; ++i)
			func(std::forward<ParamsT>(params)...);
	}
}