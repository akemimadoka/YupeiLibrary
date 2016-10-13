#pragma once

#include <cstddef>
#include <array>

namespace Yupei
{
    enum : std::size_t { kIPv4AddressBytes = 4, kIPv6AddressBytes = 16 };

    union IPv4Address
    {
        std::array<std::uint8_t, kIPv4AddressBytes> Bytes;
        std::uint32_t Address;
        //uchar alias[kIPv4AddressBytes];
    };

    union IPv6Address
    {
        std::array<std::uint16_t, kIPv6AddressBytes / 2> U16Parts;
        std::array<std::uint8_t, kIPv6AddressBytes> Bytes;
        std::array<std::uint64_t, kIPv6AddressBytes / 4> U64Parts;
        //uchar alias[kIPv6AddressBytes];
    };
}
