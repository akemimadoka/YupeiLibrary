#pragma once

#include "TypeTraits.h"

namespace Yupei
{
	namespace Internal
	{
		template<typename T>
		constexpr auto abs(T i) -> std::enable_if_t<std::is_integral<T>::value, T>
		{
			return i < T(0) ? -i : i;
		}

		template<typename M, typename N = M>
		using common_int_t = std::enable_if_t<
            std::is_integral<M>::value &&
            std::is_integral<N>::value,
            std::common_type_t<M, N>> ;
	}

	template<typename M,typename N>
	constexpr Internal::common_int_t<M,N> gcd(M m, N n)
	{
		return n == 0 ? Internal::abs(m) : gcd(n, Internal::abs(m) % Internal::abs(n));
	}

	template<typename M, typename N>
	constexpr Internal::common_int_t<M, N> lcm(M m, N n)
	{
		return m == 0 || n == 0 ? 0 : (Internal::abs(m) / gcd(m, n) * Internal::abs(n));
	}
}