#pragma once

#include "..\String.hpp"
#include <cstdint>
#include <cassert>

namespace Yupei
{
	class IPAddress
	{
	public:
		IPAddress(std::uint32_t newAddress)
		{

		}

        std::uint32_t PrivateAddress() const noexcept
        {

        }

	private:
		u8string toString_;
        std::uint32_t addressOrScopeId_;
        bool isIPv4_;

	};
}