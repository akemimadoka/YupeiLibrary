#pragma once

#include "..\String.hpp"
#include <cstdint>
#include <cassert>

namespace Yupei
{
	class IPAddress
	{
	public:
		IPAddress(std::uint32_t newAddress) noexcept
		{
			isIPv4_ = true;
			PrivateAddress(newAddress);
		}

		IPAddress()

		template<typename HashCode>
		friend void hash_value(HashCode& hashCode, const IPAddress& address) noexcept
		{
			
		}

		bool IsIPv4() const noexcept
		{
			return isIPv4_ == true;
		}

		bool IsIPv6() const noexcept
		{
			return !IsIPv4();
		}

		std::uint32_t PrivateAddress() const noexcept
		{
			assert(IsIPv4());
			return addressOrScopeId_;
		}

		void PrivateAddress(std::uint32_t address) noexcept
		{
			assert(IsIPv4());
			addressOrScopeId_ = address;
		}

		std::uint32_t PrivateScopeId() const noexcept
		{
			assert(IsIPv6());
			return addressOrScopeId_;
		}

		void PrivateScopeId(std::uint32_t id) noexcept
		{
			assert(IsIPv6());
			addressOrScopeId_ = id;
		}

	private:
		u8string toString_;
		std::uint32_t addressOrScopeId_;
		bool isIPv4_;
		std::size_t hashCode_;
	};
}