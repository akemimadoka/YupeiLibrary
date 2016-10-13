#pragma once

#include "../../TypeAlias.hpp"
#include "../../Network/Basis.hpp"
#include <cstdint>
#include <string_view>
#include <span.h>

namespace Yupei
{ 
    extern void IPv4AddressToString(IPv4Address address, std::u16string& buffer);
    extern void IPv6AddressToString(IPv6Address address, std::uint32_t scopeId, std::u16string& buffer);
    //��ʹ�� string_view ����Ϊ�� API ��Ҫ NULL-terminated string��
    //û��Ҫʹ�� u16string���ַ�����������...��
    extern void IPv4StringToAddress(const char16_t* ipString, gsl::span<std::uint8_t> bytes, std::uint16_t& port);
    extern void IPv6StringToAddress(const char16_t* ipString, gsl::span<std::uint8_t> bytes, std::uint32_t& scope, std::uint16_t& port);
}