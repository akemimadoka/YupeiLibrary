#pragma once

#include "../OS/IPAddressParser.hpp"
#include "Basis.hpp"
#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <optional>
#include <variant>
#include <string_span.h>


namespace Yupei
{
    class InvalidIPOperation : std::logic_error
    {
    public:
        InvalidIPOperation()
            : std::logic_error{"Invalid IP operation!"}
        {}

        InvalidIPOperation(gsl::cstring_span<> message)
            : std::logic_error{message.data()}
        {}
    };

    class IPAddress
    {
    private:
        enum : std::size_t
        {
            kIPv4Index = 0,
            kIPv6Index = 1
        };

        enum : std::size_t
        {
            kAddressIndex = 0,
            kScopeId = 1
        };

    public:
        //Construct an IPv4 address.
        IPAddress(std::uint32_t newAddress) noexcept
        {
            isIPv4_ = true;
            PrivateAddress(newAddress);
        }

        IPAddress(IPv4Address address) noexcept
        {
            isIPv4_ = true;
            ipv4Address_ = address;
        }

        IPAddress(const IPv6Address& address, std::uint32_t scopeId = 0) noexcept
        {
            isIPv4_ = false;
            ipv6Address_ = address;
            scopeId_ = scopeId;
        }

        IPAddress(array_view<std::uint8_t> address)
        {
            auto byteCount = address.size();
            if (byteCount == kIPv4AddressBytes)
            {
                isIPv4_ = true;
                ipv4Address_ = ConstructIPv4(address);
            }
            else if (byteCount == kIPv6AddressBytes)
            {
                isIPv4_ = false;
                scopeId_ = {};
                std::copy(cbegin(address), cend(address), begin(ipv6Address_.Bytes));
            }
            else
                throw std::invalid_argument { "Invalid byte count of address!" };
        }

        static std::optional<IPAddress> TryParse(u16string_view sv)
        {
            return P
        }

        //TODO.
        template<typename HashCode>
        friend void hash_value(HashCode& hashCode, const IPAddress& address) noexcept
        {

        }

        bool IsIPv4() const noexcept
        {
            return address_.index() == kIPv4Index;
        }

        bool IsIPv6() const noexcept
        {
            return address_.index() == kIPv6Index;
        }

        IPv4Address GetPrivateAddress() const noexcept
        {
            
        }

        void SetPrivateAddress(std::uint32_t address) noexcept
        {
            ipv4Address_.Address = address;
        }

        std::uint32_t PrivateScopeId() const noexcept
        {
            return scopeId_;
        }

        void PrivateScopeId(std::uint32_t id)
        {
            try
            {
                std::get<kScopeId>(std::get<kIPv6Index>(address_)) = id;
            }
            catch (const std::bad_variant_access&)
            {
                throw InvalidIPOperation{};
            }
        }

    private:
        static IPv4Address ConstructIPv4(array_view<std::uint8_t> addressBytes) noexcept
        {
            using std::begin;
            assert(addressBytes.size() == kIPv4AddressBytes);
            IPv4Address address;
            std::copy(cbegin(addressBytes), cend(addressBytes), begin(address.Bytes));
        }

        std::variant<IPv4Address, std::pair<IPv6Address, std::uint32_t>> address_;       
        std::size_t hashCode_;
    };
}