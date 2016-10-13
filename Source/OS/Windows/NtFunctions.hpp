#pragma once

#include "WinDef.hpp"
#include  "../../TypeAlias.hpp"
#include "NativeHandles.hpp"
#include "../../ObserverHandle.hpp"

namespace Yupei
{
    class NtFunctions;

    const NtFunctions& GetNtFunctions() noexcept;

    using RtlIpv4AddressToStringExType = long(*)(const ::in_addr*, ushort, wchar_t*, ulong*);
    using RtlIpv4StringToAddressExType = long(*)(const wchar_t*, uchar, ::in_addr*, ushort*);
    using RtlIpv6AddressToStringExType = long(*)(const ::in6_addr*, ulong, ushort, wchar_t*, ulong*);
    using RtlIpv6StringToAddressExType = long(*)(const wchar_t*, ::in6_addr*, ulong*, ushort*);

    class NtFunctions
    {
    public:
        enum : std::uint32_t { NtSuccess = 0 };

        long RtlIpv4AddressToStringEx(const ::in_addr*, ushort, wchar_t*, ulong*) const noexcept;
        long RtlIpv4StringToAddressEx(const wchar_t*, uchar, ::in_addr* address, ushort* port) const noexcept;
        long RtlIpv6AddressToStringEx(const ::in6_addr*, ulong, ushort, wchar_t*, ulong*) const noexcept;
        long RtlIpv6StringToAddressEx(const wchar_t*, ::in6_addr*, ulong*, ushort*) const noexcept;

    private:
        friend const NtFunctions& Yupei::GetNtFunctions() noexcept;

        NtFunctions();

        ObserverHandle<NativeModuleHandle> ntdllHandle_;
        mutable RtlIpv4AddressToStringExType pRtlIpv4AddressToStringEx_ = {};
        mutable RtlIpv4StringToAddressExType pRtlIpv4StringToAddressEx_ = {};
        mutable RtlIpv6AddressToStringExType pRtlIpv6AddressToStringEx_ = {};
        mutable RtlIpv6StringToAddressExType pRtlIpv6StringToAddressEx_ = {};
    };  
}