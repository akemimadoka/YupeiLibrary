#include "HashHelpers.hpp"
#include <cmath>
#include <algorithm>
#include <climits>
#include <iterator> //for std::begin(T(&)[N])

namespace Yupei
{
	namespace Internal
	{
		constexpr std::uint32_t HashHelpers::primes[80];

		bool HashHelpers::IsPrime(std::size_t candidate) noexcept
		{
			if ((candidate & 1u) != 0) //奇数
			{
				const auto limit = std::sqrt(candidate);
				for (std::size_t divisor = 3; divisor <= limit; divisor += 2)
					if (candidate % divisor == 0)
						return false;
				return true;
			}
			return candidate == 2;
		}

		std::size_t HashHelpers::GetPrime(std::size_t min) noexcept
		{
			if (min <= static_cast<std::size_t>(*std::prev(std::end(primes))))
			{
				const auto it = std::lower_bound(std::begin(primes), std::end(primes), min);
				if (it != std::end(primes)) return *it;
			}

			for (auto i = (min | 1); i < SIZE_MAX; i += 2)
				if (IsPrime(i)) return i;
			return min;
		}

		std::size_t HashHelpers::ExpandPrime(std::size_t oldSize) noexcept
		{
			auto newSize = oldSize << 1;
			if (newSize < oldSize) return SIZE_MAX;
			return GetPrime(newSize);
		}
	}
}


