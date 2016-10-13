#include "Win32IPAddressParser.hpp"
#include "NtFunctions.hpp"
#include "Win32Exception.hpp"
#include "../../TypeAlias.hpp"
#include "WinDef.hpp"
#include <stdexcept>
#include <Windows.h>
#include <mstcpip.h>
#include <inaddr.h>
#include <in6addr.h>

namespace Yupei
{
    void IPv4AddressToString(IPv4Address address, std::u16string& buffer)
    {
        const char* const InvalidIPv4AddressStr = "Invalid IPv4 address!";
        const auto& addressBytes = address.Bytes;

        const in_addr addr = { static_cast<uchar>(addressBytes[0]), static_cast<uchar>(addressBytes[1]),
            static_cast<uchar>(addressBytes[2]), static_cast<uchar>(addressBytes[3]) };
        const auto& ntFunctions = GetNtFunctions();

        //If the length of the buffer pointed to by the AddressString parameter is not large enough 
        //to receive the string representation of the IPv4 address and port, RtlIpv4AddressToStringEx 
        //returns ERROR_INVALID_PARAMETER and sets the AddressStringLength parameter to the buffer length required. 

        ulong bufferLength = {};
        auto errCode = ntFunctions.RtlIpv4AddressToStringEx(&addr, 0, nullptr, &bufferLength);

        //如果 bufferLength 在调用后依旧为 0 说明 address 有问题。
        Try<std::invalid_argument>(bufferLength == 0, InvalidIPv4AddressStr);

        buffer.resize(static_cast<std::size_t>(bufferLength));
        errCode = ntFunctions.RtlIpv4AddressToStringEx(&addr, 0, reinterpret_cast<wchar_t*>(&buffer[0]), &bufferLength);
        
        //如果有错误，前面必然已经抛出异常，故这里必然是 Success。
        assert(errCode == NtFunctions::NtSuccess);
        assert(bufferLength > 0);

        //在写入的 buffer 中包含 '\0'，会导致判等出现奇怪的结果，故舍弃掉结尾的空字符。
        buffer.pop_back();
    }

    void IPv4StringToAddress(const char16_t* ipString, gsl::span<std::uint8_t> bytes, std::uint16_t& port)
    {
        const auto& ntFunctions = GetNtFunctions();
        const auto ipStr = reinterpret_cast<const wchar_t*>(ipString);
        ::in_addr v4Addr = {};
        ushort tempPort = {};
        Try<std::invalid_argument>(ntFunctions.RtlIpv4StringToAddressEx(ipStr, 0, &v4Addr, &tempPort) == NtFunctions::NtSuccess, 
            "Invalid IPv4 address string!");
        bytes[0] = static_cast<std::uint8_t>(v4Addr.s_net);
        bytes[1] = static_cast<std::uint8_t>(v4Addr.s_host);
        bytes[2] = static_cast<std::uint8_t>(v4Addr.s_lh);
        bytes[3] = static_cast<std::uint8_t>(v4Addr.s_impno);
        port = static_cast<std::uint16_t>(tempPort);
    }

    void IPv6AddressToString(IPv6Address address, std::uint32_t scopeId, std::u16string& buffer)
    {
        const char* const InvalidIPv6AddressStr = "Invalid IPv6 address!";
        ::in6_addr v6Addr;
        auto& desAddrBytes = v6Addr.s6_addr;
        auto& srcAddrBytes = address.Bytes;
        for (std::size_t i = 0; i < kIPv4AddressBytes; ++i)
            desAddrBytes[i] = static_cast<uchar>(srcAddrBytes[i]);

        const auto& ntFunctions = GetNtFunctions();
        ulong bufferLength = {};

        auto errCode = ntFunctions.RtlIpv6AddressToStringEx(&v6Addr, static_cast<ulong>(scopeId), 0, nullptr, &bufferLength);
        Try<std::invalid_argument>(bufferLength != 0, InvalidIPv6AddressStr);

        buffer.resize(static_cast<std::size_t>(bufferLength));
        errCode = ntFunctions.RtlIpv6AddressToStringEx(&v6Addr, static_cast<ulong>(scopeId), 0, reinterpret_cast<wchar_t*>(&buffer[0]), &bufferLength);

        assert(errCode == NtFunctions::NtSuccess);

        buffer.pop_back();
    }

    void IPv6StringToAddress(const char16_t* ipString, gsl::span<std::uint8_t> bytes, std::uint32_t& scope, std::uint16_t& port)
    {
        const auto& ntFunctions = GetNtFunctions();
        const auto ipStr = reinterpret_cast<const wchar_t*>(ipString);
        ::in6_addr v6Addr = {};
        ulong tempScope = {};
        ushort tempPort = {};
        Try<std::invalid_argument>(ntFunctions.RtlIpv6StringToAddressEx(ipStr, &v6Addr, &tempScope, &tempPort) == NtFunctions::NtSuccess,
            "Invalid IPv4 address string!");
        auto& addrBytes = v6Addr.s6_addr;
        for (std::size_t i = 0; i < kIPv6AddressBytes; ++i)
            bytes[i] = static_cast<std::uint8_t>(addrBytes[i]);
        scope = static_cast<std::uint32_t>(tempScope);
        port = static_cast<std::uint16_t>(tempPort);
    }
}