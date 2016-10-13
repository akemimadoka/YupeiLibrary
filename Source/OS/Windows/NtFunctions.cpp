#include "NtFunctions.hpp"
#include "Win32Exception.hpp"
#include "../../Extensions.hpp"
#include <Windows.h>
#include <cassert>
#include <functional>

#define RETURN_FP(FuncName, ...)                                \
auto& fp = TOKEN_PASTE(TOKEN_PASTE(p, FuncName), _);            \
if(fp == nullptr)                                               \
{                                                               \
    fp = reinterpret_cast<TOKEN_PASTE(FuncName, Type)>(::GetProcAddress(ntdllHandle_, STR(FuncName)));    \
    assert(fp != nullptr);                                      \
}                                                               \
return std::invoke(fp, __VA_ARGS__);  

namespace Yupei
{
    long NtFunctions::RtlIpv4AddressToStringEx(const::in_addr* address, 
        ushort port, wchar_t* addressString, ulong* addressStringLength) const noexcept
    {
        RETURN_FP(RtlIpv4AddressToStringEx, address, port, addressString, addressStringLength)
    }

    long NtFunctions::RtlIpv4StringToAddressEx(const wchar_t* addressString, uchar strict, 
        ::in_addr* address, ushort* port) const noexcept
    {
        RETURN_FP(RtlIpv4StringToAddressEx, addressString, strict, address, port)
    }

    long NtFunctions::RtlIpv6AddressToStringEx(const::in6_addr* address, ulong scopeId, 
        ushort port, wchar_t* addressString, ulong* addressStringLength) const noexcept
    {
        RETURN_FP(RtlIpv6AddressToStringEx, address, scopeId, port, addressString, addressStringLength)
    }

    long NtFunctions::RtlIpv6StringToAddressEx(const wchar_t* addressString, ::in6_addr* address, ulong* scopeId, ushort* port) const noexcept
    {
        RETURN_FP(RtlIpv6StringToAddressEx, addressString, address, scopeId, port)
    }

    NtFunctions::NtFunctions()
    {
        ntdllHandle_ = ::GetModuleHandle(L"ntdll.dll");
        assert(ntdllHandle_);
    }

    const NtFunctions& GetNtFunctions() noexcept
    {
        static NtFunctions ntFunctions;
        return ntFunctions;
    }
}